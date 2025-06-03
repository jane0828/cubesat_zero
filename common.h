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


#define CMD_ID     0x200
#define DATA_ID    0x300
#define END_ID     0x3FF
#define NAME_ID    0x3FE
#define ECHO_ID    0x36
#define STATUS_ID  0x2FF
#define CAMERA_CMD_ID 0x106
#define VIDEO_CMD_ID 0x120



#define TO_OBC_ID   0x100

#define CMDHEL_ID       0x030
#define CMDPIC_ID       0x031
#define CMDVID_ID       0x033
#define CMDRESET_ID     0x034
#define CMDECHO_ID      0x036
#define CMDRSV_ID       0x039


#define FLAG_ACK        0x004


#define END_FRAME_VAL  0xFF
#define MAX_CAN_DLC    8

// 경로 설정
#define PIC_DIR    "/home/doteam/Desktop/Camera_team/pictures"
#define VID_DIR    "/home/doteam/Desktop/Camera_team/videos"

#endif
