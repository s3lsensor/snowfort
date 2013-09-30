/*
 * remote-shell.c
 *
 *  Created on: Sep 29, 2013
 *      Author: yzliao
 */


/**
 * \file
 *         A shell back-end for the remote receiving command
 * \author
 *         Yizheng Liao <yzliao@stanford.edu>
 */

#include "contiki.h"
#include "shell.h"
#include "remote-shell.h"

#include "net/mac/tdmardc.h"

#include <stdio.h>
#include <string.h>

process_event_t remote_command_event_message;

/*---------------------------------------------------------------------------*/
PROCESS(remote_shell_process, "Snowflake remote shell");
/*---------------------------------------------------------------------------*/
#if 0
void
shell_default_output(const char *text1, int len1, const char *text2, int len2)
{
  int i;
  if(text1 == NULL) {
    text1 = "";
    len1 = 0;
  }
  if(text2 == NULL) {
    text2 = "";
    len2 = 0;
  }

  /* Precision (printf("%.Ns", text1)) not supported on all platforms.
     putchar(c) not be supported on all platforms. */
  for(i = 0; i < len1; i++) {
    printf("%c", text1[i]);
  }
  for(i = 0; i < len2; i++) {
    printf("%c", text2[i]);
  }
  printf("\r\n");
}

/*---------------------------------------------------------------------------*/
void
shell_prompt(char *str)
{
}
/*---------------------------------------------------------------------------*/
void
shell_exit(void)
{
}
#endif
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(remote_shell_process,ev,data)
{
  PROCESS_BEGIN();

  shell_init();

  while(1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == remote_command_event_message && data != NULL);
    printf("Rx message %s\n",data);
    shell_input(data,strlen(data));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void remote_shell_init(void)
{
  process_start(&remote_shell_process,NULL);
}
/*---------------------------------------------------------------------------*/
