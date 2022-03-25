
#include <math.h>
#include <time.h>

#include "utilities.h"
#include "delay.h"
#include "timer.h"
#include "rtc-board.h"
#include "app_timer.h"
#include "system_time.h"

//由于之前的工程使用过的是52810的工程，所以没有RTC2，只能使用RTC1

APP_TIMER_DEF(lora_time_id);
#define MIN_ALARM_DELAY                             APP_TIMER_MIN_TIMEOUT_TICKS // in ticks
#define RTC_FREQ 				(APP_TIMER_CLOCK_FREQ/(APP_TIMER_CONFIG_RTC_FREQUENCY+1))	
#define TICK2MS(A)		(A/RTC_FREQ)
/*!
 * RTC timer context
 */
typedef struct
{
    uint32_t Time;  // Reference time
    uint32_t Delay; // Reference Timeout duration

} RtcTimerContext_t;

/*!
 * Keep the value of the RTC timer when the RTC alarm is set
 * Set with the \ref RtcSetTimerContext function
 * Value is kept as a Reference to calculate alarm
 */
static RtcTimerContext_t RtcTimerContext;
uint32_t RtcBkupRegisters[] = { 0, 0 };


//rtc中断回调函数，调用TimerIrqHandler处理
static void lora_time_Handle(void * p_context)
{
    UNUSED_PARAMETER(p_context);
    TimerIrqHandler( );
}
static uint8_t rtc_init_flag = 0;
//lorawan rtc初始化
void loraTime_Init(void)
{
	if(rtc_init_flag==0)
	{
		uint32_t err_code;
		// Create timers.
		err_code = app_timer_create(&lora_time_id,
		                            APP_TIMER_MODE_SINGLE_SHOT,
		                            lora_time_Handle);
		APP_ERROR_CHECK(err_code);
		rtc_init_flag = 1;
	}

}

void RtcInit( void )
{
    loraTime_Init();
    RtcSetTimerContext();
}
uint32_t RtcSetTimerContext( void )
{
    RtcTimerContext.Time = ( uint32_t )get_rtc();
    return ( uint32_t )RtcTimerContext.Time;
}

uint32_t RtcGetTimerContext( void )
{
    return RtcTimerContext.Time;
}

/*!
 * \brief returns the wake up time in ticks
 *
 * \retval wake up time in ticks
 */
uint32_t RtcGetMinimumTimeout( void )
{
    return( MIN_ALARM_DELAY );
}

/*!
 * \brief converts time in ms to time in ticks
 *
 * \param[IN] milliseconds Time in milliseconds
 * \retval returns time in timer ticks
 */
uint32_t RtcMs2Tick( uint32_t milliseconds )
{
    return ( uint32_t )APP_TIMER_TICKS(milliseconds);
}

/*!
 * \brief converts time in ticks to time in ms
 *
 * \param[IN] time in timer ticks
 * \retval returns time in milliseconds
 */
uint32_t RtcTick2Ms( uint32_t tick )
{
//    uint32_t seconds = TICK2MS(tick);

//    tick = tick % RTC_FREQ;
 //   return ( ( seconds * 1000 ) + ( ( tick * 1000 ) /RTC_FREQ ) );
	return ((uint32_t)ROUNDED_DIV((tick) * ((uint64_t)(1000 * (APP_TIMER_CONFIG_RTC_FREQUENCY + 1))), (uint64_t)APP_TIMER_CLOCK_FREQ));
}

/*!
 * \brief a delay of delay ms by polling RTC
 *
 * \param[IN] delay in ms
 */
void RtcDelayMs( uint32_t delay )
{
    uint64_t delayTicks = 0;
    uint64_t refTicks = RtcGetTimerValue( );

    delayTicks = RtcMs2Tick( delay );

    // Wait delay ms
    while( ( ( RtcGetTimerValue( ) - refTicks ) ) < delayTicks )
    {
        __NOP( );
    }
}

/*!
 * \brief Sets the alarm
 *
 * \note The alarm is set at now (read in this function) + timeout
 *
 * \param timeout Duration of the Timer ticks
 */
void RtcSetAlarm( uint32_t timeout )
{
    RtcStartAlarm( timeout );
}

void RtcStopAlarm( void )
{
    app_timer_stop(lora_time_id);
}

void RtcStartAlarm( uint32_t timeout )
{
    app_timer_stop(lora_time_id);
    app_timer_start(lora_time_id, timeout, NULL);
}

uint32_t RtcGetTimerValue( void )
{
    return ( uint32_t )get_rtc();
}

uint32_t RtcGetTimerElapsedTime( void )
{
  	uint32_t time = ( uint32_t)( get_rtc()- RtcTimerContext.Time );
	if(time>0x80000000) //溢出了 //RTC中断处理时RTC溢出了
	{
		uint64_t temp = get_rtc() + 0x01000000;  //因为52832是24的计数器，如果溢出了在计算的时候加上进位
		time = (uint32_t)(temp-RtcTimerContext.Time) ;
	}
    	return time;
}


uint32_t RtcGetCalendarTime( uint16_t *milliseconds )
{
    	uint32_t sec;
	uint32_t remain_tick;
	sec = (uint32_t)(get_rtc()/(uint64_t)RTC_FREQ);
	remain_tick = (uint32_t)(get_rtc()%(uint64_t)RTC_FREQ);
    	*milliseconds=(uint16_t)RtcTick2Ms(remain_tick);
    	return sec;
}



void RtcBkupWrite( uint32_t data0, uint32_t data1 )
{

    RtcBkupRegisters[0] = data0;
    RtcBkupRegisters[1] = data1;

}

void RtcBkupRead( uint32_t* data0, uint32_t* data1 )
{

    *data0 = RtcBkupRegisters[0];
    *data1 = RtcBkupRegisters[1];

}

void RtcProcess( void )
{

}

TimerTime_t RtcTempCompensation( TimerTime_t period, float temperature )
{
    return period;
}
