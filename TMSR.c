#include "common.h"
#include "functions.h"

int send_short_report(int sock) {
    FILE *fp = popen("rpicam-hello -v 2>&1 | grep 'Registered camera'", "r");
    if (!fp) {
        perror("popen 실패");
        return 3;
    }

    char buffer[256];
    if (fgets(buffer, sizeof(buffer), fp)) {
        size_t len = strlen(buffer);

        for (int i = 0; i < len; i += 8) {
            struct can_frame frame;
            frame.can_id = TO_OBC_ID;
            frame.can_dlc = (len - i >= 8) ? 8 : (len - i);

            for (int j = 0; j < frame.can_dlc; ++j) {
                frame.data[j] = buffer[i + j];
            }

            if (write(sock, &frame, sizeof(frame)) != sizeof(frame)) {
                perror("카메라 정보 전송 실패");
                break;
            } else {
                printf("카메라 정보 전송: %.8s\n", frame.data);
            }

            usleep(1000); // 최소한의 간격 주기 (1ms)
        }

        // 종료 프레임 전송 (null 문자 전송)
        struct can_frame end_frame = {
            .can_id = TO_OBC_ID,
            .can_dlc = 1,
            .data = {0x00}
        };
        write(sock, &end_frame, sizeof(end_frame));




    } else {
        printf("카메라 정보 추출 실패 또는 해당 문자열 없음\n");
    }

    pclose(fp);
    return 1;
}
