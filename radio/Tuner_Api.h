/***********************************************************************//**
 * @file		Hero.h
 * @purpose		This example used to test NXP HERO function
 * @version		0.1
 * @date		1. Oct. 2010
 * @author		NXP BU Automotive Car Entertainment Team & VITEC ELECTRONICS(SHENZHEN) AE Team
 *---------------------------------------------------------------------
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors and VITEC Electronics assume no responsibility or liability for the
 * use of the software, convey no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors and VITEC Electronics
 * reserve the right to make changes in the software without
 * notification. NXP Semiconductors and VITEC Electronics also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/
#ifndef __RADIO_FUNC_H
#define __RADIO_FUNC_H

//#define RADIO_D3_ENABLE
#define RADIO_LITHIO_ENABLE


typedef enum{
	Radio_Hero,
	Radio_Atomic,
	Radio_Atomic2,
	Radio_Lithio,
	Radio_Helio,
	Radio_Dirana3,

	Radio_Last
}eDev_Type;
extern eDev_Type RadioDev;

#define Is_Radio_Dirana3	(RadioDev == Radio_Dirana3)
#define Is_Radio_Atomic2	(RadioDev == Radio_Atomic2)
#define Is_Radio_Lithio		(RadioDev == Radio_Lithio)
#define Is_Radio_Hero		(RadioDev == Radio_Hero)


#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW  0;
#endif

typedef enum{
	RADIO_PRIMARY,
	RADIO_SECONDARY,

	RADIO_ID_LAST
}RADIO_ID;


/* tuner mode */
#define Radio_POWERDOWNMODE 0
#define Radio_PRESETMODE 0x10
#define Radio_SEARCHMODE 0x20
#define Radio_AFUPDATEMODE 0x30
#define Radio_JUMPMODE 0x40
#define Radio_CHECKMODE 0x50
#define Radio_ENDMODE 0x70

#define TUNER_LEVEL_3dB		3
#define TUNER_LEVEL_5dB		5
#define TUNER_LEVEL_10dB	10
#define TUNER_LEVEL_14dB	14
#define TUNER_LEVEL_20dB	20
#define TUNER_LEVEL_25dB	25
#define TUNER_LEVEL_30dB	30
#define TUNER_LEVEL_35dB	35
#define TUNER_LEVEL_40dB	40
#define TUNER_LEVEL_45dB	45
#define TUNER_LEVEL_50dB	50
#define TUNER_LEVEL_55dB	55
#define TUNER_LEVEL_60dB	60

#define TUNER_PERCENT_27		27
#define TUNER_PERCENT_23		23
#define TUNER_PERCENT_20		20

#define TUNER_OFFSET_10KHz		100
#define TUNER_OFFSET_1500Hz	15
#define TUNER_OFFSET_300KHz		300


#define RADIO_FM_LEVEL_AVAILABLE_TIME	4	//5 ms after tuning
#define RADIO_AM_LEVEL_AVAILABLE_TIME	36	//36 ms after tuning
#define RADIO_USN_AVAILABLE_TIME	34	//34ms

#define FM_SCAN_LEVEL			TUNER_LEVEL_25dB	//dB, 0.5dB/step -8dB~99.5dB
#define AM_SCAN_LEVEL			TUNER_LEVEL_35dB	//dB, 0.5dB/step -8dB~99.5dB
#define FM_SCAN_LEVEL_HI		TUNER_LEVEL_45dB	//dB, 0.5dB/step -8dB~99.5dB
#define AM_SCAN_LEVEL_HI		TUNER_LEVEL_55dB	//dB, 0.5dB/step -8dB~99.5dB

#define FM_USN_DISTURBANCE	TUNER_PERCENT_27	//disturbance = 27%
#define FM_WAM_DISTURBANCE	TUNER_PERCENT_23	//disturbance = 23%

#define FM_FREQ_OFFSET			TUNER_OFFSET_10KHz	//offset = 10k, step=100Hz
#define AM_FREQ_OFFSET			TUNER_OFFSET_1500Hz	//offset = 1.5k, step=100Hz

#define STEREO_Separation_Level_H  TUNER_LEVEL_20dB		//Stereo indication by LEVEL
#define STEREO_Separation_Level_L  TUNER_LEVEL_14dB		//Stereo indication by LEVEL

//RDS search condition
#define RDS_STOP_LEVEL		TUNER_LEVEL_20dB
#define RDS_STOP_USN		TUNER_PERCENT_20
#define RDS_STOP_WAM		TUNER_PERCENT_20
#define RDS_STOP_IFCResult	TUNER_OFFSET_300KHz		//30KHz


/*radio band define*/
#define MaxBandNum 9		 
#define FM1_BAND 0
#define FM2_BAND 1
#define FM3_BAND 2
#define MW_BAND 3
#define LW_BAND 4
#define SW_BAND 5
#define WX_BAND 6
#define DAB_3_BAND 7
#define DAB_L_BAND 8

/*one step define
NOTE:FM Freqency uint is 10KHz,AM Freqency uint is 1KHz*/
#define AM_Step_9k 9  //am step 9khz
#define AM_Step_10k 10 //am step 10khz
#define FM_Step_50k 5 //fm step 50khz
#define FM_Step_100k 10 //fm step 10 0khz
#define FM_Step_200k 20 //fm step 200khz

/*tuner scan/seek condition define*/
#define Radio_ScanLevel 		TUNER_LEVEL_25dB  //dB, 0.5dB/step -8dB~99.5dB
#define Radio_FMScanSen ((uint8_t)((Radio_ScanLevel+8)*2))//DX
#define Radio_FMScanSen_Loc ((uint8_t)((Radio_ScanLevel+TUNER_LEVEL_20dB+8)*2))//   Local Att=DX+20
#define Radio_FMScanUNoise 70
#define Radio_FMScanMNoise 60
#define Radio_FMScanOffset 10

#define Radio_FM_Status 0x20
#define Radio_AMScanSen 		((8+TUNER_LEVEL_35dB)*2)//DX
#define Radio_AMScanSen_Loc 	((8+TUNER_LEVEL_35dB+20)*2)  //   Local Att=DX+20
#define Radio_AMScanOffset 		TUNER_OFFSET_1500Hz
#define Radio_AM_Status 0xA0

#define StereoSeparation_Level_H  ((uint8_t)((TUNER_LEVEL_20dB+8)*2))		//Stereo indication by LEVEL
#define StereoSeparation_Level_L  ((uint8_t)((TUNER_LEVEL_14dB+8)*2))		//Stereo indication by LEVEL
/*check station result */
#define NO_STATION   90
#define PRESENT_STATION        100

/*station max*/
#define MaxStationNum 7/*Station count(6)+current statuin(1)1*/



#define F_UP   1
#define F_DOWN  0
#define StationRecordLen (MaxBandNum*MaxStationNum*2)

/*are config parameters struct*/
typedef struct{         //area radio parameter 
	uint32_t FM_MaxFreq;			 //fm max freq
	uint32_t FM_MinFreq;                        //fm min freq
	uint32_t AM_MaxFreq;                       // am max freq
	uint32_t AM_MinFreq;                        //am min freq
	uint32_t FM_AutoSeekStep;               // fm step
	uint32_t FM_ManualSeekStep;           
	uint32_t AM_AutoSeekStep;              //am step
	uint32_t AM_ManualSeekStep;
}Radio_AreaConfigDef;

/*area define*/
enum Radio_AreaCode{  
    Radio_CHN=0,
    Radio_EUR,
    Radio_USA,
    Radio_JPN
};

 /*band freq range*/
typedef struct     
{
    uint16_t MinFreq;
    uint16_t MaxFreq;
}FreqBaundDef;
 
/*tation freq*/
typedef struct 
{
    uint16_t Freq[MaxStationNum];   //0-current work freq,or backup freq;
}StationMemType;


/*band function*/
extern void Radio_SetBand(uint8_t Band);
extern void Radio_NextBand(void);
extern uint8_t Radio_IsFMBand(void);
extern uint8_t Radio_GetCurrentBand(void);

/*preset function*/
extern void Radio_SaveCurrentFreq2Preset(uint8_t Station);
extern void Radio_SelectPreset(uint8_t Station);
extern void Radio_ClearCurrentStation(void);
extern uint8_t Radio_GetCurrentStation(void);

/*radio freqency function*/
extern void Radio_SetFreq(uint8_t mode,uint8_t Band,uint16_t Freq);
extern uint32_t Radio_GetFreqStep(uint8_t band);
extern void Radio_ChangeFreqOneStep(uint8_t UpDown);
extern uint16_t Radio_GetCurrentFreq(void);

/*parameters*/
extern void Radio_Para_Init(void);


/*check station */
extern void Radio_CheckStation(void);
extern void Radio_CheckStationInit(void);
extern uint8_t Radio_CheckStationStatus(void);

/*read/save  radio setup parameters from/ to eeprom*/
void Radio_ReadStationRecord(void);
void Radio_StoreCurrentBand(void);
void Radio_StoreAll(void);
void Radio_Save_Station(uint8_t StationNumber ,uint16_t StationFreq);
void Radio_StoreStationRecord(uint8_t Band);
void Radio_SetSeekSenLevel(uint8_t Lev);
void Radio_StoreStation(uint8_t Band,uint8_t Station);
uint8_t Radio_CheckStereo(void);
uint16_t Radio_Get_Level(uint8_t fm);
uint8_t Radio_Is_AF_Update_Available (void);
uint8_t Radio_Is_RDAV_Available (void);
uint16_t Radio_Get_RDS_Data(uint32_t*rds_data);

#endif
