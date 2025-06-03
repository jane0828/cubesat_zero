code#include "common.h"
#include "functions.h"

/*
  @brief HEL 명령 데이터 파싱 함수
  
  @param data       8바이트 CAN 데이터
  @param lens_pos   렌즈 위치 값 (0~32)
  @param denoise    노이즈 필터 설정값
 */
void parse_hel_command(const uint8_t data[8], uint8_t *lens_pos, uint8_t *denoise) {
    *lens_pos = data[0];
    *denoise = data[1];
}

/*
  @brief HEL 명령 수행 함수
  
  @param lens_pos 렌즈 위치
  @param denoise  노이즈 필터 모드
*/ 
void run_hel_command(uint8_t lens_pos, uint8_t denoise) {
    // denoise 모드 문자열 변환
    const char* denoise_str = "auto";
    if (denoise == 0x01) denoise_str = "cdn_off";
    else if (denoise == 0x02) denoise_str = "cdn_fast";
    else if (denoise == 0x03) denoise_str = "cdn_hq";

    // 명령어 구성 (예시용, 실제 명령은 환경에 따라 수정)
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "rpicam-hello --lens-position %d --denoise %s",
             lens_pos, denoise_str);

    printf("HEL 명령 실행: %s\n", cmd);

    int ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "HEL 명령 실행 실패 (code %d)\n", ret);
    } else {
        printf("HEL 명령 실행 완료\n");
    }
}
