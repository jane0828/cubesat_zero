#include "common.h"
#include "functions.h"

int setup_can_socket(const char *ifname) {
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) { perror("socket"); exit(1); }

    strcpy(ifr.ifr_name, ifname);
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); exit(1);
    }

    return s;
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

//void send_echo_reply(int sock) {
//    struct can_frame reply;
//    reply.can_id = ECHO_ID;
//    reply.can_dlc = 1;
//    reply.data[0] = 0x07;
//    if (write(sock, &reply, sizeof(reply)) != sizeof(reply)) {
//        perror("에코 응답 전송 실패");
//    } else {
//        printf("에코 응답 전송 완료\n");
//    }
//}
