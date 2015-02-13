#ifndef __MAIN_H_
#define __MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <limits.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

int main_parse_options(int, char**);
int main_read_optarg(char);
void main_show_help();

#endif//__MAIN_H_
