#include "functions.h"
#include "common.h"

void check_ack(int sock, const struct can_frame *frame, int check) {
    switch (check) {
        case 1:
            send_ack(sock, frame);
            printf("ACK 보냄\n");
            break;

        case 2:
            send_nack2(sock, frame);
            printf("ERROR CODE 2\n");
            break;

        case 3:
            send_nack3(sock, frame);
            printf("ERROR CODE 3\n");
            break;

        case 4:
            send_nack4(sock, frame);
            printf("ERROR CODE 4\n");
            break;

        default:
            printf("No way!!! return = %d\n", check);
            break;
    }
}
