#ifndef __LORAWAN__APP__H__
#define __LORAWAN__APP__H__

#include<stdio.h>
#include<stdint.h>

#include "board-config.h"
#include "LoRaMac.h"
#include "LoRaMacTest.h"
#include "rtc-board.h"
#include "board.h"
#include "sx126x-board.h"
#include "board-config.h"

#define LORA_DEBUG_EN		1
#if 	LORA_DEBUG_EN

#define   LORA_DEBUG(...)   						BOARD_DEBUG(__VA_ARGS__)
#define	LORA_DEBUG_DUMP_BUF(str,buf,size)		LORA_DEBUG("%s:",str);\
												for(uint16_t i=0 ; i<size ; i++)\
												{LORA_DEBUG("0x%02x ",buf[i]);}\
												LORA_DEBUG("\r\n")
#define 	LORA_DEBUG_LOCATION(...)					LORA_DEBUG("log info:file is %s,line is %d,func is %s,user log:",__FILE__,__LINE__,__func__);LORA_DEBUG(__VA_ARGS__)
#else
#define 	LORA_DEBUG(...)
#define 	LORA_DEBUG_DUMP_BUF(str,buf,size)
#define 	LORA_DEBUG_LOCATION(...)
#endif


void lorawan_init(LoRaMacRegion_t	region);
void lorawan_task(void);

void lorawan_test(void);

#endif
