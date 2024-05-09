#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#define MAX_LINES 1000
#define MAX_LINE_LENGTH 100

void optimize_code(char *input[], int line_count);
void readfile(char *filename, char *out);