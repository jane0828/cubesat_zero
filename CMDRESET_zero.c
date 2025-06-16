#include "common.h"
#include "functions.h"

void send_boot_echo(int sock) {
    struct can_frame echo = {
        .can_id = TO_OBC_ID,
        .can_dlc = 2,
        .data = { 0x36, 0x87 }
    };

    if (write(sock, &echo, sizeof(echo)) != sizeof(echo)) {
        perror("에코 응답 전송 실패");
    } else {
        printf("부팅 후 에코 응답 전송 완료 (0x36, 0x87)\n");
    }
}


void reboot_zero(int sock, const struct can_frame *frame) {
    if (frame->can_dlc == 1 && frame->data[0] == 0x01) {
        send_ack(sock, frame);
        sleep(2);  // 실수 방지 대기
        system("sudo /sbin/reboot");
    }
}

