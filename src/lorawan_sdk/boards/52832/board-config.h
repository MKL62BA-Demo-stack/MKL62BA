/*
	20200810 by ysh
	用于选择lora驱动所需要的硬件IO口
*/
#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include "nrf_gpio.h"
#include "system_port.h"

#define BOARD_DEBUG(...) 						SYS_LOG_INFO(__VA_ARGS__)
#define BOARD_DEBUG_DUMP_BUF(str,buf,size)		SYS_LOG_INFO_BUF(str,buf,size)
#define BOARD_DEBUG_ERROR(...)					SYS_LOG_ERROR(__VA_ARGS__)

#define SET_GPIO(A)   		nrf_gpio_pin_set(A) 
#define CLEAR_GPIO(A) 		nrf_gpio_pin_clear(A)
#define READ_GPIO(A)   		nrf_gpio_pin_read(A)
#define OUTPUT_GPIO(A) 		nrf_gpio_cfg_output(A)
#define INPUT_GPIO(A) 		nrf_gpio_cfg_input(A,NRF_GPIO_PIN_NOPULL)

#define LORA_NSS_PIN	17
#define LORA_SCK_PIN	16
#define LORA_MOSI_PIN	14
#define LORA_MISO_PIN	13
#define LORA_NRST_PIN	11
#define LORA_BUSY_PIN	19
#define LORA_DIO1_PIN	12

#define LORA_PA_PWR_PIN	23

#endif // __BOARD_CONFIG_H__
