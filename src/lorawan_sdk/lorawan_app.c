#include "lorawan_app.h"
#include "system_time.h"


static LoRaMacPrimitives_t macPrimitives;
static LoRaMacCallback_t macCallbacks;
static uint8_t lora_process_request=0;
static uint32_t wait_dutycycle_ms = 0;



/*!
 * MAC status strings
 */
const char* MacStatusStrings[24] =
{
    "OK",                            // LORAMAC_STATUS_OK
    "Busy",                          // LORAMAC_STATUS_BUSY
    "Service unknown",               // LORAMAC_STATUS_SERVICE_UNKNOWN
    "Parameter invalid",             // LORAMAC_STATUS_PARAMETER_INVALID
    "Frequency invalid",             // LORAMAC_STATUS_FREQUENCY_INVALID
    "Datarate invalid",              // LORAMAC_STATUS_DATARATE_INVALID
    "Frequency or datarate invalid", // LORAMAC_STATUS_FREQ_AND_DR_INVALID
    "No network joined",             // LORAMAC_STATUS_NO_NETWORK_JOINED
    "Length error",                  // LORAMAC_STATUS_LENGTH_ERROR
    "Region not supported",          // LORAMAC_STATUS_REGION_NOT_SUPPORTED
    "Skipped APP data",              // LORAMAC_STATUS_SKIPPED_APP_DATA
    "Duty-cycle restricted",         // LORAMAC_STATUS_DUTYCYCLE_RESTRICTED
    "No channel found",              // LORAMAC_STATUS_NO_CHANNEL_FOUND
    "No free channel found",         // LORAMAC_STATUS_NO_FREE_CHANNEL_FOUND
    "Busy beacon reserved time",     // LORAMAC_STATUS_BUSY_BEACON_RESERVED_TIME
    "Busy ping-slot window time",    // LORAMAC_STATUS_BUSY_PING_SLOT_WINDOW_TIME
    "Busy uplink collision",         // LORAMAC_STATUS_BUSY_UPLINK_COLLISION
    "Crypto error",                  // LORAMAC_STATUS_CRYPTO_ERROR
    "FCnt handler error",            // LORAMAC_STATUS_FCNT_HANDLER_ERROR
    "MAC command error",             // LORAMAC_STATUS_MAC_COMMAD_ERROR
    "ClassB error",                  // LORAMAC_STATUS_CLASS_B_ERROR
    "Confirm queue error",           // LORAMAC_STATUS_CONFIRM_QUEUE_ERROR
    "Multicast group undefined",     // LORAMAC_STATUS_MC_GROUP_UNDEFINED
    "Unknown error",                 // LORAMAC_STATUS_ERROR
};
/*!
 * MAC event info status strings.
 */
const char* EventInfoStatusStrings[17] =
{
    "OK",                            // LORAMAC_EVENT_INFO_STATUS_OK
    "Error",                         // LORAMAC_EVENT_INFO_STATUS_ERROR
    "Tx timeout",                    // LORAMAC_EVENT_INFO_STATUS_TX_TIMEOUT
    "Rx 1 timeout",                  // LORAMAC_EVENT_INFO_STATUS_RX1_TIMEOUT
    "Rx 2 timeout",                  // LORAMAC_EVENT_INFO_STATUS_RX2_TIMEOUT
    "Rx1 error",                     // LORAMAC_EVENT_INFO_STATUS_RX1_ERROR
    "Rx2 error",                     // LORAMAC_EVENT_INFO_STATUS_RX2_ERROR
    "Join failed",                   // LORAMAC_EVENT_INFO_STATUS_JOIN_FAIL
    "Downlink repeated",             // LORAMAC_EVENT_INFO_STATUS_DOWNLINK_REPEATED
    "Tx DR payload size error",      // LORAMAC_EVENT_INFO_STATUS_TX_DR_PAYLOAD_SIZE_ERROR
    "Downlink too many frames loss", // LORAMAC_EVENT_INFO_STATUS_DOWNLINK_TOO_MANY_FRAMES_LOSS
    "Address fail",                  // LORAMAC_EVENT_INFO_STATUS_ADDRESS_FAIL
    "MIC fail",                      // LORAMAC_EVENT_INFO_STATUS_MIC_FAIL
    "Multicast fail",                // LORAMAC_EVENT_INFO_STATUS_MULTICAST_FAIL
    "Beacon locked",                 // LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED
    "Beacon lost",                   // LORAMAC_EVENT_INFO_STATUS_BEACON_LOST
    "Beacon not found"               // LORAMAC_EVENT_INFO_STATUS_BEACON_NOT_FOUND
};



/*!
 * \brief   MCPS-Confirm event function
 *
 * \param   [IN] mcpsConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void McpsConfirm( McpsConfirm_t *mcpsConfirm )
{

       LORA_DEBUG( "\r\n###### ===== MCPS-Confirm ==== ######\r\n" );
       LORA_DEBUG( "STATUS      : %s\r\n", EventInfoStatusStrings[mcpsConfirm->Status] );
	if( mcpsConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
    	{

	}
}


/*!
 * \brief   MCPS-Indication event function
 *
 * \param   [IN] mcpsIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
static void McpsIndication( McpsIndication_t *mcpsIndication )
{
	LORA_DEBUG( "\r\n###### ===== MCPS-Indication ==== ######\r\n" );
	LORA_DEBUG( "STATUS      : %s\r\n", EventInfoStatusStrings[mcpsIndication->Status] );

	if( mcpsIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK )
	{
		return;
	}

	switch( mcpsIndication->McpsIndication )
	{

		case MCPS_UNCONFIRMED:
		{
			break;
		}
		case MCPS_CONFIRMED:
		{
			break;
		}
		case MCPS_PROPRIETARY:
		{
			break;
		}
		case MCPS_MULTICAST:
		{

			break;
		}
		default:
			break;
	}
	
	
	if( mcpsIndication->FramePending == true )
	{
	// The server signals that it has pending data to be sent.
	// We schedule an uplink as soon as possible to flush the server.

	//OnTxNextPacketTimerEvent( NULL );
	}
	LORA_DEBUG("LORAWAN  recv downlink ,port is %d,rssi is %d,snr is %d,len is %d\r\n",mcpsIndication->Port,mcpsIndication->Rssi,mcpsIndication->Snr,mcpsIndication->BufferSize);
	if(mcpsIndication->BufferSize)
		LORA_DEBUG_DUMP_BUF("LORAWAN recv data ", mcpsIndication->Buffer, mcpsIndication->BufferSize);
}

static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{

    LORA_DEBUG( "\r\n###### ===== MLME-Confirm ==== ######\r\n" );
    LORA_DEBUG( "STATUS      : %s request:%d\r\n", EventInfoStatusStrings[mlmeConfirm->Status] ,mlmeConfirm->MlmeRequest);
    if( mlmeConfirm->Status != LORAMAC_EVENT_INFO_STATUS_OK )
    {
    }
    switch( mlmeConfirm->MlmeRequest )
    {
        case MLME_JOIN:
        {
		
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                	LORA_DEBUG("+JOINING: JOINED\r\nOK\r\n");	
            }
            else
            {
                	LORA_DEBUG("+JOINING: JOIN FAILED\r\nOK\r\n");
            }
            break;
        }
        case MLME_LINK_CHECK:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
  
            }
            break;
        }
	 case MLME_DEVICE_TIME:
	 {
	 	 if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
	 	 {
	 	 }
	 }
        default:
            break;
    }
}





/*!
 * \brief   MLME-Indication event function
 *
 * \param   [IN] mlmeIndication - Pointer to the indication structure.
 */
static void MlmeIndication( MlmeIndication_t *mlmeIndication )
{
	//if( mlmeIndication->Status != LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED )
	{
	    	LORA_DEBUG( "\r\n###### ===== MLME-Indication ==== ######\r\n" );
	    	LORA_DEBUG( "STATUS      : %s\r\n", EventInfoStatusStrings[mlmeIndication->Status] );
	 	LORA_DEBUG( "MlmeIndication is %d\r\n" ,mlmeIndication->MlmeIndication);
	}
	if( mlmeIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK )
	{
	}
	switch( mlmeIndication->MlmeIndication )
	{
	    	case MLME_SCHEDULE_UPLINK:
	    	{
	       	 break;
	    	}
	    	default:
	        	break;
	}
}


static void OnMacProcessNotify( void )
{
    	lora_process_request++;
}



void lorawan_init(LoRaMacRegion_t	region)
{
	LoRaMacStatus_t status;
	RtcInit( );
	SX126xIoInit();

	//MAC层的回调函数，TXdone不会进入MAC层，只有rxdone/rxtimeout/rxerror/txtimeout等才可以进去mac层
	//发送完成后接收超时或者接收到数据调用此函数
	//调用此函数后时可以下发下一条数据的
	macPrimitives.MacMcpsConfirm = McpsConfirm;//tx compelete

	//rxdone 接收完成回调，可以在此函数内部处理接收的数据
	macPrimitives.MacMcpsIndication = McpsIndication;

	//   MLME层接收成功或者接收超时回调
	macPrimitives.MacMlmeConfirm = MlmeConfirm;
	
	macPrimitives.MacMlmeIndication = MlmeIndication;//基本上没有被调用
	
	//获取设备电池状态，MAC指令 、认证需要
	macCallbacks.GetBatteryLevel = BoardGetBatteryLevel;
	macCallbacks.GetTemperatureLevel = NULL;
	
	//这一块暂时对我们没用，不移植
	macCallbacks.NvmContextChange = NULL;


	//函数在 无线层面调用，TX DONE /RX DONE/TX TIMEOUT/RX TIMEOUT/RX ERROR /ACK TIMEOUT
	macCallbacks.MacProcessNotify = OnMacProcessNotify;
	status = LoRaMacInitialization( &macPrimitives, &macCallbacks, region );//LORAMAC_REGION_US915
	if ( status != LORAMAC_STATUS_OK )
	{
	    	LORA_DEBUG( "LoRaMac wasn't properly initialized, error: %s", MacStatusStrings[status] );
	    	// Fatal error, endless loop.
	    	while ( 1 )
	    	{
	    	}
	}
	LoRaMacStart( );
}
void lorawan_task(void)
{
	if( Radio.IrqProcess != NULL )
	{
	    	Radio.IrqProcess( );
	}

	while(lora_process_request)  
	{
	    	lora_process_request--;
	    	LoRaMacProcess( );
	}

}

LoRaMacStatus_t lorawan_join_network(uint8_t dr)
{

	LoRaMacStatus_t status;
	MlmeReq_t mlmeReq;


	mlmeReq.Type = MLME_JOIN;

	mlmeReq.Req.Join.Datarate =  dr;


	// Starts the join procedure
	status = LoRaMacMlmeRequest( &mlmeReq );
	if( status == LORAMAC_STATUS_OK )
	{
		LORA_DEBUG("start join network\r\n");	
	}
	else if(status == LORAMAC_STATUS_DUTYCYCLE_RESTRICTED)
	{
		LORA_DEBUG("lorawan join wait dutycycle %d ms\r\n",mlmeReq.ReqReturn.DutyCycleWaitTime);
		wait_dutycycle_ms = mlmeReq.ReqReturn.DutyCycleWaitTime;
	}
	else
	{
		LORA_DEBUG("join network false,status is %s\r\n",MacStatusStrings[status]);
	}
	return status;
}
//设置的DR只有在ADR关闭的时候生效。ADR开启不生效
LoRaMacStatus_t lora_app_send_frame( uint8_t *buf,uint8_t buflen,uint8_t port,uint8_t is_confirm,uint8_t dr ,uint8_t confirm_nbtrials)
{
    	McpsReq_t mcpsReq;
    	LoRaMacTxInfo_t txInfo;
	LoRaMacStatus_t status;


	status = LoRaMacQueryTxPossible(buflen, &txInfo);
	if(status == LORAMAC_STATUS_OK)
	{
		if( is_confirm == false )
		{
			mcpsReq.Type = MCPS_UNCONFIRMED;
			mcpsReq.Req.Unconfirmed.fPort = port;
			mcpsReq.Req.Unconfirmed.fBuffer = buf;
			mcpsReq.Req.Unconfirmed.fBufferSize = buflen;
			mcpsReq.Req.Unconfirmed.Datarate = dr;
		}
		else
		{
			mcpsReq.Type = MCPS_CONFIRMED;
			mcpsReq.Req.Confirmed.fPort = port;
			mcpsReq.Req.Confirmed.fBuffer = buf;
			mcpsReq.Req.Confirmed.fBufferSize = buflen;
			mcpsReq.Req.Confirmed.NbTrials = confirm_nbtrials;
			mcpsReq.Req.Confirmed.Datarate = dr;
		}
		status = LoRaMacMcpsRequest( &mcpsReq );
	}
	else
	{
		mcpsReq.Type = MCPS_UNCONFIRMED;
		mcpsReq.Req.Unconfirmed.fBuffer = NULL;
		mcpsReq.Req.Unconfirmed.fBufferSize = 0;
		mcpsReq.Req.Unconfirmed.Datarate = dr;
		mcpsReq.Req.Confirmed.fPort = 0;
		status = LoRaMacMcpsRequest( &mcpsReq );
		
		return  status;
	}
	if(status == LORAMAC_STATUS_OK)
	{
	}
	else if(status ==LORAMAC_STATUS_DUTYCYCLE_RESTRICTED )
	{
		wait_dutycycle_ms = mcpsReq.ReqReturn.DutyCycleWaitTime;
	}
	return status;
}









uint8_t lora_mac_set_deveui( uint8_t *deveui)
{
	MibRequestConfirm_t mibset;
	mibset.Type = MIB_DEV_EUI;
	mibset.Param.DevEui = deveui;
	if(LoRaMacMibSetRequestConfirm(&mibset)!=LORAMAC_STATUS_OK)
		return 0;
	return 1;
}
uint8_t lora_mac_set_appeui( uint8_t *appeui)
{
	MibRequestConfirm_t mibset;
	mibset.Type = MIB_JOIN_EUI;
	mibset.Param.JoinEui = appeui;
	if(LoRaMacMibSetRequestConfirm(&mibset)!=LORAMAC_STATUS_OK)
		return 0;
	return 1;
}

uint8_t lora_mac_set_appkey( uint8_t *appkey)
{
	MibRequestConfirm_t mibset;
	mibset.Type = MIB_NWK_KEY;
	mibset.Param.AppKey = appkey;
	if(LoRaMacMibSetRequestConfirm(&mibset)!=LORAMAC_STATUS_OK)
		return 0;
	return 1;
}
ActivationType_t lora_mac_get_net_type(void)
{
	MibRequestConfirm_t mibget;
	mibget.Type = MIB_NETWORK_ACTIVATION;
	LoRaMacMibGetRequestConfirm(&mibget);
	return mibget.Param.NetworkActivation;
}
void lorawan_param_init()
{
	uint8_t deveui[8] = {0x03,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
	uint8_t appeui[8]= { 0x70,0xB3,0xD5,0x7E,0xD0,0x02,0x6B,0x87};
	uint8_t appkey[16]= {0x2B,0x7E,0x15,0x16,0x28,0xAE,0xD2,0xA6,0xAB,0xF7,0x15,0x88,0x09,0xCF,0x4F,0x3C};
	

	lora_mac_set_deveui(deveui);
	lora_mac_set_appeui(appeui);
	lora_mac_set_appkey(appkey);
}
void lorawan_test(void)
{
	static uint8_t state = 0;
	static uint32_t tick;
	static LoRaMacStatus_t status;
	switch(state)
	{
		case 0:
			lorawan_init(LORAMAC_REGION_EU868);
			lorawan_param_init();
			state= 1;
			break;
		case 1:
			status = lorawan_join_network(0);
			if(status == LORAMAC_STATUS_OK)
			{
				state = 2;
			}
			break;
		case 2:
			if(LoRaMacIsBusy()==0)
			{
				if(lora_mac_get_net_type() == ACTIVATION_TYPE_OTAA)
				{
					state = 3;
					tick = get_rtc();
				}
				else 
					state = 1;
			}
			break;
		case 3:
			if(LoRaMacIsBusy()==0 && timepassed_rtc(tick,10000))
			{
				uint8_t testbuf[] = {0,1,2,3,4,5,6,7,8,9};
				status = lora_app_send_frame(testbuf,10,100,0,1,1);
				LORA_DEBUG("lorawan send status is %s\r\n",MacStatusStrings[status]);
				tick = get_rtc();

			}
			break;
	}
	lorawan_task();
}

