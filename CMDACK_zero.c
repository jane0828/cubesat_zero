#include "common.h"
#include "functions.h"

void send_ack(int sock, const struct can_frame *frame) {

    if (wait_for_can_up("can0") == 1) {
        struct can_frame ack;
        ack.can_id = TO_OBC_ID;
        ack.can_dlc = 2;

        switch (frame->can_id) {
            case CMDHEL_ID:
            case CMDPIC_ID:
            case CMDVID_ID:
            // case CMDRESET_ID:
            // case TMSR_ID:
            // case TMLR_ID:
            // case CMDRSV_ID:
                ack.data[0] = frame->can_id & 0xFF;
                ack.data[1] = 0x01;
                break;

            case CMDECHO_ID:
                ack.data[0] = CMDECHO_ID & 0xFF;
                ack.data[1] = 135;
                break;

            default:
                return;
        }

        // write(sock, &ack, sizeof(ack));

        if (write(sock, &ack, sizeof(ack)) != sizeof(ack)) {
            perror("ACK 전송 실패");
        } else {
            printf("ACK 전송 완료: to 0x%03X (data[0]=0x%02X, data[1]=0x%02X)\n",
                frame->can_id, ack.data[0], ack.data[1]);
        }
    }
}
