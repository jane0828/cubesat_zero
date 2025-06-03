#include "common.h"
#include "functions.h"

int main() {
    int s = setup_can_socket("can0");
    struct can_frame frame;

    while (1) {
        if (read(s, &frame, sizeof(frame)) > 0) {
            // ACK 전송부터 시작 (명령 잘 받았어!)
            send_ack(s, &frame);

            // 명령 종류별 실제 동작
            if (frame.can_id == CMDHEL_ID) {
                uint8_t delay, res, mode;
                uint32_t shutter;
                int8_t ev;
                parse_camera_command(frame.data, &lens_pos, &denoise);
                take_photo_and_send(s, delay, shutter, res, mode, ev);
            }

            else if (frame.can_id == CMDPIC_ID) {
                uint8_t delay, res, mode;
                uint32_t shutter;
                int8_t ev;
                parse_camera_command(frame.data, &delay, &shutter, &res, &mode, &ev);
                take_photo_and_send(s, delay, shutter, res, mode, ev);
            }

            else if (frame.can_id == CMDVID_ID) {
                take_video_and_send(s, &frame);
            }

            // else if (frame.can_id == CMDRESET_ID) {
            //     reboot_zero(s);
            // }

            else if (frame.can_id == CMDECHO_ID) {
                send_ack(s, &frame);
            }

            // else if (frame.can_id == TMSR_ID) {
                
            // }

            // else if (frame.can_id == TMLR_ID) {
                
            // }

            // else if (frame.can_id == CMDRSV_ID) {
                
            // }

        }
    }

    close(s);
    return 0;
}
