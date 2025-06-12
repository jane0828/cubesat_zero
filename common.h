#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>


#define TO_OBC_ID   0x100

#define CMDPIC_ID       0x031
#define CMDVID_ID       0x033
#define CMDRESET_ID     0x034
#define CMDECHO_ID      0x036
#define TMSR_ID         0x037
#define TMLR_ID         0x038
#define CMDRSV_ID       0x039


#define FLAG_ACK        0x004


#define END_FRAME_VAL  0xFF
#define MAX_CAN_DLC    8

// 경로 설정
#define PIC_DIR    "/home/doteam/Desktop/Camera_team/pictures"
#define VID_DIR    "/home/doteam/Desktop/Camera_team/videos"

#endif
