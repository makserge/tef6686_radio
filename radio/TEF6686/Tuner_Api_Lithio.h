
#ifndef _ATOMIC2_RADIO_H_
#define _ATOMIC2_RADIO_H_


/*
 state transition times will fall within the following limits :
	Power-on  -> Boot state : power supply voltage settling + 5 ms.
	Boot state ->   Idle state : 50 ms.
	Idle state  ->  Active state : 100 ms
*/
typedef enum{
	eTuner_Power_on,
	eTuner_Boot_state,
	eTuner_Idle_state,
	eTuner_Wait_Active,
	eTuner_Active_state,
	
	eTuner_Not_Exist,	
	
	eTuner_Last
}TUNER_STATE;


/*---------------------------------------------------------------------------------
function define start
----------------------------------------------------------------------------------*/
typedef enum
{
	eAR_TuningAction_Standby	  = 0, 
	eAR_TuningAction_Preset 	  = 1, /*!< Tune to new program with short mute time */
	eAR_TuningAction_Search 	  = 2, /*!< Tune to new program and stay muted */
	eAR_TuningAction_AF_Update	  = 3, /*!< Tune to alternative frequency, store quality and tune back with inaudible mute */
	eAR_TuningAction_Jump		  = 4, /*!< Tune to alternative frequency with short inaudible mute  */
	eAR_TuningAction_Check		  = 5, /*!< Tune to alternative frequency and stay muted */
	eAR_TuningAction_End		  = 7  /*!< Release the mute of a Search/Check action (frequency is ignored) */
} AR_TuningAction_t, *pAR_TuningAction_t;


typedef enum
{
	eAR_AFUMode_Fixed				= 0, /*!< Bandwidth control according to parameters  */
	eAR_AFUMode_Automatic			= 1  /*!< Optimal bandwidth on basis of signal conditions. Other parameters will beignored */
} AR_AFUMode_t, *pAR_AFUMode_t;

typedef enum
{
	eAR_BWMode_Fixed				= 0, /*!< Bandwidth control according to parameters */
	eAR_BWMode_Automatic			= 1  /*!< Optimal bandwidth on basis of signal conditions. Other parameters will be ignored */
} AR_BWMode_t, *pAR_BWMode_t;

typedef enum
{
	eAR_TimerMode_Disable			= 0, /*!< Disable Timer (for evaluation only) */
	eAR_TimerMode_FastTimer 		= 1, /*!< Use fast timer control  */
	eAR_TimerMode_SlowTimer 		= 2, /*!< Use slow timer control */
	eAR_TimerMode_DualTimer 		= 3  /*!< Combined fast and slow timer control */
} AR_TimerMode_t, *pAR_TimerMode_t;

typedef enum
{
	eAR_LimitMode_NoLimit			= 0, /*!< Maximum channel separation */
	eAR_LimitMode_Fixed 			= 1, /*!< Channel separation set by limit parameter */
	eAR_LimitMode_ForceMono 		= 2  /*!< No Channel separation */
} AR_LimitMode_t, *pAR_LimitMode_t;


typedef enum
{
	eAR_RDSMode_Decoder 			= 0, /*!< decoder mode; output of RDS group data (block A, B, C, D) */
	eAR_RDSMode_Demodulator 		= 1  /*!< demodulator mode; output in raw demodulator data format */
} AR_RDSMode_t, *pAR_RDSMode_t;

typedef enum
/*! \brief RDS decoder restart control*/
{
	eAR_RDSControl_NoControl		= 0, /*!< Disable control of decoder restart */
	eAR_RDSControl_Manual			= 1, /*!< manual restart; start looking for new RDS signal immediately	*/
	eAR_RDSControl_Automatic		= 2  /*!< automatic restart after tuning; start looking for new RDS signal after Preset, Search, Jump or Check tuning action */
} AR_RDSControl_t, *pAR_RDSControl_t;

typedef enum
{
	eAR_RDSPinFunction_NoPin		= 0, /*!< Dont send RDS Data Availability on a pin	  */
	eAR_RDSPinFunction_Interrupt	= 1, /*!< data-available interrupt by dedicated interrupt pin and register */
	eAR_RDSPinFunction_Status		= 2, /*!< data-available status output; active low (?DAVN?) */
	eAR_RDSPinFunction_2wire		= 3  /*!< legacy 2-wire demodulator data and clock output (?RDDA? and ?RDCL?) */
} AR_RDSPinFunction_t, *pAR_RDSPinFunction_t;

typedef enum
{
	eAR_QSIPinFunction_NoPin		= 0, /*!< Dont send QSI information on a pin	*/
	eAR_QSIPinFunction_Interrupt	= 1, /*!< QSI Status interrupt pin & register	*/
	eAR_QSIPinFunction_Level		= 2  /*!< QSI status active output (active LOW) */
} AR_QSIPinFunction_t, *pAR_QSIPinFunction_t;

typedef struct
{
	uint16_t Status;
	int16_t Level;
	int16_t FreqOffset;
	uint8_t IFBandwidth;
	uint16_t Modulation;
} AM_QData_t;

typedef struct
{
	uint16_t Status;
	int16_t Level;
	uint16_t UltraSonicNoise;
	uint16_t WidebandAMMultipath;
	int16_t FreqOffset;
	uint8_t IFBandwidth;
	uint16_t Modulation;
} FM_QData_t;

typedef struct
{
	uint16_t Status;
	uint16_t groupA;
	uint16_t groupB;
	uint16_t groupC;
	uint16_t groupD;
	uint16_t BlockError;
} FM_RDSData_t;

typedef struct
{
	uint16_t InputAtt;
	uint16_t FeedbackAtt;
} AGC_Data_t;

typedef struct
{
	bool Stereo;
	bool DigSignal;
} SignalSts_t;

typedef enum{
		AUDIO_IIS,
		DAC_LR
}eAudio_Output;

typedef enum{
	ANALOG_RADIO,
	IIS_AUDIO,
	AUDIO_PROCESSOR,
	SINE_WAVE
}eAudio_Source;

typedef enum{
	IIS_SD_0,
	IIS_SD_1,

	IO_OFF,
	IO_INPUT,
	IO_OUTPUT,

	IIS_16_BITS,
	IIS_32_BITS,
	LSB_16BITS,
	LSB_18BITS,
	LSB_20BITS,
	LSB_24BITS,

	MODE_SLAVE,
	MODE_MASTER,

	SAMPLE_RATE_3200,
	SAMPLE_RATE_4410,
	SAMPLE_RATE_4800,
	
}eDIG_IO;

/*==============================================================================*/
/* External function/data references:											*/
/*==============================================================================*/

/*==============================================================================*/
/* Exported functions:															*/
/*==============================================================================*/
int Radio_Tune_To(AR_TuningAction_t mode,	int frequency );
int FM_Tune_To(AR_TuningAction_t mode,	 int frequency );
int AM_Tune_To(AR_TuningAction_t mode,	 int frequency );
int APPL_Get_Operation_Status(TUNER_STATE *status);
int APPL_Set_ReferenceClock(unsigned int frequency, bool is_ext_clk);
int APPL_Activate(void);
int APPL_Get_Quality_Status(bool fm, byte *status );
int AUDIO_Set_Mute(bool mute);
int AUDIO_Set_Volume(int vol);
int Set_Output_Source(eAudio_Output out,eAudio_Source source);
int AUDIO_Set_Dig_IO(eDIG_IO sig,eDIG_IO mod,eDIG_IO form,eDIG_IO oper, eDIG_IO sample);
void APPL_PowerSwitch(bool OnOff);

void Radio_ChannelEqualizer(bool enable);

#endif


