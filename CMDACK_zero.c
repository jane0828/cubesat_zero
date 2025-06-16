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
            case CMDRESET_ID:
            case CMDTMSR_ID:
            case CMDTMLR_ID:
            case CMDRSVPIC_ID:
                ack.data[0] = frame->can_id & 0xFF;
                ack.data[1] = 0x01;
                break;

            case CMDECHO_ID:
                ack.data[0] = CMDECHO_ID & 0xFF;
                ack.data[1] = 135;
                break;

            default:
		printf("누구세요...?\n");
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


// NACK2: 명령을 받긴 받았는데 이상한 명령을 받음
void send_nack2(int sock, const struct can_frame *frame) {
    if (wait_for_can_up("can0") != 1) {
        fprintf(stderr, "CAN 인터페이스가 비활성화됨\n");
        return;
    }

    struct can_frame nack;
    nack.can_id = TO_OBC_ID;
    nack.can_dlc = 2;
    nack.data[0] = frame->can_id & 0xFF;
    nack.data[1] = 0x02;

    if (write(sock, &nack, sizeof(nack)) != sizeof(nack)) {
        perror("NACK2 전송 실패");
    } else {
        printf("NACK2 전송 완료: to 0x%03X (data[0]=0x%02X, data[1]=0x%02X)\n",
               frame->can_id, nack.data[0], nack.data[1]);
    }
}



// 사진 촬영에 실패함함
void send_nack3(int sock, const struct can_frame *frame) {
    if (wait_for_can_up("can0") != 1) {
        fprintf(stderr, "CAN 인터페이스가 비활성화되어 NACK3 전송 중단\n");
        return;
    }

    struct can_frame nack;
    nack.can_id = TO_OBC_ID;
    nack.can_dlc = 2;
    nack.data[0] = frame->can_id & 0xFF;
    nack.data[1] = 0x03;

    if (write(sock, &nack, sizeof(nack)) != sizeof(nack)) {
        perror("NACK3 전송 실패");
    } else {
        printf("NACK3 전송 완료: to 0x%03X (data[0]=0x%02X, data[1]=0x%02X)\n",
               frame->can_id, nack.data[0], nack.data[1]);
    }
}




// 촬영은 했는데 파일 열기에 실패함함
void send_nack4(int sock, const struct can_frame *frame) {
    if (wait_for_can_up("can0") != 1) {
        fprintf(stderr, "CAN 인터페이스 비활성화됨. NACK4 전송 중단\n");
        return;
    }

    struct can_frame nack;
    nack.can_id = TO_OBC_ID;
    nack.can_dlc = 2;
    nack.data[0] = frame->can_id & 0xFF;
    nack.data[1] = 0x04;

    if (write(sock, &nack, sizeof(nack)) != sizeof(nack)) {
        perror("NACK4 전송 실패");
    } else {
        printf("NACK4 전송 완료: to 0x%03X (data[0]=0x%02X, data[1]=0x%02X)\n",
               frame->can_id, nack.data[0], nack.data[1]);
    }
}
