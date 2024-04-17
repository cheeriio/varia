#ifndef MIMUW_SIK_RADIO_H
#define MIMUW_SIK_RADIO_H
#include <stdio.h>
#include "common.h"

void print_sender_usage_info(char* pname){
    fprintf(stderr, "Usage: %s [-a <address>] [-p <package size>]"
            " [-P <port>] [-n <name>] [-R <rtime>] [-f <fsize>]\n", pname);
    return;
}

void print_receiver_usage_info(char* pname){
    fprintf(stderr, "Usage: %s [-a <address>] "
            " [-P <port>] [-b <buffer size>]\n", pname);
    return;
}

#endif //MIMUW_SIK_RADIO_H