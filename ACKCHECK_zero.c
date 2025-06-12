#include functions.h
#include common.h

void check_ack(int sock, const struct can_frame *frame) {
    uint8_t delay, res, mode;
    uint32_t shutter;
    int8_t ev;


    int check = take_photo_and_send(sock, &frame, delay, shutter, res, mode, ev);

    switch (check) {
        case 1:
            send_ack(sock, &frame);
            break;

        case 2:
            send_nack2(sock, &frame);
            break;

        case 3:
            send_nack3(sock, &frame);
            break;

        case 4:
            send_nack4(sock, &frame);
            break;

        default:
            printf("No way!!! return = %d\n", check);
            break;
    }
}
