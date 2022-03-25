/*!
	20200814 by ysh
 */
#include "nrf_delay.h"
#include "delay-board.h"

void DelayMsMcu( uint32_t ms )
{
    nrf_delay_ms( ms );
}
