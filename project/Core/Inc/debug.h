/*
 * debug.h
 *
 *  Created on: Oct 7, 2022
 *      Author: sanderspeetjens
 */

#ifndef INC_DEBUG_H_
#define INC_DEBUG_H_

#include "stm32f7xx_hal.h"

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>

#define enableDebug 1
#if enableDebug == 1
#define debug(fmt, ...) printf("\033[32m"fmt"\033[0m", ##__VA_ARGS__)
#define debugln(fmt, ...) printf("\033[32m"fmt"\033[0m\n", ##__VA_ARGS__)
#define debugWarn(fmt, ...) printf("\033[33m"fmt"\033[0m", ##__VA_ARGS__)
#define debugWarnln(fmt, ...) printf("\033[33m"fmt"\033[0m\n", ##__VA_ARGS__)
#define debugErr(fmt, ...) printf("\033[31m"fmt"\033[0m", ##__VA_ARGS__)
#define debugErrln(fmt, ...) printf("\033[31m"fmt"\033[0m\n", ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#define debugln(fmt, ...)
#define debugWarn(fmt, ...)
#define debugWarnln(fmt, ...)
#define debugErr(fmt, ...)
#define debugErrln(fmt, ...)
#endif

#define ANSI_ESC "\x1B"
#define ANSI_CSI "\x9B"
#define ANSI_DCS "\x90"
#define ANSI_OSC "\x9D"

#define CLEAR_SCREEN "\033c"

#define CURSOR_RESET ANSI_ESC "[H"
#define CURSOR_UP(n) ANSI_ESC "[" #n "A"
#define CURSOR_DOWN(n) ANSI_ESC "[" #n "B"
#define CURSOR_RIGHT(n) ANSI_ESC "[" #n "C"
#define CURSOR_LEFT(n) ANSI_ESC "[" #n "D"
#define CURSOR_NEXT_N_LINES(n) ANSI_ESC "[" #n "E"
#define CURSOR_PREV_N_LINES(n) ANSI_ESC "[" #n "F"
#define CURSOR_COL(n) ANSI_ESC "[" #n "G"
#define CURSOR_POS ANSI_ESC "[" #n ";" #n "H"
#define CURSOR_SAVE ANSI_ESC "7"
#define CURSOR_RESTORE ANSI_ESC "8"

#define ERASE_FROM_CURSOR_TO_END ANSI_ESC "[0J"
#define ERASE_FROM_CURSOR_TO_BEGINNING ANSI_ESC "[1J"
#define ERASE_ENTIRE_SCREEN ANSI_ESC "[2J"
#define ERASE_FROM_CURSOR_TO_END_LINE ANSI_ESC "[0K"
#define ERASE_FROM_CURSOR_TO_BEGINNING_LINE ANSI_ESC "[1K"
#define ERASE_ENTIRE_LINE ANSI_ESC "[2K"

int _write(int file, char *ptr, int len);
int _read(int fd, char* ptr, int len);
#endif /* INC_DEBUG_H_ */
