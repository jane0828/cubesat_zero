#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdint.h>

#define CMD_ID   0x200
#define DATA_ID  0x300
#define END_ID   0x3FF
#define NAME_ID  0x3FE

char* get_photo_path(int year, int month, int day, int hour, int seq, char* out) {
    DIR *d;
    struct dirent *dir;
    int count = 0;
    char prefix[64];
    snprintf(prefix, sizeof(prefix),
             hour == 24 ?
             "photo_%04d%02d%02d_" :
             "photo_%04d%02d%02d_%02d",
             year, month, day, hour);

    d = opendir("/home/doteam/Desktop/Camera_team/pictures");
    if (!d) return NULL;

    while ((dir = readdir(d)) != NULL) {
        if (strstr(dir->d_name, prefix) == dir->d_name) {
            if (count == seq) {
                snprintf(out, 512, "/home/doteam/Desktop/Camera_team/pictures/%s", dir->d_name);
                closedir(d);
                return out;
            }
            count++;
        }
    }

    closedir(d);
    return NULL;
}

void send_filename(int sock, const char *filename) {
    struct can_frame frame;
    size_t len = strlen(filename);
    size_t sent = 0;

    while (sent < len) {
        frame.can_id = NAME_ID;
        frame.can_dlc = (len - sent > 8) ? 8 : (len - sent);
        memcpy(frame.data, filename + sent, frame.can_dlc);
        write(sock, &frame, sizeof(frame));
        sent += frame.can_dlc;
        usleep(5000);
    }

    frame.can_id = NAME_ID;
    frame.can_dlc = 1;
    frame.data[0] = 0x00;
    write(sock, &frame, sizeof(frame));
    usleep(5000);
}

void send_file_over_can(int sock, const char* filepath) {
    FILE *fp = fopen(filepath, "rb");
    if (!fp) { perror("파일 열기 실패"); return; }

    struct can_frame frame;
    int seq = 0;
    size_t len;

    while ((len = fread(frame.data, 1, 8, fp)) > 0) {
        frame.can_id = DATA_ID + (seq & 0xFF);
        frame.can_dlc = len;
        write(sock, &frame, sizeof(frame));
        usleep(10000);
        seq++;
    }

    fclose(fp);
}

int setup_can_socket(const char *ifname) {
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    strcpy(ifr.ifr_name, ifname);
    ioctl(s, SIOCGIFINDEX, &ifr);
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    return s;
}

int main() {
    int sock = setup_can_socket("can0");
    struct can_frame frame;
    uint8_t buffer[5] = {0};
    int received = 0;

    while (1) {
        if (read(sock, &frame, sizeof(frame)) > 0) {
            if (frame.can_id >= CMD_ID && frame.can_id < CMD_ID + 5) {
                buffer[frame.can_id - CMD_ID] = frame.data[0];
                received++;
            }

            if (received == 5) {
                received = 0;

                int year  = 2000 + buffer[1];
                int month = buffer[2];
                int day   = buffer[3];
                int hour  = buffer[4];

                char path[512];
                int seq = 0;

                while (get_photo_path(year, month, day, hour, seq, path)) {
                    const char *filename = strrchr(path, '/');
                    if (filename) filename++; else filename = path;

                    send_filename(sock, filename);
                    send_file_over_can(sock, path);
                    seq++;
                }

                struct can_frame end = { .can_id = END_ID, .can_dlc = 1, .data = { 0xFF } };
                write(sock, &end, sizeof(end));
            }
        }
    }

    close(sock);
    return 0;
}
