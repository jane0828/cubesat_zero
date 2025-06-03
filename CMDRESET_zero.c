#include "common.h"
#include "functions.h"

// void send_boot_echo(int sock) {
//     struct can_frame echo = {
//         .can_id = CMDECHO_ID,
//         .can_dlc = 1,
//         .data = { 0x07 }
//     };

//     if (write(sock, &echo, sizeof(echo)) != sizeof(echo)) {
//         perror("에코 응답 전송 실패");
//     } else {
//         printf("부팅 후 에코 응답 전송 완료 (0x135)\n");
//     }
// }

void reboot_zero(int sock) {
    struct can_frame reboot;
    while (1) {
        if (read(sock, &reboot, sizeof(reboot)) <= 0) continue;

        if (reboot.can_id != CMDRESET_ID) continue;

        if (reboot.can_dlc == 1 && reboot.data[0] == 0x001) {
            sleep(2);            // 실수 방지 2초 대기!!
            system("sudo /sbin/reboot");
        }
    }
}
