/*
	20200810 by ysh
	目前先采用IO模拟的方式来使用SPI
*/

#include "spi-board.h"
#include "board-config.h"
#include "utilities.h"

void spi_io_init(void)
{
	OUTPUT_GPIO(LORA_SCK_PIN);
	OUTPUT_GPIO(LORA_MOSI_PIN);  
	INPUT_GPIO(LORA_MISO_PIN);
	CLEAR_GPIO(LORA_SCK_PIN);
	nrf_gpio_input_disconnect(LORA_MISO_PIN);  //关闭52832的MISO io避免灌电流引起的功耗问题
}
int spi_transdata(uint8_t *tx_data,uint16_t tx_len,uint8_t *rxbuf)
{

	uint16_t i,wr_data,j;

	INPUT_GPIO(LORA_MISO_PIN);
	
	CLEAR_GPIO(LORA_NSS_PIN);

	for(j=0; j<tx_len; j++)
	{
		wr_data=tx_data[j];
		rxbuf[j]=0;
		for(i=0; i<8; i++) 
		{
			if(wr_data&0x80)
			{
			SET_GPIO(LORA_MOSI_PIN) ;
			}
			else
			{
			CLEAR_GPIO(LORA_MOSI_PIN);
			}
			SET_GPIO(LORA_SCK_PIN);

			wr_data<<=1;
			rxbuf[j]<<=1;
			if(READ_GPIO(LORA_MISO_PIN))
			rxbuf[j]|=0x01;
			CLEAR_GPIO(LORA_SCK_PIN);
		}
	}
	SET_GPIO(LORA_NSS_PIN);

	nrf_gpio_input_disconnect(LORA_MISO_PIN);
	return 0;
}



