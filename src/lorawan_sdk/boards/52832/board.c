/*!

	20200814 by ysh
	�����к�����������Ϊ�գ�������Ҫ��ֲBoardGetBatteryLevel����(��֤��Ҫ)
 */

#include "board.h"

//#include "nordic_common.h"
#include "app_util_platform.h"



uint8_t _mask=0;
void BoardCriticalSectionBegin( uint32_t *mask )
{
	app_util_critical_region_enter(&_mask);
}

void BoardCriticalSectionEnd( uint32_t *mask )
{
 	app_util_critical_region_exit(_mask);
}

void BoardInitPeriph( void )
{

}

void BoardInitMcu( void )
{

}

void BoardResetMcu( void )
{

}

void BoardDeInitMcu( void )
{

}

uint32_t BoardGetRandomSeed( void )
{
	return 0;  
}

void BoardGetUniqueId( uint8_t *id )
{

}

uint16_t BoardBatteryMeasureVoltage( void )
{
    return 0;
}

uint32_t BoardGetBatteryVoltage( void )
{
    return 0;
}

//��һ��ĺ�����Ҫʵ�֣������������к���˵��
uint8_t BoardGetBatteryLevel( void )
{
    return 0;
}


void SystemClockConfig( void )
{
   
}

void CalibrateSystemWakeupTime( void )
{

}

void SystemClockReConfig( void )
{
 
}

void SysTick_Handler( void )
{
  
}

uint8_t GetBoardPowerSource( void )
{
	return 0;
}

/**
  * \brief Enters Low Power Stop Mode
  *
  * \note ARM exists the function when waking up
  */
void LpmEnterStopMode( void)
{
 
}

/*!
 * \brief Exists Low Power Stop Mode
 */
void LpmExitStopMode( void )
{

}

/*!
 * \brief Enters Low Power Sleep Mode
 *
 * \note ARM exits the function when waking up
 */
void LpmEnterSleepMode( void)
{
   
}

void BoardLowPowerHandler( void )
{

}

