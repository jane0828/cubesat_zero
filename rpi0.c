#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <time.h>

#define CMD_ID     0x100
#define DATA_ID    0x200
#define END_ID     0x2FF
#define ECHO_ID    0x007
#define STATUS_ID  0x300
#define IMAGE_PATH "/tmp/capture.jpg"

void setup_can(int *s, struct sockaddr_can *addr, struct ifreq *ifr) {
    *s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (*s < 0) { perror("socket"); exit(1); }

    strcpy(ifr->ifr_name, "can0");
    ioctl(*s, SIOCGIFINDEX, ifr);
    addr->can_family = AF_CAN;
    addr->can_ifindex = ifr->ifr_ifindex;

    if (bind(*s, (struct sockaddr *)addr, sizeof(*addr)) < 0) {
        perror("bind"); exit(1);
    }
}

void send_status(int sock, unsigned char code) {
    struct can_frame frame;
    frame.can_id = STATUS_ID;
    frame.can_dlc = 5;

    time_t now = time(NULL);
    uint32_t ts = (uint32_t)now;

    frame.data[0] = code;
    frame.data[1] = (ts >> 24) & 0xFF;
    frame.data[2] = (ts >> 16) & 0xFF;
    frame.data[3] = (ts >> 8) & 0xFF;
    frame.data[4] = ts & 0xFF;

    if (write(sock, &frame, sizeof(frame)) != sizeof(frame)) {
        perror("상태 메시지 전송 실패");
    } else {
        printf("상태 메시지 전송 완료: 0x%02X (timestamp: %u)\n", code, ts);
    }
}

void send_file_over_can(int sock, const char *filepath) {
    FILE *fp = fopen(filepath, "rb");
    if (!fp) { perror("fopen"); return; }

    struct can_frame frame;
    int seq = 0;
    size_t len;
    size_t total_sent = 0;

    send_status(sock, 0x11); // 전송 시작 알림
    printf("전송 시작...\n");

    while ((len = fread(frame.data, 1, 8, fp)) > 0) {
        frame.can_id = DATA_ID + (seq & 0xFF);
        frame.can_dlc = len;

        if (write(sock, &frame, sizeof(frame)) != sizeof(frame)) {
            perror("CAN 전송 실패");
        } else {
            total_sent += len;
            printf("\u2192 %3zu bytes 전송됨 (누적: %zu bytes)\n", len, total_sent);
        }

        seq++;
        usleep(10000);
    }

    fclose(fp);
    printf("전송 완료 (%d 프레임, 총 %zu bytes)\n", seq, total_sent);

    struct can_frame end;
    end.can_id = END_ID;
    end.can_dlc = 1;
    end.data[0] = 0xFF;
    if (write(sock, &end, sizeof(end)) != sizeof(end)) {
        perror("전송 종료 프레임 전송 실패");
    } else {
        printf("전송 종료 프레임 전송 완료 (0x2FF, 0xFF)\n");
    }
}

void send_echo_reply(int sock) {
    struct can_frame reply;
    reply.can_id = ECHO_ID;
    reply.can_dlc = 1;
    reply.data[0] = 0x07;
    if (write(sock, &reply, sizeof(reply)) != sizeof(reply)) {
        perror("에코 응답 전송 실패");
    } else {
        printf("에코 응답 전송 완료\n");
    }
}

void handle_command(unsigned char code, int sock) {
    char cmd[256];

    switch (code) {
        case 0x01:
        case 0x06:
            printf("사진 촬영 명령 수신\n");
            send_status(sock, 0x09); // 촬영 시작
            snprintf(cmd, sizeof(cmd), "rpicam-jpeg -o %s --width 320 --height 240 --quality 50 -t 1000", IMAGE_PATH);
            if (system(cmd) != 0) {
                fprintf(stderr, "촬영 명령 실패\n");
                return;
            }
            send_status(sock, 0x10); // 촬영 완료
            send_file_over_can(sock, IMAGE_PATH);
            break;

        case 0x05:
            printf("재부팅 명령 수신. 시스템 재시작합니다...\n");
            sync(); sleep(1);
            system("sudo /usr/sbin/reboot");
            break;

        case 0x07:
            printf("에코 테스트 명령 수신. 응답 전송 중...\n");
            send_echo_reply(sock);
            break;

        default:
            printf("알 수 없는 명령: 0x%02X\n", code);
            break;
    }
}

int wait_for_can_up(const char *ifname) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "ip link show %s | grep -q 'state UP'", ifname);

    for (int i = 0; i < 20; ++i) {  // 최대 20초까지 대기
        if (system(cmd) == 0) {
            return 1;  // 준비됨
        }
        sleep(1);
    }
    return 0;  // 실패
}


int main() {
    int sock;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    if (!wait_for_can_up("can0")) {
        fprintf(stderr, "can0 인터페이스가 준비되지 않았습니다. 종료합니다.\n");
        return 1;
    }


    setup_can(&sock, &addr, &ifr);
    sleep(2); // 인터페이스 안정화

    struct can_frame echo;
    echo.can_id = 0x07;
    echo.can_dlc = 1;
    echo.data[0] = 0x07;
    write(sock, &echo, sizeof(echo));
    printf("부팅 후 자동 에코 응답 전송 완료 (0x07)\n");
    printf("명령 대기 중...\n");

    while (1) {
        if (read(sock, &frame, sizeof(frame)) > 0) {
            if (frame.can_id == CMD_ID && frame.can_dlc > 0) {
                handle_command(frame.data[0], sock);
            }
        }
    }
    return 0;
}
