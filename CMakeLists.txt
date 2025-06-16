cmake_minimum_required(VERSION 3.10)
project(zero_Project C)

# 소스 파일 리스트
set(SOURCES
    main.c
    CMDACK_zero.c
    CMDPIC_zero.c
    CMDRESET_zero.c
    CMDVID_zero.c
    functions.c
    ACKCHECK_zero.c
    TMSR.c
)

# 실행 파일 생성
add_executable(zero_app ${SOURCES})

# 필요시 라이브러리 링크 (예: socket, pthread)
target_link_libraries(zero_app pthread)
