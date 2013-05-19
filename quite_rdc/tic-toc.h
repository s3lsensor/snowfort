#include <stdio.h>

/* Adds a tic = time */
void tic(uint16_t time, const char* tag);
/* Adds the last tic and displays the results */
void toc();
/* Prints tic-toc : to be called by etimer process */
void print_tics();
