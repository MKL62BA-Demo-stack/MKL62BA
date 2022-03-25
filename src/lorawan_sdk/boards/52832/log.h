#ifndef  __LOG__H
#define __LOG__H
#include "SEGGER_RTT.h"
#define BOARD_DEBUG(...) 	SEGGER_RTT_printf(0,__VA_ARGS__)
#endif
