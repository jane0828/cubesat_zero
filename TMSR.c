#include "common.h"
#include "functions.h"

int send_short_report(int sock) {
    FILE *fp;
    char buffer[512];

    // Step 1: 실패 메시지 체크 ("Failed to start streaming" or "no cameras available")
    fp = popen("rpicam-hello -v 2>&1", "r");
    if (!fp) {
        perror("popen fail-check");
        return 3;
    }

    int should_return_3 = 0;
    while (fgets(buffer, sizeof(buffer), fp)) {
        if (strstr(buffer, "Failed to start streaming") || 
            strstr(buffer, "ERROR: *** no cameras available ***")) {
            should_return_3 = 1;
            break;
        }
    }
    pclose(fp);

    if (should_return_3) {
        printf("[INFO] 카메라 연결 실패 감지됨\n");
        return 3;
    }

    // Step 2: 정상 로그 추출 후 전송
    fp = popen("rpicam-hello -v 2>&1 | grep 'Registered camera'", "r");
    if (!fp) {
        perror("popen success-check");
        return 3;
    }

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
                perror("CAN 전송 실패");
                break;
            } else {
                printf("[CAN 송신] %.8s\n", frame.data);
            }

            usleep(1000); // 1ms 지연
        }

        // 종료 프레임 전송
        struct can_frame end_frame = {
            .can_id = TO_OBC_ID,
            .can_dlc = 1,
            .data = {0x00}
        };
        write(sock, &end_frame, sizeof(end_frame));
    } else {
        printf("[INFO] 정상 로그 없음: 'Registered camera' 미출력\n");
    }

    pclose(fp);
    return 1;
}


