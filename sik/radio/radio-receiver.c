#include "common.h"
#include "radio-util.h"
#include <pthread.h>
#include <time.h>

struct Station {
    char* name;
    char* addr;
    uint16_t port;
    struct sockaddr_in address;
    clock_t last_reply;
};

uint32_t MAX_UDP_PACKET_SIZE = 65507;

char* DISCOVER_ADDR = "255.255.255.255";
uint32_t PSIZE = 0;
uint32_t BSIZE = 65536;
uint32_t CTRL_PORT = 38675;
uint32_t UI_PORT = 38675;
uint32_t RTIME = 250;
char* FAV_STATION = NULL;

char* SOURCE_ADDR = NULL;
uint32_t DATA_PORT = 0;
char* NAZWA = NULL;
struct sockaddr_in source_address;

uint8_t *buffer, *message;

/* Has the packet corresponding to the byte been received */
uint8_t* received;

uint32_t read_bytes, q_start = 0, q_max_len;
uint64_t session_id, byte_zero, first_byte_num;
int flags = 0;
bool session_change = false;
uint64_t read_session_id, read_first_byte_num;
int music_socket;

int new_session = 0;

#define MAX_STATIONS 1000
uint16_t num_of_stations = 0;

struct Station stations[MAX_STATIONS];

void read_packet(int socket_fd);

void* lookup_sender(void*);

void* reply_receiver(void*);

void* telnet_comm(void*);

void* rexmit_sender(void*);

int main(int argc, char* argv[]){
    if(argc % 2 != 1){
        print_receiver_usage_info(argv[0]);
        return 1;
    }
    for(int i = 1; i < argc; i+=2){
        if(strcmp(argv[i], "-a") == 0){
            DISCOVER_ADDR = argv[i + 1];
        } else if(strcmp(argv[i], "-C") == 0){
            CTRL_PORT = read_port(argv[i + 1]);
        } else if(strcmp(argv[i], "-U") == 0){
            UI_PORT = read_port(argv[i + 1]);
        } else if(strcmp(argv[i], "-n") == 0){
            FAV_STATION = argv[i + 1];
        } else if(strcmp(argv[i], "-b")){
            BSIZE = read_unsigned_number(argv[i + 1]);
        } else if(strcmp(argv[i], "-R") == 0){
            RTIME = read_unsigned_number(argv[i + 1]);
        } else{
            print_receiver_usage_info(argv[0]);
            return 1;
        }
    }
    if(DISCOVER_ADDR == NULL){
        fprintf(stderr, "No discover address supplied.\n");
        return 1;
    }

    buffer = calloc(BSIZE, 1);
    message = calloc(MAX_UDP_PACKET_SIZE, 1);
    
    // Create the thread sending LOOKUP messages.
    pthread_attr_t lookup_attr;
    pthread_attr_init(&lookup_attr);
    pthread_t lookup_thread;
    pthread_create(&lookup_thread, &lookup_attr, lookup_sender, NULL);

    // Create the thread listening for REPLY.
    pthread_attr_t reply_attr;
    pthread_attr_init(&reply_attr);
    pthread_t reply_thread;
    pthread_create(&reply_thread, &reply_attr, reply_receiver, NULL);

    // Create the thread communicating with TELNET.
    pthread_attr_t telnet_attr;
    pthread_attr_init(&telnet_attr);
    pthread_t telnet_thread;
    pthread_create(&telnet_thread, &telnet_attr, telnet_comm, NULL);


    while(true){
        while(NAZWA == NULL){
            // Do nothing.
        }

        read_packet(music_socket);

        if(read_first_byte_num >= first_byte_num + BSIZE * 3 / 4 && !new_session){
            for(uint32_t j = 0; j < PSIZE; j++){
                printf("%c", buffer[q_start * PSIZE + j]);
            }
            memset(buffer + q_start * PSIZE, 0, PSIZE);
            if(received[q_start] == 0)
                session_change = 1;
            else
                received[q_start] = 0;
            q_start = (q_start + 1) % q_max_len;
            first_byte_num += PSIZE;
        }
    }
}

void read_packet(int socket_fd){
    socklen_t len = sizeof(source_address);
    struct sockaddr_in sender_address;
    do {
        if(NAZWA == NULL)
            return;
        read_bytes = recvfrom(socket_fd, message, MAX_UDP_PACKET_SIZE, flags, (struct sockaddr*)&sender_address, &len);
    } while(sender_address.sin_addr.s_addr != source_address.sin_addr.s_addr && NAZWA != NULL);

    if (PSIZE != read_bytes - 16){
        PSIZE = read_bytes - 16;
        q_max_len = BSIZE / PSIZE;
        free(received);
        received = calloc(q_max_len, 1);
        received[0] = 1;
        memset(buffer, 0, BSIZE);
        new_session = 1;
        q_start = 0;
    } else {
        new_session = 0;
    }

    if(NAZWA == NULL)
        return;

    memcpy(&read_session_id, message, 8);
    read_session_id = be64toh(read_session_id);
    memcpy(&read_first_byte_num, message + 8, 8);
    read_first_byte_num = be64toh(read_first_byte_num);
    if(new_session){
        byte_zero = read_first_byte_num;
        first_byte_num = read_first_byte_num;
    }

    if(read_first_byte_num >= first_byte_num && read_session_id == session_id){
        for(uint32_t i = 0; i < q_max_len && first_byte_num + i * PSIZE < read_first_byte_num; i++){
            if(received[(q_start + i) % q_max_len] == 0){
                pthread_attr_t rex_attr;
                pthread_attr_init(&rex_attr);
                pthread_attr_setdetachstate(&rex_attr, PTHREAD_CREATE_DETACHED);
                int* rex_arg = malloc(sizeof(uint32_t));
                memset(rex_arg, first_byte_num + i * PSIZE, 1);
                pthread_t rex_thread;
                pthread_create(&rex_thread, &rex_attr, rexmit_sender, rex_arg);

                // START A NEW THREAD SENDING REXMIT
                fprintf(stderr, "MISSING: BEFORE %ld EXPECTED %ld\n", read_first_byte_num, first_byte_num + i * PSIZE);
            }
        }
    }

    if(read_session_id > session_id){
        /* prepare for the new session */
        session_change = true;
        session_id = read_session_id;
        memset(buffer, 0, BSIZE);
        memset(received, 0, q_max_len);
        first_byte_num = read_first_byte_num;
        memcpy(buffer, message + 16, PSIZE);
        q_start = 0;
        byte_zero = read_first_byte_num;
    } else if(read_session_id < session_id) {
        /* ignore previous session packets */
        return;
    } else if(read_first_byte_num >= first_byte_num + q_max_len * PSIZE){
        if(first_byte_num + q_max_len * PSIZE * 2 <= read_first_byte_num){
            /* Clearing the whole buffer. */
            memset(buffer, 0, BSIZE);
            memset(received, 0, q_max_len);
            memcpy(buffer + ((q_start + (read_first_byte_num - first_byte_num) / PSIZE) % q_max_len) * PSIZE, message + 16, PSIZE);
            q_start = (q_start + (read_first_byte_num - first_byte_num) / PSIZE) % q_max_len;
            first_byte_num = read_first_byte_num;
            received[(q_start + (read_first_byte_num - first_byte_num) / PSIZE) % q_max_len] = 1;
        } else{
            /* Clearing only a part. */
            uint32_t position = (q_start + (read_first_byte_num - first_byte_num) / PSIZE) % q_max_len;
            if(position >= q_start){
                memset(buffer + q_start * PSIZE, 0, (position - q_start) * PSIZE);
                memset(received + q_start, 0, position - q_start);
            } else{
                memset(buffer + q_start * PSIZE, 0, (q_max_len - q_start) * PSIZE);
                memset(buffer, 0, position * PSIZE);

                memset(received + q_start, 0, q_max_len - q_start);
                memset(received, 0, position);
            }
            first_byte_num += PSIZE;
            q_start = (position + 1) % q_max_len;

            memcpy(buffer + position * PSIZE, message + 16, PSIZE);
            received[position] = 1;
        }
    } else if(read_first_byte_num < first_byte_num){
        /* ignore previous missed packets */
        return;
    } else {
        uint32_t position = (q_start + (read_first_byte_num - first_byte_num) / PSIZE) % q_max_len;
        memcpy(buffer + position * PSIZE, message + 16, PSIZE);
        received[position] = 1;
    }
}

void* lookup_sender(__attribute__((unused)) void* data){
    char lookup_message[] = "ZERO_SEVEN_COME_IN";

    int lookup_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in send_address;
    send_address.sin_family = AF_INET;
    send_address.sin_port = htons(CTRL_PORT);
    inet_aton(DISCOVER_ADDR, &send_address.sin_addr);

    sendto(lookup_socket_fd, lookup_message, strlen(lookup_message), 0, (const struct sockaddr*)&send_address, sizeof(send_address));

    clock_t start_of_cycle = clock();
    clock_t current_time;

    while(true){
        current_time = clock();
        if(((double)(current_time - start_of_cycle)) / CLOCKS_PER_SEC >= 5.0) {
            sendto(lookup_socket_fd, lookup_message, strlen(lookup_message), 0, (const struct sockaddr*)&send_address, sizeof(send_address));
            start_of_cycle = current_time;
        }
    }

    return NULL;
}

void* telnet_comm(__attribute__((unused)) void* data){
    return NULL;
}

void* reply_receiver(__attribute__((unused)) void* data){
    int listening_socket = bind_socket(CTRL_PORT);
    char header[] = "BOREWICZ_HERE";

    char* reply_message = calloc(MAX_UDP_PACKET_SIZE, 1);
    uint32_t read;
    while(true){
        read = receive_message(listening_socket, reply_message, MAX_UDP_PACKET_SIZE, 0);
        
        if(read < strlen(header) + 7)   // 3 spaces + min 3 per every part of message + 1 new line sign.
            continue;

        if(strncmp(header, reply_message, strlen(header)) != 0)
            continue;

        char* read_data_port = NULL;
        char* read_addr = NULL;
        char* read_name = NULL;

        uint32_t start = strlen(header) + 1, end;
        for(end = start; end < read; end++){
            if(reply_message[end] == ' '){
                read_addr = calloc(end - start + 1, 1);
                memcpy(read_addr, reply_message + start, end - start);
                break;
                start = end + 1;
            }
        }

        for(end = start; end < read; end++){
            if(reply_message[end] == ' '){
                read_data_port = calloc(end - start + 1, 1);
                memcpy(read_data_port, reply_message + start, end - start);
                break;
                start = end + 1;
            }
        }

        for(end = start; end < read; end++){
            if(reply_message[end] == '\n'){
                read_name = calloc(end - start + 1, 1);
                memcpy(read_name, reply_message + start, end - start);
                break;
            }
        }

        uint32_t port = strtoul(read_data_port, NULL, 10);
        if(port > UINT16_MAX){
            free(read_data_port);
            free(read_addr);
            free(read_name);
            continue;
        }

        int existing = 0;
        for(uint16_t i = 0; i < num_of_stations; i++){
            struct Station* station = stations + i;
            if(strcmp(station->addr, read_addr) == 0 && strcmp(station->name, read_name) == 0 && station->port == port){
                station->last_reply = clock();
                existing = 1;
                free(read_data_port);
                free(read_addr);
                free(read_name);
                break;
            }
        }

        if(!existing){
            struct Station* station = stations + num_of_stations;
            station->addr = read_addr;
            station->name = read_name;
            station->port = port;
            station->address = get_send_address(read_addr, (uint16_t)read_port(read_data_port));
            station->last_reply = clock();
        } 
    }
}

void* rexmit_sender(void* data){
    uint32_t requested = *((uint32_t*)data);
    free(data);

    clock_t start = clock();
    clock_t current_time;
    
    char header[] = "LOUDER_PLEASE";
    char num_str[10];
    sprintf(num_str, "%d", requested);

    char* rex_message = malloc(strlen(header) + strlen(num_str) + 2);
    memcpy(rex_message, header, strlen(header));
    memset(rex_message + strlen(header), ' ', 1);
    memcpy(rex_message + strlen(header) + 1, num_str, strlen(num_str));
    memset(rex_message + strlen(header) + strlen(num_str) + 1, '\n', 1);

    int rex_socket = bind_socket(CTRL_PORT);

    while(true) {
        uint32_t index = q_start + (first_byte_num - requested) / PSIZE;
        index %= q_max_len;

        if((received[index] == 1) || (first_byte_num - requested) / PSIZE > q_max_len || (NAZWA = NULL))
            break;
        
        current_time = clock();

        if(((double)(current_time - start)) / CLOCKS_PER_SEC >= RTIME) {
            start = current_time;
            sendto(rex_socket, rex_message, strlen(rex_message), 0, (const struct sockaddr*)&source_address, sizeof(source_address));
        }
    }

    free(rex_message);
    return NULL;
}