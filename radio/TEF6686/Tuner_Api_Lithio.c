

#include "public.h"


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
int Radio_Tune_To(AR_TuningAction_t mode,   U16 frequency )
{
	int status;

	status = devTEF668x_Radio_Tune_To(TEF665X_Is_FM_Freq(frequency), (U16)mode, frequency);

	return status;
}

int APPL_Get_Operation_Status(TUNER_STATE *status)
{
	U8 data;
	int r;

	if(SUCCESS ==(r = devTEF668x_APPL_Get_Operation_Status(&data)))
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

int APPL_Set_ReferenceClock(U32 frequency, bool is_ext_clk)
{
	return devTEF668x_APPL_Set_ReferenceClock((U16)(frequency >> 16), (U16)frequency, is_ext_clk);
}

int APPL_Activate(void)
{
	return devTEF668x_APPL_Activate(1);
}

int APPL_Get_Quality_Status(bool fm,U8 *status )
{
	return devTEF668x_Radio_Get_Quality_Status (fm,status);
}

//mute=1, unmute=0
int AUDIO_Set_Mute(bool mute)
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
void APPL_PowerSwitch(bool OnOff)
{
	devTEF668x_APPL_Set_OperationMode(OnOff? 0 : 1);//standby mode = 1
}

//enable == TRUE, enable CEQ, FALSE--> disable CEQ
void Radio_ChannelEqualizer(bool enable)
{	
	devTEF668x_Radio_Set_ChannelEqualizer(1,enable);
}

//--------------------------------------------------------------------------------



