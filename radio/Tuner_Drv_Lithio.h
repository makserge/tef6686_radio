

#ifndef _ATOMIC2_DRV_H_
#define _ATOMIC2_DRV_H_


#define TEF665X_FM_FREQUENCY_MIN	6500		//step 10kHz
#define TEF665X_FM_FREQUENCY_MAX	10800
#define TEF665X_FM_FREQUENCY_STEP	10

#define TEF665X_LW_FREQUENCY_MIN	144			//step 1kHz
#define TEF665X_LW_FREQUENCY_MAX	288
#define TEF665X_LW_FREQUENCY_STEP	1

#define TEF665X_MW_FREQUENCY_MIN	522			//step 1kHz
#define TEF665X_MW_FREQUENCY_MAX	1710
#define TEF665X_MW_FREQUENCY_STEP	1

#define TEF665X_SW_FREQUENCY_MIN	2300		//step 1kHz
#define TEF665X_SW_FREQUENCY_MAX	27000
#define TEF665X_SW_FREQUENCY_STEP	1

#define TEF665X_Is_FM_Freq(a)	((a>=TEF665X_FM_FREQUENCY_MIN)&&(a<=TEF665X_FM_FREQUENCY_MAX))
#define TEF665X_Is_LW_Freq(a)	((a>=TEF665X_LW_FREQUENCY_MIN)&&(a<=TEF665X_LW_FREQUENCY_MAX))
#define TEF665X_Is_MW_Freq(a)	((a>=TEF665X_MW_FREQUENCY_MIN)&&(a<=TEF665X_MW_FREQUENCY_MAX))
#define TEF665X_Is_SW_Freq(a)	((a>=TEF665X_SW_FREQUENCY_MIN)&&(a<=TEF665X_SW_FREQUENCY_MAX))
#define TEF665X_Is_AM_Freq(a)	((TEF665X_Is_LW_Freq(a))||(TEF665X_Is_MW_Freq(a))||(TEF665X_Is_SW_Freq(a)))

typedef enum
{
	TEF665X_MODULE_FM = 32,
	TEF665X_MODULE_AM = 33,
	TEF665X_MODULE_AUDIO   = 48,
	TEF665X_MODULE_APPL  = 64
} TEF668x_MODULE;

typedef enum
{
	TEF665X_Cmd_Tune_To  =  1,
	TEF665X_Cmd_Set_Tune_Options   =2,
	TEF665X_Cmd_Set_Bandwidth      =10,
	TEF665X_Cmd_Set_RFAGC            =11,
	TEF665X_Cmd_Set_Antenna =12,

	TEF665X_Cmd_Set_MphSuppression = 20,
	TEF668X_Cmd_Set_ChannelEqualizer = 22,	//only Lithio avaliable
	TEF665X_Cmd_Set_NoiseBlanker = 23,
	TEF665X_Cmd_Set_NoiseBlanker_Audio = 24,

	TEF665X_Cmd_Set_DigitalRadio = 30,
	TEF665X_Cmd_Set_Deemphasis = 31,
	TEF668X_Cmd_Set_StereoImprovement = 32,	//only Lithio avaliable

	TEF665X_Cmd_Set_LevelStep = 38,
	TEF665X_Cmd_Set_LevelOffset = 39,

	TEF665X_Cmd_Set_Softmute_Time = 40,
	TEF665X_Cmd_Set_Softmute_Mod = 41,
	TEF665X_Cmd_Set_Softmute_Level = 42,
	TEF665X_Cmd_Set_Softmute_Noise = 43,
	TEF665X_Cmd_Set_Softmute_Mph = 44,
	TEF665X_Cmd_Set_Softmute_Max = 45,

	TEF665X_Cmd_Set_Highcut_Time = 50,
	TEF665X_Cmd_Set_Highcut_Mod = 51,
	TEF665X_Cmd_Set_Highcut_Level = 52,
	TEF665X_Cmd_Set_Highcut_Noise = 53,
	TEF665X_Cmd_Set_Highcut_Mph = 54,
	TEF665X_Cmd_Set_Highcut_Max = 55,
	TEF665X_Cmd_Set_Highcut_Min = 56,
	TEF665X_Cmd_Set_Lowcut_Min = 58,
	
	TEF665X_Cmd_Set_Stereo_Time = 60,
	TEF665X_Cmd_Set_Stereo_Mod = 61,
	TEF665X_Cmd_Set_Stereo_Level = 62,
	TEF665X_Cmd_Set_Stereo_Noise = 63,
	TEF665X_Cmd_Set_Stereo_Mph = 64,
	TEF665X_Cmd_Set_Stereo_Max = 65,
	TEF665X_Cmd_Set_Stereo_Min = 66,
	
	TEF668X_Cmd_Set_StHiBlend_Time = 70,	//only Lithio avaliable
	TEF668X_Cmd_Set_StHiBlend_Mod = 71, //only Lithio avaliable
	TEF668X_Cmd_Set_StHiBlend_Level  = 72, //only Lithio avaliable
	TEF668X_Cmd_Set_StHiBlend_Noise  = 73, //only Lithio avaliable
	TEF668X_Cmd_Set_StHiBlend_Mph  = 74, //only Lithio avaliable
	TEF668X_Cmd_Set_StHiBlend_Max  = 75, //only Lithio avaliable
	TEF668X_Cmd_Set_StHiBlend_Min  = 76, //only Lithio avaliable

	TEF665X_Cmd_Set_Scaler = 80,
	TEF665X_Cmd_Set_RDS = 81,
	TEF665X_Cmd_Set_QualityStatus = 82,
	TEF665X_Cmd_Set_DR_Blend = 83,
	TEF665X_Cmd_Set_DR_Options = 84,
	TEF665X_Cmd_Set_Specials = 85,

	TEF668X_Cmd_Set_StBandBlend_Time = 90,	//only Lithio avaliable
	TEF668X_Cmd_Set_StBandBlend_Gain = 91, //only Lithio avaliable
	TEF668X_Cmd_Set_StBandBlend_Bias  = 92, //only Lithio avaliable
	
	TEF665X_Cmd_Get_Quality_Status = 128,
	TEF665X_Cmd_Get_Quality_Data = 129,
	TEF665X_Cmd_Get_RDS_Status = 130,
	TEF665X_Cmd_Get_RDS_Data = 131,
	TEF665X_Cmd_Get_AGC = 132,
	TEF665X_Cmd_Get_Signal_Status = 133,
	TEF665X_Cmd_Get_Processing_Status = 134,
	TEF665X_Cmd_Get_Interface_Status = 135,
} TEF668x_RADIO_COMMAND;

typedef enum
{
	TEF665X_Cmd_Set_Volume = 10,
	TEF665X_Cmd_Set_Mute = 11,
	TEF665X_Cmd_Set_Input = 12,
	TEF665X_Cmd_Set_Output_Source = 13,
	
	TEF665X_Cmd_Set_Ana_Out = 21,
	TEF665X_Cmd_Set_Dig_IO = 22,
	TEF665X_Cmd_Set_Input_Scaler = 23,
	TEF665X_Cmd_Set_WaveGen = 24
} TEF668x_AUDIO_COMMAND;

typedef enum
{
	TEF665X_Cmd_Set_OperationMode = 1,
	TEF665X_Cmd_Set_GPIO = 3,
	TEF665X_Cmd_Set_ReferenceClock = 4,
	TEF665X_Cmd_Activate = 5,

	TEF665X_Cmd_Get_Operation_Status = 128,
	TEF665X_Cmd_Get_GPIO_Status = 129,
	TEF665X_Cmd_Get_Identification = 130,
	TEF665X_Cmd_Get_LastWrite = 131
} TEF668x_APPL_COMMAND;


/***********************/
/* Write mode commands */
/***********************/

#define TEF665X_COMMAND_TUNETO                  0x01U
#define TEF665X_COMMAND_SETTUNEOPTIONS          0x02U
#define TEF665X_COMMAND_SETBANDWIDTH            0x0AU
#define TEF665X_COMMAND_SETRFAGC                0x0BU
#define TEF665X_COMMAND_SETANTENNA              0x0CU

#define TEF665X_COMMAND_SETMPHSUPPRESSION       0x14U
#define TEF665X_COMMAND_SETCHANNELEQUALIZER     0x16U
#define TEF665X_COMMAND_SETNOISEBLANKER         0x17U
#define TEF665X_COMMAND_SETNOISEBLANKERAUDIO    0x18U

#define TEF665X_COMMAND_SETDIGITALRADIO         0x1EU
#define TEF665X_COMMAND_SETDEEMPHASIS           0x1FU

#define TEF665X_COMMAND_SETLEVELOFFSET          0x27U

#define TEF665X_COMMAND_SETSOFTMUTETIME         0x28U
#define TEF665X_COMMAND_SETSOFTMUTEMOD          0x29U
#define TEF665X_COMMAND_SETSOFTMUTELEVEL        0x2AU
#define TEF665X_COMMAND_SETSOFTMUTENOISE        0x2BU
#define TEF665X_COMMAND_SETSOFTMUTEMPH          0x2CU
#define TEF665X_COMMAND_SETSOFTMUTEMAX          0x2DU

#define TEF665X_COMMAND_SETHIGHCUTTIME          0x32U
#define TEF665X_COMMAND_SETHIGHCUTMOD           0x33U
#define TEF665X_COMMAND_SETHIGHCUTLEVEL         0x34U
#define TEF665X_COMMAND_SETHIGHCUTNOISE         0x35U
#define TEF665X_COMMAND_SETHIGHCUTMPH           0x36U
#define TEF665X_COMMAND_SETHIGHCUTMAX           0x37U
#define TEF665X_COMMAND_SETHIGHCUTMIN           0x38U
#define TEF665X_COMMAND_SETLOWCUTMAX            0x39U
#define TEF665X_COMMAND_SETLOWCUTMIN            0x3AU

#define TEF665X_COMMAND_SETSTEREOTIME           0x3CU
#define TEF665X_COMMAND_SETSTEREOMOD            0x3DU
#define TEF665X_COMMAND_SETSTEREOLEVEL          0x3EU
#define TEF665X_COMMAND_SETSTEREONOISE          0x3FU
#define TEF665X_COMMAND_SETSTEREOMPH            0x40U
#define TEF665X_COMMAND_SETSTEREOMAX            0x42U

#define TEF665X_COMMAND_SETSTHIBLENDTIME        0x46U
#define TEF665X_COMMAND_SETSTHIBLENDMOD         0x47U
#define TEF665X_COMMAND_SETSTHIBLENDLEVEL       0x48U
#define TEF665X_COMMAND_SETSTHIBLENDNOISE       0x49U
#define TEF665X_COMMAND_SETSTHIBLENDMPH         0x4AU
#define TEF665X_COMMAND_SETSTHIBLENDMAX         0x4BU
#define TEF665X_COMMAND_SETSTHIBLENDMIN         0x4CU

#define TEF665X_COMMAND_SETSCALER               0x50U
#define TEF665X_COMMAND_SETRDS                  0x51U
#define TEF665X_COMMAND_SETQUALITYSTATUS        0x52U
#define TEF665X_COMMAND_SETDRBLEND              0x53U
#define TEF665X_COMMAND_SETDROPTIONS            0x54U
#define TEF665X_COMMAND_SETSPECIALS             0x55U

#define TEF665X_COMMAND_SETVOLUME               0x0AU
#define TEF665X_COMMAND_SETMUTE                 0x0BU
#define TEF665X_COMMAND_SETINPUT                0x0CU

#define TEF665X_COMMAND_SETANAOUT               0x15U
#define TEF665X_COMMAND_SETDIGIO                0x16U
#define TEF665X_COMMAND_SETINPUTSCALER          0x17U
#define TEF665X_COMMAND_SETWAVEGEN              0x18U

#define TEF665X_COMMAND_SETOPERATIONMODE        0x01U
#define TEF665X_COMMAND_SETKEYCODE              0x02U
#define TEF665X_COMMAND_SETGPIO                 0x03U
#define TEF665X_COMMAND_SETREFERENCECLOCK       0x04U
#define TEF665X_COMMAND_ACTIVATE                0x05U

/**********************/
/* Read mode commands */
/**********************/

#define TEF665X_API_READ_LENWR                  3
#define TEF665X_API_READ_QSTATUS_LEN            2
#define TEF665X_API_READ_QDATA_LEN              14
#define TEF665X_API_READ_AGC_LEN                4
#define TEF665X_API_READ_RDSSTATUS_LEN          2
#define TEF665X_API_READ_RDSDATA_LEN            10
#define TEF665X_API_READ_BLOCKERR_LEN           2

#define TEF665X_COMMAND_GETQUALITYSTATUS        0x80U
#define TEF665X_COMMAND_GETQUALITYDATA          0x81U
#define TEF665X_COMMAND_GETRDSSTATUS            0x82U
#define TEF665X_COMMAND_GETRDSDATA              0x83U
#define TEF665X_COMMAND_GETAGC                  0x84U
#define TEF665X_COMMAND_GETSIGNALSTATUS         0x85U
#define TEF665X_COMMAND_GETPROCESSINGSTATUS     0x86U
#define TEF665X_COMMAND_GETINTERFACESTATUS      0x87U

#define TEF665X_COMMAND_GETOPERATIONSTATUS      0x80U
#define TEF665X_COMMAND_GETGPIOSTATUS           0x81U
#define TEF665X_COMMAND_GETIDENTIFICATION       0x82U
#define TEF665X_COMMAND_GETLASTWRITE            0x83U

/****************/
/* Byte offsets */
/****************/

#define TEF665X_INDEX_TUNETO_MODE               1
#define TEF665X_INDEX_TUNETO_FREQUENCY          2

/**************/
/* Byte masks */
/**************/

/* Get_Quality */
#define TEF665X_GETQUALITY_STATUS_TIMESTAMP_MSK 0x01FF
#define TEF665X_GETQUALITY_STATUS_TIMESTAMP_POS 0

#define TEF665X_GETQUALITY_STATUS_AFUPDATE_MSK  0x8000
#define TEF665X_GETQUALITY_STATUS_AFUPDATE_POS  15



uint16_t devTEF668x_Set_Cmd(TEF668x_MODULE module, uint8_t cmd, uint16_t len,...);

uint16_t devTEF668x_Radio_Tune_To (uint8_t fm,uint16_t mode,uint16_t frequency );
uint16_t devTEF668x_Radio_Get_Quality_Status (uint8_t fm,uint8_t *status);
uint16_t devTEF668x_Radio_Get_Quality_Level (uint8_t fm,uint8_t *status,int16_t *level);
uint16_t devTEF668x_Radio_Get_Quality_Data (uint8_t fm,uint8_t *usn,uint8_t *wam,uint16_t *offset);
uint16_t devTEF668x_APPL_Get_Operation_Status(uint8_t *status);
uint16_t devTEF668x_APPL_Set_ReferenceClock(uint16_t frequency_high,uint16_t frequency_low,uint16_t type);
uint16_t devTEF668x_APPL_Activate(uint16_t mode);
uint16_t devTEF668x_Audio_Set_Mute(uint16_t mode);
uint16_t devTEF668x_Audio_Set_Volume(int16_t volume);
uint16_t devTEF668x_Radio_Get_Signal_Status(uint8_t fm,uint16_t *status);
uint16_t devTEF668x_Para_Load(void);
uint16_t devTEF668x_Audio_Set_Dig_IO(uint16_t signal,uint16_t mode,uint16_t format,uint16_t operation,uint16_t samplerate);
uint16_t devTEF668x_Audio_Set_Output_Source(uint16_t signal,uint16_t source);
uint16_t devTEF668x_APPL_Set_OperationMode(uint16_t mode);
uint8_t devTEF668x_Radio_Is_AF_Update_Available (void);
uint8_t devTEF668x_Radio_Is_RDAV_Available (void);
uint8_t devTEF668x_Radio_Get_RDS_Data(uint8_t fm,uint16_t *status,uint16_t *A_block,uint16_t *B_block,uint16_t *C_block,uint16_t *D_block, uint16_t *dec_error);
uint16_t devTEF668x_Radio_Get_RDS_DataRaw(uint8_t fm,uint16_t *status,uint32_t *raw_data);
uint16_t devTEF668x_Radio_Set_ChannelEqualizer(uint8_t fm,uint16_t mode);
uint16_t devTEF668x_Radio_Set_StereoImprovement(uint8_t fm,uint16_t mode);
uint16_t devTEF668x_Radio_Set_StHiBlend_Time(uint8_t fm,uint16_t slow_attack,uint16_t slow_decay,uint16_t fast_attack,uint16_t fast_decay);


#endif 
