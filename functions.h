#ifndef FUNCTIONS_H
#define FUNCTIONS_H

int setup_can_socket(const char *ifname);
int wait_for_can_up(const char *ifname);
void send_echo_reply(int sock);

#endif
