#include <stdio.h>
#include <string.h>
#include <endian.h>
#include <time.h>
#include "common.h"
#include "radio-util.h"
#include <pthread.h>
#include <ctype.h>

uint32_t MAX_UDP_PACKET_SIZE = 65507;

char DEFAULT_NAME[] = "Nienazwany Nadajnik";

char* MCAST_ADDR = NULL;
uint32_t DATA_PORT = 28675;
uint32_t PSIZE = 512;
uint32_t CTRL_PORT = 38675;
uint32_t FSIZE = 128 * 1024;
uint32_t RTIME = 250;
char* NAZWA = DEFAULT_NAME;

char* resend_nr;    // Marks which packets need to be resent.
char** history;     // Tracked packet history of size (FSIZE).
uint32_t history_size, first_packet_n = 0, first_packet_pos = 0, n_of_packets = 0;
pthread_mutex_t mutex;

int socket_fd, flags = 0;
struct sockaddr_in send_address;

void* comm_receiver(void*);

void* retransmission_sender(void*);

void try_lookup(char*, int, struct sockaddr_in);

void try_rexmit(char*, int);

int main(int argc, char* argv[]){
    if(argc % 2 != 1){
        print_sender_usage_info(argv[0]);
        return 1;
    }
    for(int i = 1; i < argc; i+=2){
        if(strcmp(argv[i], "-a") == 0){
            MCAST_ADDR = argv[i + 1];
        } else if(strcmp(argv[i], "-p") == 0){
            PSIZE = read_unsigned_number(argv[i + 1]);
        } else if(strcmp(argv[i], "-P") == 0){
            DATA_PORT = read_port(argv[i + 1]);
        }  else if(strcmp(argv[i], "-C") == 0){
            CTRL_PORT = read_port(argv[i + 1]);
        } else if(strcmp(argv[i], "-n") == 0){
            NAZWA = argv[i + 1];
        } else if(strcmp(argv[i], "-R") == 0){
            RTIME = read_unsigned_number(argv[i + 1]);
        } else if(strcmp(argv[i], "-f") == 0){
            FSIZE = read_unsigned_number(argv[i + 1]);
        } else{
            print_sender_usage_info(argv[0]);
            return 1;
        }
    }
    if(MCAST_ADDR == NULL){
        fprintf(stderr, "No receiver address supplied.\n");
        return 1;
    }

    int correct_name = 0;
    for(size_t i = 0; i < strlen(NAZWA); i++){
        if(!isblank(NAZWA[i]))
            correct_name = 1;
    }
    if(!correct_name){
        fprintf(stderr, "Station name can't be blank\n");
        return 1;
    }

    if(PSIZE == 0){
        fprintf(stderr, "PSIZE can't be 0\n");
        return 1;
    }

    if(PSIZE > MAX_UDP_PACKET_SIZE - 16){
        fprintf(stderr, "Packet size bigger than max possible UDP packet size\n");
        return 1;
    }

    send_address.sin_family = AF_INET;
    send_address.sin_port = htons(DATA_PORT);
    if (inet_aton(MCAST_ADDR, &send_address.sin_addr) == 0) {
        fprintf(stderr, "%s is an invalid multicast address\n", MCAST_ADDR);
        return 1;
    }

    pthread_mutex_init(&mutex, NULL);
    history_size = FSIZE / (PSIZE + 16);
    resend_nr = malloc(history_size);
    history = malloc(history_size * sizeof(char*));
    for(uint32_t i = 0; i < history_size; i++){
        history[i] = malloc(PSIZE + 16);
    }
    uint8_t* message = malloc(PSIZE + 16);
    uint32_t read;

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        PRINT_ERRNO();
    }
    set_port_reuse(socket_fd);

    int optval = 1;
    CHECK_ERRNO(setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, (void *) &optval, sizeof optval));
    optval = 4;
    CHECK_ERRNO(setsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &optval, sizeof optval));

    int flags = 0;
    uint64_t session_id = htobe64(time(NULL));
    uint64_t first_byte_num = 0, fbn_ne;
    memcpy(message, &session_id, sizeof(uint64_t));

    pthread_attr_t comm_attr;
    pthread_attr_init(&comm_attr);
    pthread_t comm_rec_thread;
    pthread_create(&comm_rec_thread, &comm_attr, comm_receiver, NULL);

    clock_t start_of_cycle = clock();
    clock_t current_time;

    do {
        read = fread(message + 16, 1, PSIZE, stdin);
        first_byte_num += read;

        fbn_ne = htobe64(first_byte_num - read);
        memcpy(message + 8, &fbn_ne, 8);

        if(read == PSIZE)
            sendto(socket_fd, message, PSIZE + 16, flags, (const struct sockaddr*)&send_address, sizeof(send_address));
        

        pthread_mutex_lock(&mutex);
        if(history_size == 0){
            pthread_mutex_unlock(&mutex);
            continue;
        } if(n_of_packets < history_size) {
            char* node = history[n_of_packets];
            memcpy(node, message, PSIZE + 16);
            n_of_packets++;
        } else {
            int pos = (first_packet_pos + history_size) % history_size;
            char* node = history[pos];
            memcpy(node, message, PSIZE + 16);
            resend_nr[pos] = 0;
            first_packet_n += PSIZE;
            first_packet_pos = (first_packet_pos + 1) % history_size;
        }

        current_time = clock();

        if(((double)(current_time - start_of_cycle)) / CLOCKS_PER_SEC >= RTIME) {
            pthread_attr_t resend_attr;
            pthread_attr_init(&resend_attr);
            pthread_t resend_thread;
            pthread_create(&resend_thread, &resend_attr, retransmission_sender, NULL);

            pthread_join(resend_thread, NULL);

            start_of_cycle = current_time;
        }

        pthread_mutex_unlock(&mutex);

    } while(read == PSIZE);

}

/* 
Receives the LOOKUP and REXMIT messages. Responds to the LOOKUP
and modifies the 'resend_nr' upon receiving REXMIT.
*/
void* comm_receiver(__attribute__((unused)) void* data){
    int socket_fd = bind_socket(CTRL_PORT);
    char* buffer = malloc(MAX_UDP_PACKET_SIZE);
    struct sockaddr_in sender_address;
    socklen_t len = sizeof(sender_address);

    uint32_t read_bytes;

    while(true){
        read_bytes = recvfrom(socket_fd, buffer, MAX_UDP_PACKET_SIZE, 0, (struct sockaddr*)&sender_address, &len);

        try_lookup(buffer, read_bytes, sender_address);
        try_rexmit(buffer, read_bytes);
    }

    return NULL;
}

/*
Retransmits the requested packages.
*/
void* retransmission_sender(__attribute__((unused)) void* data){
    for(uint32_t i = first_packet_n; i < history_size; i++){
        int j = i % history_size;
        char* message = history[j];
        if(resend_nr[j] == 1)
            sendto(socket_fd, message, PSIZE + 16, flags, (const struct sockaddr*)&send_address, sizeof(send_address));
        resend_nr[j] = 0;
    }

    return NULL;
}

void try_lookup(char* buffer, int read_bytes, struct sockaddr_in sender_address){
    if(read_bytes < 19)
        return;
    if(strncmp(buffer, "ZERO_SEVEN_COME_IN\n", 19) == 0){
        char header[] = "BOREWICZ_HERE";
        char dataport_str[5];
        memset(dataport_str, 0, 5);
        sprintf(dataport_str, "%d", DATA_PORT);
        int message_len = strlen(header) + strlen(MCAST_ADDR) + strnlen(dataport_str, 5) + strlen(NAZWA) + 4;
        char* message = malloc(message_len);
        int curlen = 0;
        memcpy(message, header, strlen(header));
        curlen += strlen(header);
        memset(message + curlen, ' ', 1);
        curlen += 1;
        memcpy(message + curlen, MCAST_ADDR, strlen(MCAST_ADDR));
        curlen += strlen(MCAST_ADDR);
        memset(message + curlen, ' ', 1);
        curlen += 1;
        memcpy(message + curlen, dataport_str, strnlen(dataport_str, 5));
        curlen += strnlen(dataport_str, 5);
        memset(message + curlen, ' ', 1);
        curlen += 1;
        memcpy(message + curlen, NAZWA, strlen(NAZWA));
        curlen += strlen(NAZWA);
        memset(message + curlen, '\n', 1);

        int reply_socket = socket(AF_INET, SOCK_DGRAM, 0);
        sendto(reply_socket, message, message_len, 0, (const struct sockaddr*)&sender_address, sizeof(sender_address));
        free(message);
    }
}

void try_rexmit(char* buffer, int read_bytes){
    if(read_bytes < 15)
        return;
    if(strncmp(buffer, "LOUDER_PLEASE ", 14) != 0)
        return;

    // At first we loop through the message to see whether it is of correct format.
    int correct_format = 1;
    if(!isdigit(buffer[14]))
        correct_format = 0;
    for(int i = 15; i < read_bytes - 2; i++){
        if(!isdigit(buffer[i]) && buffer[i] != ',')
            correct_format = 0;
    }
    if(!isdigit(buffer[read_bytes - 2]) || buffer[read_bytes - 1] != '\n')
        correct_format = 0;
    if(!correct_format)
        return;

    uint32_t current_num = 0;
    pthread_mutex_lock(&mutex);
    for(int i = 15; i < read_bytes; i++){
        if(isdigit(buffer[i]))
            current_num = 10 * current_num + buffer[i] - '0';
        else{
            if(current_num >= first_packet_n){
                // The requested packet is still tracked in our packet history.
                int diff = (current_num - first_packet_n) / PSIZE;
                resend_nr[(first_packet_pos + diff) % history_size] = 1;
            }
            current_num = 0;
        }
    }
    pthread_mutex_unlock(&mutex);
}