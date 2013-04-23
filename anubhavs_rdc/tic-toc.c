#include "tic-toc.h"
#include <stdio.h>
#include <string.h>
#include "rtimer.h"

#define MAX_TICS 20
#define RTIMER_MS RTIMER_SECOND/1000

uint16_t tics[MAX_TICS];
char *tags[MAX_TICS];
uint8_t num_tics = 0;

uint16_t tics_copy[MAX_TICS];
char *tags_copy[MAX_TICS];
uint8_t num_tics_copy = 0;




void tic(uint16_t time, const char* tag)
{
  tics[num_tics]=time;
  tags[num_tics]=tag;
  num_tics++;
}

void toc()
{
  num_tics_copy = num_tics;
  num_tics = 0;
  memcpy(tags_copy, tags, num_tics_copy*sizeof(char*));
  memcpy(tics_copy, tics, num_tics_copy*sizeof(uint16_t));
}

void print_tics()
{
  if (num_tics_copy==0) return;
  uint16_t ref = tics_copy[0];
  uint16_t delta = 0;
  float delta_ms = 0;
  float rt_ms = RTIMER_SECOND/1000.0;
  uint16_t ms = 0;
  uint16_t us = 0;
  int i=0;
  for (i=0; i<num_tics_copy; i++) {
    delta = tics_copy[i]-ref;
    delta_ms = delta/rt_ms;
    ms = delta_ms; //cast to uint16_t
    us = (delta_ms-ms)*1000;
    printf("[%s: %u.%03u]  ", tags_copy[i], ms, us);
  }
  printf("\n");
}
