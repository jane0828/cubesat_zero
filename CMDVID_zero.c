#include "common.h"
#include "functions.h"

void take_video_and_send(int sock, struct can_frame *frame) {
    if (frame->can_dlc != 8) {
        printf("영상 명령 길이 오류\n");
        return;
    }

    uint32_t delay_ms = (frame->data[0] << 24) | (frame->data[1] << 16) |
                        (frame->data[2] << 8)  | frame->data[3];
    uint8_t fps = frame->data[4];

    printf("영상 촬영 명령 수신: 지연 %u ms, FPS %u\n", delay_ms, fps);

    // 현재 시간으로 파일명 생성
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char h264_path[256], mp4_path[256];

    snprintf(h264_path, sizeof(h264_path),
            "%s/video_%04d%02d%02d_%02d%02d%02d.h264",
            VID_DIR,
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);



    // 촬영 명령
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "rpicam-vid -t %u -o %s --framerate %u",
             delay_ms, h264_path, fps);

    printf("촬영 명령 실행 중: %s\n", cmd);
    system(cmd);

    // h264 파일 확인
    if (access(h264_path, F_OK) != 0) {
        printf("촬영된 .h264 파일이 존재하지 않습니다: %s\n", h264_path);
        return;
    }

    printf("영상 촬영 완료: %s\n", h264_path);


    FILE *fp = fopen(h264_path, "rb");
    if (!fp) {
        perror("파일 열기 실패");
        return;
    }

    printf("영상 전송 시작: %s\n", h264_path);

    struct can_frame video;
    size_t len;
    while ((len = fread(video.data, 1, 8, fp)) > 0) {
        video.can_id = TO_OBC_ID;
        video.can_dlc = len;
        write(sock, &video, sizeof(video));
        usleep(1000); 
    }

    fclose(fp);

    // 전송 종료 알림 (0xFF 패킷)
    video.can_id = TO_OBC_ID;
    video.can_dlc = 1;
    video.data[0] = 0xFF;
    write(sock, &video, sizeof(video));

    printf("영상 전송 완료!\n");
}

//    printf("영상 전송 완료\n");











// int main() {
//     int sock = setup_can_socket("can0");
//     struct can_frame frame;

//     printf("영상 명령 수신 대기 중...\n");

//     while (1) {
//         if (read(sock, &frame, sizeof(frame)) > 0) {
//             if (frame.can_id == VIDEO_CMD_ID) {
//                 take_video_and_send(&frame);
//             }
//         }
//     }

//     close(sock);
//     return 0;
// }