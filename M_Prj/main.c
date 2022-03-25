#include "platform_driver.h"
#include "system_time.h"
#include "wdt.h"
#include "system_port.h"
#include "nrf_drv_clock.h"
#include "lorawan_app.h"

void HardFault_Handler(void)
{
	SYS_LOG_INFO("HardFault_Handler\r\n");
       app_error_handler(0xDEADBEEF, __LINE__, __FILE__);
}     
static void lfclk_config(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);
}
int main(void)
{
	SYS_LOG_INFO("sys start\r\n");
	SYS_LOG_INFO("---chip ram size is %x\r\n",NRF_FICR->INFO.RAM*1024);
	Platform_driver_init();	
	lfclk_config();
	systime_init();
	systime_start(500);


	while(1)
	{	

		lorawan_test();
		V_FeedWdog();	
		power_manage();
	}
}

 


