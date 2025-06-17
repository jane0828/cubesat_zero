#include "common.h"
#include "functions.h"

int main() {
    int s = setup_can_socket("can0");
    struct can_frame frame;


    if (!wait_for_can_up("can0")) {
        fprintf(stderr, "can0 인터페이스가 준비되지 않았습니다. 종료합니다.\n");
        return 1;
    }

    printf("명령 대기 중...\n");




    while (1) {
        if (read(s, &frame, sizeof(frame)) > 0) {
            
            // 명령 종류별 실제 동작
            if (frame.can_id == CMDPIC_ID) {
                uint8_t delay, res, mode;
                uint32_t shutter;
                int8_t ev;
                parse_camera_command(frame.data, &delay, &shutter, &res, &mode, &ev);
                send_ack(s, &frame);
		int check = take_photo_and_send(s, delay, shutter, res, mode, ev);
		check_ack(s, &frame, check);
            }

            else if (frame.can_id == CMDVID_ID) {
                take_video_and_send(s, &frame);
            }

            else if (frame.can_id == CMDRESET_ID) {
                 reboot_zero(s, &frame);
             }

            else if (frame.can_id == CMDECHO_ID) {
                send_ack(s, &frame);
            }

            else if (frame.can_id == CMDTMSR_ID) {
		send_ack(s, &frame);
                int check = send_short_report(s);
		check_ack(s, &frame, check);
            }

            // else if (frame.can_id == TMLR_ID) {
                
            // }

            else if (frame.can_id == CMDRSVPIC_ID) {
                send_rsvcmdpic(s, &frame);
		send_ack(s, &frame);
            }

	    else {
		send_nack2(s, &frame);
		}
        }
    }

    close(s);
    return 0;
}
