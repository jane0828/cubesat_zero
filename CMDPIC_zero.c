#include "common.h"
#include "functions.h"

// 수신된 CAN data 8 bytes를 해석하여 각 카메라 파라미터로 분리해 줌
void parse_camera_command(const uint8_t data[8], uint8_t *delay, uint32_t *shutter,
                          uint8_t *res, uint8_t *mode, int8_t *ev) {
    *delay = data[0];
    *shutter = (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4];
    *res = data[5];
    *mode = data[6];
    *ev = (int8_t)data[7];
}


int take_photo_and_send(int sock, uint8_t delay, uint32_t shutter, uint8_t res, uint8_t mode, int8_t ev) {
    sleep(delay);

    // 파일명 생성
    time_t now = time(NULL);           // 현재 시간 초 단위로 가져옴 (Unix timestamp, 1970. 1. 1. 이후 지난 초 수)
    struct tm *t = localtime(&now);    // 년, 월, 시로 변환
    char filename[256];
    snprintf(filename, sizeof(filename),
             "photo_%04d%02d%02d_%02d%02d%02d.jpg",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);

    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath),
             "/home/doteam-CAMERA-0/Desktop/Camera_team/pictures/%s", filename);

    int width = 1280, height = 800;
    if (res == 0x00) { width = 1920; height = 1080; }
    else if (res == 0x01) { width = 1024; height = 768; }
    else if (res == 0x03) { width = 1280; height = 720; }
    else if (res == 0x04) { width = 320; height = 240; }

    const char *mode_str = "normal";
    if (mode == 0x01) mode_str = "sport";
    else if (mode == 0x02) mode_str = "long";

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "rpicam-jpeg -o %s --shutter %u --quality 50 --width %d --height %d --exposure %s --ev %d -t 1000",
             fullpath, shutter, width, height, mode_str, ev);

    if (system(cmd) != 0) {
        fprintf(stderr, "사진 촬영 실패\n");
        return 3;
    }

    sleep(1);

    FILE *fp = fopen(fullpath, "rb");
    if (!fp) {
        perror("파일 열기 실패");
        return 4;
    }

    fseek(fp, 0, SEEK_SET);  // 파일 포인터 맨 앞으로 이동 (안전용)

    printf("사진 전송 시작: %s\n", fullpath);

    struct can_frame frame;
    int seq = 0;
    size_t len;

    while (1) {
        memset(&frame, 0, sizeof(frame));  // 프레임 전체 초기화 (쓰레기값 제거)

        len = fread(frame.data, 1, 8, fp);  // JPEG 데이터를 8바이트씩 읽기
        if (len <= 0) break;

        frame.can_id = TO_OBC_ID;
        frame.can_dlc = len;
        write(sock, &frame, sizeof(frame));
        usleep(1000);


if (seq == 0) {
    printf("첫 프레임 데이터: ");
    for (int i = 0; i < len; i++) {
        printf("%02X ", frame.data[i]);
    }
    printf("\n");
}
        seq++;
    }

    fclose(fp);

    // 전송 종료 프레임 전송
    frame.can_id = TO_OBC_ID;
    frame.can_dlc = 1;
    frame.data[0] = 0xFF;
    write(sock, &frame, sizeof(frame));

    printf("사진 전송 완료 (%d 프레임)\n", seq);
    // unlink(fullpath); // 필요시 주석 해제
    return 1;

}

void send_rsvcmdpic(int sock, const struct can_frame *frame) {

     // 파일명 생성
    time_t now = time(NULL);           // 현재 시간 초 단위로 가져옴 (Unix timestamp, 1970. 1. 1. 이후 지난 초 수)
    struct tm *t = localtime(&now);    // 년, 월, 시로 변환
    char filename[256];
    snprintf(filename, sizeof(filename),
             "photo_%04d%02d%02d_%02d%02d%02d.jpg",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);

    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath),
             "/home/doteam-CAMERA-0/Desktop/Camera_team/pictures/%s", filename);

    if (frame->can_id == 0x39 && frame->can_dlc >= 4) {
        // 빅엔디안 4바이트 데이터를 int형으로 변환
        int value = (frame->data[0] << 24) |
                    (frame->data[1] << 16) |
                    (frame->data[2] << 8)  |
                    (frame->data[3]);

        // write(sock, &ack, sizeof(ack));
        // 현재 시간 (Unix timestamp: 초 단위)
        time_t unix_time = time(NULL);
        if (unix_time == ((time_t)-1)) {
            perror("time error");
            return 1;
        }
        int new_value = value - unix_time; 

        char cmd[512];
        snprintf(cmd, sizeof(cmd),
             "rpicam-jpeg -o %s -t %d",fullpath, new_value);



    }
}
// int main() {
//     int s = setup_can_socket("can0");
//     struct can_frame frame;

//     while (1) {
//         if (read(s, &frame, sizeof(frame)) > 0 && frame.can_id == CAMERA_CMD_ID) {
//             uint8_t delay, res, mode;
//             uint32_t shutter;
//             int8_t ev;
//             parse_camera_command(frame.data, &delay, &shutter, &res, &mode, &ev);
//             run_camera_and_send(s, delay, shutter, res, mode, ev);
//         }
//     }

//     return 0;
// }
