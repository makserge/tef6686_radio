#include "tef6686.h"


/* -----------------------------------------------------------------------------
 * Internal define:
 *-----------------------------------------------------------------------------*/
/* -----------------------------------------------------------------------------
 * Internal Prototypes:
 *-----------------------------------------------------------------------------*/


/****************************************************************/
/* Function Calls for Control Commands and Information Requests */
/****************************************************************/
//--------------------------------------------------------------------------------
int Radio_Tune_To(AR_TuningAction_t mode, uint16_t frequency)
{
	int status;

	status = devTEF668x_Radio_Tune_To(TEF665X_Is_FM_Freq(frequency), (uint16_t)mode, frequency);

	return status;
}

int APPL_Get_Operation_Status(TUNER_STATE *status)
{
	uint8_t data;
	int r;

	if(1 ==(r = devTEF668x_APPL_Get_Operation_Status(&data)))
	{
		switch(data)
		{
			case 0:
				*status = eTuner_Boot_state;
				break;
			case 1:
				*status = eTuner_Idle_state;
				break;
			default:
				*status = eTuner_Active_state;
				break;
		}
	}
	
	return r;
}

int APPL_Set_ReferenceClock(uint32_t frequency, uint8_t is_ext_clk)
{
	return devTEF668x_APPL_Set_ReferenceClock((uint16_t)(frequency >> 16), (uint16_t)frequency, is_ext_clk);
}

int APPL_Activate(void)
{
	return devTEF668x_APPL_Activate(1);
}

int APPL_Get_Quality_Status(uint8_t fm, uint8_t *status )
{
	return devTEF668x_Radio_Get_Quality_Status (fm,status);
}

//mute=1, unmute=0
int AUDIO_Set_Mute(uint8_t mute)
{
		return devTEF668x_Audio_Set_Mute(mute);//AUDIO_Set_Mute mode = 0 : disable mute
}
//-60 -- +24 dB volume
int AUDIO_Set_Volume(int vol)
{
	return devTEF668x_Audio_Set_Volume((int16_t)vol);
}

//TRUE = ON;
//FALSE = OFF
void APPL_PowerSwitch(uint8_t OnOff)
{
	devTEF668x_APPL_Set_OperationMode(OnOff? 0 : 1);//standby mode = 1
}

//enable == TRUE, enable CEQ, FALSE--> disable CEQ
void Radio_ChannelEqualizer(uint8_t enable)
{	
	devTEF668x_Radio_Set_ChannelEqualizer(1,enable);
}

//--------------------------------------------------------------------------------
