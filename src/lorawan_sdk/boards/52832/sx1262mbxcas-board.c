/*!
	20200810 by ysh
	
 */
#include <stdlib.h>
#include <string.h>
#include "utilities.h"
#include "board-config.h"
#include "delay.h"
#include "radio.h"
#include "sx126x-board.h"
#include "spi-board.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"


/*
 无线芯片PA控制， drive radio RX/TX pins 
 */
void sx126x_pa_control (int8_t state)
{
	//发送接收都置高，由DIO2控制发送还是接收
	if(state>=0)
	{
	    	SET_GPIO(LORA_PA_PWR_PIN);
	}
	else
	{
	    	CLEAR_GPIO(LORA_PA_PWR_PIN);
	}
}
static  uint8_t  inbuf[256+20];
static  uint8_t outbuf[256+20];
//芯片IO初始化
void SX126xIoInit( void )
{
	INPUT_GPIO(LORA_NRST_PIN);
	INPUT_GPIO(LORA_BUSY_PIN);
	INPUT_GPIO(LORA_DIO1_PIN);
	OUTPUT_GPIO(LORA_NSS_PIN);
	OUTPUT_GPIO(LORA_PA_PWR_PIN);
	SET_GPIO(LORA_PA_PWR_PIN);

	spi_io_init();
}

DioIrqHandler *loradioIrq;
static void pin_irq(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    loradioIrq(0);
}
//芯片IO中断初始化
void SX126xIoIrqInit( DioIrqHandler dioIrq )
{
	loradioIrq=dioIrq;
	nrf_drv_gpiote_init();
	nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);	
	nrf_drv_gpiote_in_init(LORA_DIO1_PIN,&in_config,pin_irq);
	nrf_drv_gpiote_in_event_enable(LORA_DIO1_PIN,1);
}

void SX126xIoDeInit( void )
{

}

void SX126xIoDbgInit( void )
{

}

void SX126xIoTcxoInit( void )
{
	SX126xSetDio3AsTcxoCtrl( TCXO_CTRL_1_8V, 255 );

}


uint32_t SX126xGetBoardTcxoWakeupTime( void )
{
	#define BOARD_TCXO_WAKEUP_TIME 5
	return BOARD_TCXO_WAKEUP_TIME;
}

void SX126xIoRfSwitchInit( void )
{
    SX126xSetDio2AsRfSwitchCtrl( true );
}

static RadioOperatingModes_t OperatingMode;
RadioOperatingModes_t SX126xGetOperatingMode( void )
{
    return OperatingMode;
}

void SX126xSetOperatingMode( RadioOperatingModes_t mode )
{
    OperatingMode = mode;
}

//控制复位管脚，(0=low, 1=high, 2=floating)
void SX126X_PIN_RST_CONTROL(uint8_t state)
{
	if(state == 0 || state == 1)   // drive pin
	{
		OUTPUT_GPIO(LORA_NRST_PIN);
		if(state)
		{
			SET_GPIO(LORA_NRST_PIN);
		}
		else
		{
			CLEAR_GPIO(LORA_NRST_PIN);
		}
	}
	else     // keep pin floating
	{
		INPUT_GPIO(LORA_NRST_PIN);
	}
}
void SX126xReset( void )
{
	do
	{
		DelayMs( 10 );
		SX126X_PIN_RST_CONTROL(0);
		DelayMs( 20 );
		SX126X_PIN_RST_CONTROL(2); // internal pull-up
		DelayMs( 10 );
	}
	while( SX126xReadRegister(0x0741) != 0x24 );
}

void SX126xWaitOnBusy( void )
{
	while (READ_GPIO(LORA_BUSY_PIN) != 0);
}

void SX126xWakeup( void )
{
	uint16_t len=0;
	inbuf[len++]=RADIO_GET_STATUS;
	inbuf[len++]=0x00;
	spi_transdata(inbuf,len,outbuf);

	// Wait for chip to be ready.
	SX126xWaitOnBusy( );
}

void SX126xWriteCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
	uint16_t i,len=0;
	SX126xCheckDeviceReady( );
	inbuf[len++]=command;
	for(  i = 0; i < size; i++ )
	{
	    	inbuf[len++]= buffer[i];
	}
	spi_transdata(inbuf,len,outbuf);
	if( command != RADIO_SET_SLEEP )
	{
	    	SX126xWaitOnBusy( );
	}
}

uint8_t SX126xReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
	uint16_t i,len=0;

	SX126xCheckDeviceReady( );


	inbuf[len++]=command;
	for(  i = 0; i < size+1; i++ )
	{
	    	inbuf[len++]= 0;
	}
	spi_transdata(inbuf,len,outbuf);


	memcpy1(buffer,outbuf+2,size);

	SX126xWaitOnBusy( );

	return outbuf[1];
}

void SX126xWriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
	uint16_t i,len=0;

	SX126xCheckDeviceReady( );

	inbuf[len++]=RADIO_WRITE_REGISTER;
	inbuf[len++]=( address & 0xFF00 ) >> 8 ;
	inbuf[len++]=address & 0x00FF;
	for(  i = 0; i < size; i++ )
	{
	    	inbuf[len++]= buffer[i];
	}
	spi_transdata(inbuf,len,outbuf);

	SX126xWaitOnBusy( );
}

void SX126xWriteRegister( uint16_t address, uint8_t value )
{
	SX126xWriteRegisters( address, &value, 1 );	
}

void SX126xReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
	uint16_t i,len=0;
	SX126xCheckDeviceReady( );

	inbuf[len++]=RADIO_READ_REGISTER;
	inbuf[len++]=( address & 0xFF00 ) >> 8 ;
	inbuf[len++]=address & 0x00FF;
	inbuf[len++]=0;
	for(  i = 0; i < size; i++ )
	{
	   	inbuf[len++]= 0;
	}
	spi_transdata(inbuf,len,outbuf);
	memcpy1(buffer,outbuf+4,size);
	SX126xWaitOnBusy( );
}

uint8_t SX126xReadRegister( uint16_t address )
{
	uint8_t data;
    	SX126xReadRegisters( address, &data, 1 );
    	return data;
}

void SX126xWriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
	uint16_t i,len=0;
	SX126xCheckDeviceReady( );

	inbuf[len++]=RADIO_WRITE_BUFFER;
	inbuf[len++]=offset ;
	for(  i = 0; i < size; i++ )
	{
	    	inbuf[len++]= buffer[i];
	}
	spi_transdata(inbuf,len,outbuf);
	memcpy(buffer,outbuf+2,size);
	SX126xWaitOnBusy( );
}

void SX126xReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
	uint16_t i,len=0;

	SX126xCheckDeviceReady( );

	inbuf[len++]=RADIO_READ_BUFFER;
	inbuf[len++]=offset ;
	inbuf[len++]=0;
	for(  i = 0; i < size; i++ )
	{
	    	inbuf[len++]= 0;
	}
	spi_transdata(inbuf,len,outbuf);
	memcpy1(buffer,outbuf+3,size);
	SX126xWaitOnBusy( );
}

void SX126xSetRfTxPower( int8_t power )
{
	SX126xSetTxParams( power, RADIO_RAMP_40_US );
}

uint8_t SX126xGetDeviceId( void )
{
	return SX1262;
}

void SX126xAntSwOn( void )
{
    	sx126x_pa_control(0);
}

void SX126xAntSwOff( void )
{
    	sx126x_pa_control(-1);
}

bool SX126xCheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}
