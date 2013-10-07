/*
 * remote-shell.h
 *
 *  Created on: Sep 29, 2013
 *      Author: yzliao
 */

/**
 * \file
 *         A brief description of what this file is.
 * \author
 *         Yizheng Liao <yzliao@stanford.edu>
 */
#ifndef REMOTE_SHELL_H_
#define REMOTE_SHELL_H_

#include "contiki.h"

void remote_shell_init(void);
void remote_shell_send(const char* cmd, const uint16_t len);
void remote_shell_input(void);

PROCESS_NAME(remote_shell_process);
extern process_event_t remote_command_event_message;

#endif /* REMOTE_SHELL_H_ */
