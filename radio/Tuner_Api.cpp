/***********************************************************************//**
 * @file		Hero.c
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

#include "TEF6686.h"

/* station */
StationMemType StationRecord[MaxStationNum]; 

/* area config*/
#define AreaSelect Radio_EUR

#if (AreaSelect==Radio_CHN)   /*China */          
const Radio_AreaConfigDef Radio_AreaConfig={
    //FM_MaxFreq  FM_MinFreq  AM_MaxFreq  AM_MinFreq  FM_AutoSeekStep/k  FM_ManualSeekStep/k  AM_AutoSeekStep/k  AM_ManualSeekStep/k
    10800,        8750,      1620,       522,         FM_Step_100k,        FM_Step_100k,                 9,                  9};
	
const FreqBaundDef FreqBaundConfig[MaxBandNum]={		
   /*0-FM1,1-FM2,2-FM3,3-LW,4-MW,5-SW*/
    {8750,10800},{8750,10800},{8750,10800},{522,1710},{144,288},{2300,27000}};    
    
#elif (AreaSelect==Radio_EUR)  /*Europe */   	                        
const Radio_AreaConfigDef Radio_AreaConfig={
//FM_MaxFreq  FM_MinFreq  AM_MaxFreq  AM_MinFreq  FM_AutoSeekStep/k  FM_ManualSeekStep/k  AM_AutoSeekStep/k  AM_ManualSeekStep/k
10800,        8750,      1602,       531,         FM_Step_100k,        FM_Step_100k,               9,                  9};
const FreqBaundDef FreqBaundConfig[MaxBandNum]={		 //0-FM1,1-FM2,2-FM3,3-LW,4-MW,5-SW
    {6500,10800},{6500,10800},{6500,10800},{144,288},{522,1710},{2300,27000}};    // 
    
#elif (AreaSelect==Radio_USA)      /*USA */                
const Radio_AreaConfigDef Radio_AreaConfig={
//FM_MaxFreq  FM_MinFreq  AM_MaxFreq  AM_MinFreq  FM_AutoSeekStep/k  FM_ManualSeekStep/k  AM_AutoSeekStep/k  AM_ManualSeekStep/k
10790,        8750,      1710,       530,        FM_Step_100k,        FM_Step_100k,         10,                  10};
const FreqBaundDef FreqBaundConfig[MaxBandNum]={		 //0-FM1,1-FM2,2-FM3,3-LW,4-MW,5-SW
    {6500,10800},{6500,10800},{6500,10800},{144,288},{522,1710},{2300,27000}};    // 
    
#elif (AreaSelect==Radio_JPN)      /*Japan */                
const Radio_AreaConfigDef Radio_AreaConfig={
//FM_MaxFreq  FM_MinFreq  AM_MaxFreq  AM_MinFreq  FM_AutoSeekStep/k  FM_ManualSeekStep/k  AM_AutoSeekStep/k  AM_ManualSeekStep/k
9000,        7600,      1629,       522,          FM_Step_100k,        FM_Step_100k,               9,                   9};
const FreqBaundDef FreqBaundConfig[MaxBandNum]={		 //0-FM1,1-FM2,2-FM3,3-LW,4-MW,5-SW
    {6500,10800},{6500,10800},{6500,10800},{144,288},{522,1710},{2300,27000}};    // 
    
#endif


/* radio default station */
/*
const StationMemType StationDefaultRecord[MaxStationNum]=			 //now config: MaxBandNum=6, MaxStationNum=7 
{
	{10000,10000,10000,10000,10000,10000,10000},     //fm1				  
	{10000,10000,10000,10000,10000,10000,10000},		//fm2
	{10000,10000,10000,10000,10000,10000,10000},     //fm3
	{522,522,522,522,522,522,522},		//mw
	{144,144,144,144,144,144,144},		//lw
	{2300,2300,2300,2300,2300,2300,2300}		//sw
};
*/
/*check station step*/
static uint8_t CheckIfStep;

/*current radio  band*/
uint8_t Radio_CurrentBand;
/*current radio  freqency*/
uint16_t Radio_CurrentFreq;
/*current radio  station*/
uint8_t Radio_CurrentStation;

#ifdef RADIO_D3_ENABLE
eDev_Type RadioDev = Radio_Dirana3;
#elif defined RADIO_LITHIO_ENABLE
eDev_Type RadioDev = Radio_Lithio;
#elif defined RADIO_ATOMIC_ENABLE
eDev_Type RadioDev = Radio_Atomic;
#elif defined RADIO_ATOMIC2_ENABLE
eDev_Type RadioDev = Radio_Atomic2;
#elif defined RADIO_HELIO_ENABLE
eDev_Type RadioDev = Radio_Helio;
#else
eDev_Type RadioDev = Radio_Hero;
#endif



/*-----------------------------------------------------------------------
Function name:	Radio_SetFreq
Input:		mode:
                              TEF663X_PRESETMODE.,TEF663X_SEARCHMODE
                              TEF663X_AFUPDATEMODE,TEF663X_JUMPMODE...
                        Freq:
Output:			
Description:	 
------------------------------------------------------------------------*/
void Radio_SetFreq(uint8_t mode,uint8_t Band,uint16_t Freq)
{
       /*frequency baundary check*/
	if((Freq>FreqBaundConfig[Radio_CurrentBand].MaxFreq)||(Freq<FreqBaundConfig[Radio_CurrentBand].MinFreq)){
		Freq = FreqBaundConfig[Radio_CurrentBand].MinFreq;
	}
	if(Band>=MaxBandNum){
		return;
	}

//AF_UPDATE not change current info
	if(mode != Radio_AFUPDATEMODE)
	{
		Radio_CurrentBand=Band;
		Radio_CurrentFreq=Freq;
		StationRecord[Radio_CurrentBand].Freq[0]=Radio_CurrentFreq;
	}

	if(Is_Radio_Atomic2||Is_Radio_Lithio)
	{
		AR_TuningAction_t A2Mode;

		switch(mode){
			case Radio_PRESETMODE:
				A2Mode = eAR_TuningAction_Preset;
				break;
			case Radio_SEARCHMODE:
				A2Mode = eAR_TuningAction_Search;
				break;
			case Radio_AFUPDATEMODE:
				A2Mode = eAR_TuningAction_AF_Update;
				break;
			case Radio_JUMPMODE:
				A2Mode = eAR_TuningAction_Jump;
				break;
			case Radio_CHECKMODE:
				A2Mode = eAR_TuningAction_Check;
				break;
		}
		Radio_Tune_To(A2Mode,Freq);
	}
}
/*====================================================
 Function:Radio_ChangeFreqOneStep
 Input: 
      UP/DOWN
 OutPut:
      Null
 Desp:
     change curren freq on step
=========================================================*/
void Radio_ChangeFreqOneStep(uint8_t UpDown )
{
	int step;

	step=Radio_GetFreqStep(Radio_CurrentBand);
	if(UpDown==1)
	{	/*increase one step*/
		Radio_CurrentFreq+=step;
		/*frequency baundary check	*/
		if(Radio_CurrentFreq>FreqBaundConfig[Radio_CurrentBand].MaxFreq)	
		{  
			Radio_CurrentFreq=FreqBaundConfig[Radio_CurrentBand].MinFreq;
		}
	}
	else	
	{      /*decrease one step*/
		Radio_CurrentFreq-=step;  
		/*frequency baundary check*/	
		if(Radio_CurrentFreq<FreqBaundConfig[Radio_CurrentBand].MinFreq)	
		{	
		    	Radio_CurrentFreq=FreqBaundConfig[Radio_CurrentBand].MaxFreq;
		}		
	}

}
/*-----------------------------------------------------------------------
Function name:	Radio_GetCurrentFreq
Input:			
Output:			
Description:	return current freq
------------------------------------------------------------------------*/
uint16_t Radio_GetCurrentFreq(void)
{
	 return Radio_CurrentFreq;
}
/*-----------------------------------------------------------------------
Function name:	Radio_GetCurrentBand
Input:			
Output:			
Description:	 return current band 
------------------------------------------------------------------------*/
uint8_t Radio_GetCurrentBand(void)
{
	 return Radio_CurrentBand;
}

/*-----------------------------------------------------------------------
Function name:	Radio_GetCurrentStation
Input:			
Output:			
Description:	 return current station 
------------------------------------------------------------------------*/
uint8_t Radio_GetCurrentStation(void)
{
	 return Radio_CurrentStation;
}
/*-----------------------------------------------------------------------
Function name:	Radio_ClearCurrentStation
Input:			
Output:			
Description:	 set current station 
------------------------------------------------------------------------*/
void Radio_ClearCurrentStation(void)
{
	  Radio_CurrentStation=0;
}
/*-----------------------------------------------------------------------
Function name:	Radio_GetFreqStep
Input:			
Output:			
Description:	 Get current band freq step
------------------------------------------------------------------------*/
uint32_t Radio_GetFreqStep(uint8_t band)
{
	return (band<=FM3_BAND)? Radio_AreaConfig.FM_ManualSeekStep : Radio_AreaConfig.AM_ManualSeekStep;
}
/*-----------------------------------------------------------------------
Function name:	Radio_IsFMBand
Input:			
Output:			
Description:	 check current band is fm band
------------------------------------------------------------------------*/
uint8_t Radio_IsFMBand(void)
{ 
      return ((Radio_CurrentBand<=FM3_BAND)?1:0 )	;
}

/*-----------------------------------------------------------------------
Function name:	Radio_SetBand
Input:			Band
Output:			
Description:	 set band
------------------------------------------------------------------------*/
void Radio_SetBand(uint8_t Band)
{
	Radio_SetFreq(Radio_PRESETMODE,Band,StationRecord[Band].Freq[0]);
}
/*-----------------------------------------------------------------------
Function name:	Radio_NextBand
Input:			NULL
Output:			
Description:	 set to next band
------------------------------------------------------------------------*/
void Radio_NextBand(void)
{
	Radio_CurrentBand++;
	/*check band avilable*/
	if(Radio_CurrentBand>=MaxBandNum)	
	{
		Radio_CurrentBand=0;
	}
	/*set to current band*/
	Radio_SetBand(Radio_CurrentBand);
	Radio_CurrentFreq=StationRecord[Radio_CurrentBand].Freq[0];
	Radio_CurrentStation=0;
}
/*-----------------------------------------------------------------------
Function name:	Radio_ValidFreq
Input:			band and freqencey pointer
Output:			valid freqency
Description:	 	min<= freq <= max, aligned by step
------------------------------------------------------------------------*/
static void Radio_ValidFreq(uint8_t band, uint16_t *pFreq)
{
	uint16_t step,min,max;

	step = Radio_GetFreqStep(band);
	max = FreqBaundConfig[band].MaxFreq;
	min = FreqBaundConfig[band].MinFreq;

	if((*pFreq > max)||(*pFreq < min))	//frequency baundary check
		*pFreq = min;
	else {  //step check
		int num  = (*pFreq - min)/step;	//align freq by step
		*pFreq = min + num * step;
	}
}
/*-----------------------------------------------------------------------
Function name:	Radio_ReadStationRecord
Input:			
Output:			
Description:	 write  one station to eeprom
------------------------------------------------------------------------*/
void Radio_StoreStation(uint8_t Band,uint8_t Station)
{
  //  EEPROM_Write(EEPRom_StationRecordDefaultAddr + Band *sizeof(StationRecord[0]) +2*Station,
//	(uint8_t *)(&StationRecord[Band].Freq[Station]),2);

}
/*-----------------------------------------------------------------------
Function name:	Radio_ReadStationRecord
Input:			
Output:			
Description:	 read  one station from eeprom
------------------------------------------------------------------------*/
void Radio_ReadStation(uint8_t Band,uint8_t Station)
{
//    EEPROM_Read(EEPRom_StationRecordDefaultAddr + Band *sizeof(StationRecord[0]) +2*Station,
//	(uint8_t *)(&StationRecord[Band].Freq[Station]),2);

}
/*-----------------------------------------------------------------------
Function name:	Radio_ReadStationRecord
Input:			
Output:			
Description:	 read all station from eeprom
------------------------------------------------------------------------*/
void Radio_ReadStationRecord(void)
{
	uint16_t band,station;
	uint16_t *pFreq;
	
	//EEPROM_Read(EEPRom_StationRecordDefaultAddr,(uint8_t *)(&StationRecord[0].Freq[0]),sizeof(StationRecord));
        /* for(band=0;band<MaxBandNum;band++)
	     for(station=0;station<MaxStationNum;station++)
	     	Radio_ReadStation( band, station);*/
		 
	/*set all frequency to valid value*/
	for(band=0;band<MaxBandNum;band++)
	{	
//	        EEPROM_Read(EEPRom_StationRecordDefaultAddr+ band *sizeof(StationRecord[0]),(uint8_t *)(&StationRecord[band].Freq[0]),sizeof(StationRecord[0]));
		pFreq = &StationRecord[band].Freq[0];	
		for(station=0;station<MaxStationNum;station++,pFreq++) 
		{
			Radio_ValidFreq(band,pFreq);
		}
	}
}
/*-----------------------------------------------------------------------
Function name:	Radio_StoreStationRecord
Input:			
Output:			
Description:	 store one band of station to eeprom
------------------------------------------------------------------------*/
void Radio_StoreStationRecord(uint8_t Band)
{
/*
	EEPROM_Write(EEPRom_StationRecordDefaultAddr + Band *sizeof(StationRecord[0]) ,
	(uint8_t *)(&StationRecord[Band].Freq[0]),
	sizeof(StationRecord[0]));	 */
	int i;
	for(i=0;i<MaxStationNum;i++)
	{
		 Radio_StoreStation( Band, i);
	}
	      
}

/*-----------------------------------------------------------------------
Function name:	Radio_ReadCurrentBand
Input:			
Output:			
Description:	read station from eeprom 
------------------------------------------------------------------------*/
uint8_t Radio_ReadCurrentBand(void)
{
	uint8_t band;
	/*read current band from eeprom*/
//	EEPROM_Read(EEPRom_CurrentBandAddr,&band, 1);
	/*if band not aviliable*/
	if(band>=MaxBandNum)
		band=FM1_BAND;
	return band;
}
/*-----------------------------------------------------------------------
Function name:	Radio_StoreCurrentBand
Input:			
Output:			
Description:	 store current band  to eeprom
------------------------------------------------------------------------*/
void Radio_StoreCurrentBand(void)
{
	//EEPROM_Write(EEPRom_CurrentBandAddr,&Radio_CurrentBand,1);
}
/*-----------------------------------------------------------------------
Function name:	Radio_StoreAll
Input:			
Output:			
Description:	 store all the radio para,station to eeprom
------------------------------------------------------------------------*/
void Radio_StoreAll(void)
{
        uint8_t i;

	/*write eeprom available flag*/
	//i=AvailableDataFlag;
	//EEPROM_Write(EEPRom_CheckAddr,&i,1);
	/*store current band*/
	Radio_StoreCurrentBand();
	/*store station*/
	for(i=0;i<MaxBandNum;i++)	
	{
	    Radio_StoreStationRecord(i);
	}
}

/*-----------------------------------------------------------------------
Function name:	
Input:			
Output:			
Description:	 check stereo indicator
------------------------------------------------------------------------*/
uint8_t Radio_CheckStereo(void)
{
	uint16_t Status;
	uint8_t stereo = 0;

	if(Is_Radio_Atomic2||Is_Radio_Lithio)
	{
		//if(1==devTEF668x_Radio_Get_Signal_Status(1,&Status))
			//stereo = ( Status & bit15)? 1 : 0;
	}

	return stereo;
}

uint8_t SeekSenLevel = LOW;
void Radio_SetSeekSenLevel(uint8_t Lev)
{
	SeekSenLevel = Lev;
}

uint8_t Radio_Is_AF_Update_Available (void)
{
	if(Is_Radio_Atomic2||Is_Radio_Lithio)
	{
		return devTEF668x_Radio_Is_AF_Update_Available();
	}

	return 0;
}
uint8_t Radio_Is_RDAV_Available (void)
{
	if(Is_Radio_Atomic2||Is_Radio_Lithio)
	{
		return devTEF668x_Radio_Is_RDAV_Available();
	}
	
	return 0;
}



//level detector result
//output: -200 ... 1200 (0.1 * dBuV) = -20 ... 120 dBuV RF input level
//return =  dBuV
uint16_t Radio_Get_Level(uint8_t fm)
{
	int16_t level;
	uint8_t status;
	
	if(Is_Radio_Atomic2||Is_Radio_Lithio)
	{
		if(1 == devTEF668x_Radio_Get_Quality_Level(fm,&status,&level))
		{
			return level;
		}
	}

	return -255;
}

// status =  0.1  32 ms
// level = -20 ... 120 dBuV
// usn = 0  100%
// wam = 0  100%
//offset = -1200  1200 (*0.1 kHz) = -120 kHz � 120 kHz
//bandwidth = FM, 560  3110 [*0.1 kHz]  AM 30 � 80 [*0.1 kHz]
// modulation = 0  100%
static int Radio_Get_Data(uint8_t fm, uint8_t *usn, uint8_t *wam, uint16_t *offset)
{
	if(Is_Radio_Atomic2||Is_Radio_Lithio)
	{
		if(1 == devTEF668x_Radio_Get_Quality_Data (fm,usn,wam,offset))
		{
			return 1;
		}
	}
	
	return 0;
}

uint16_t Radio_Get_RDS_Data(uint32_t*rds_data)
{
	if(Is_Radio_Atomic2||Is_Radio_Lithio)
	{
		uint16_t status;
		if(1 == devTEF668x_Radio_Get_RDS_DataRaw(1,&status,rds_data))
			return 1;
		
	}

	return !1;
}
/*--------------------------------------------------------------------
 Function:Radio_CheckStation
 Input: 
      Null
 OutPut:
      Null
 Desp:
     check station if aviable
---------------------------------------------------------------------*/
#define RADIO_CHECK_INTERVAL	5
static uint32_t Radio_Check_Timer;
void Radio_CheckStation(void)
{
	unsigned char threshold;

	uint8_t usn, wam;
	uint16_t offset;
	
	uint8_t fm = (Radio_CurrentBand<=FM3_BAND);
	switch(CheckIfStep)
	{
		case 10://start check init
			CheckIfStep = 20;
			break;

		case 20://Check QRS(quality of read status)	
            delay(fm ? RADIO_FM_LEVEL_AVAILABLE_TIME : RADIO_AM_LEVEL_AVAILABLE_TIME);        
			CheckIfStep=30;  //Set to next step
			break;   

		case 30://check level , AM=3 times,FM=2 times
		case 31:
		case 32:
			threshold = fm ? ((SeekSenLevel ==HIGH) ? FM_SCAN_LEVEL_HI : FM_SCAN_LEVEL) 
						: ((SeekSenLevel ==HIGH) ? AM_SCAN_LEVEL_HI : AM_SCAN_LEVEL);

			if(Radio_Get_Level(fm) < threshold)
			{//exit check
				CheckIfStep=NO_STATION;
			}
			else{
				threshold = fm ? 31 : 32;
				if(++CheckIfStep > threshold)
				{
					CheckIfStep=40;
					delay(40);//set for usn... available
				}
				else
					delay(RADIO_CHECK_INTERVAL);//set for the next time
			}
			break;
			
		case 40:
			CheckIfStep = NO_STATION;

			if(1 == Radio_Get_Data(fm,&usn,&wam,&offset))
			{
				if( fm? ((usn<FM_USN_DISTURBANCE)&&(wam < FM_WAM_DISTURBANCE)&&(offset < FM_FREQ_OFFSET))
									: (offset < AM_FREQ_OFFSET))

				CheckIfStep = PRESENT_STATION ;
			}
			break;
			
        case NO_STATION:
		break; 
		
        case PRESENT_STATION:
		break; 
		
        default:
		CheckIfStep = NO_STATION;
		break;        
	}
}
/*--------------------------------------------------------------------
 Function:Radio_CheckStationStatus
 Input: 
      Null
 OutPut:
      CheckIfStep
 Desp:
     get check station step
---------------------------------------------------------------------*/
uint8_t Radio_CheckStationStatus(void)
{
	return CheckIfStep;
}

/*--------------------------------------------------------------------
 Function:Radio_CheckStationInit
 Input: 
      Null
 OutPut:
      NULL
 Desp:
      check station init
---------------------------------------------------------------------*/
void Radio_CheckStationInit(void)
{
	CheckIfStep=10;
}
/*-----------------------------------------------------------------------
Function name:	Radio_SelectPreset
Input:		Station :1~6	
Output:		
Description:	 select preset station
------------------------------------------------------------------------*/
void Radio_SelectPreset(uint8_t Station)
{
	if(StationRecord[Radio_CurrentBand].Freq[Station]==0) 
	{		//no station record
		return;
	}
	/*set radio freqency*/	
	Radio_SetFreq(Radio_PRESETMODE,Radio_CurrentBand,StationRecord[Radio_CurrentBand].Freq[Station]);
	/*reset station*/
	StationRecord[Radio_CurrentBand].Freq[0]=StationRecord[Radio_CurrentBand].Freq[Station];
	Radio_CurrentStation=Station;
	Radio_CurrentFreq=StationRecord[Radio_CurrentBand].Freq[0];
}
/*-----------------------------------------------------------------------
Function name:	Radio_SaveCurrentFreq2Preset
Input:			
Output:		
Description:	preset station save
------------------------------------------------------------------------*/
void Radio_SaveCurrentFreq2Preset(uint8_t Station)
{
	if(StationRecord[Radio_CurrentBand].Freq[Station]==Radio_CurrentFreq)
	{
		return;
	}
	StationRecord[Radio_CurrentBand].Freq[Station]=StationRecord[Radio_CurrentBand].Freq[0];
	Radio_CurrentStation=Station;
	Radio_CurrentFreq=StationRecord[Radio_CurrentBand].Freq[0];
	/*store  station*/
	//Radio_StoreStationRecord(Radio_CurrentBand);
	Radio_StoreStation(Radio_CurrentBand,Radio_CurrentStation);
}
/*-----------------------------------------------------------------------
Function name:	
Input:			
Output:			
Description:	 
------------------------------------------------------------------------*/
void Radio_Para_Init(void)
{
	uint8_t Check_eeprom;
/*
	EEPROM_Read(EEPRom_CheckAddr,&Check_eeprom,1);
	//not store the config in eeprom
	if(Check_eeprom!=AvailableDataFlag)	
	{       
		//load default parameters
		memcpy(StationRecord,StationDefaultRecord,sizeof(StationRecord));

		
		//store config to eeprom
		Radio_StoreAll();
	}
	else
	{	    //load station from eeprom
     	Radio_ReadStationRecord();		
	} 
  */
	  /*init band*/
	Radio_CurrentStation=1;
	Radio_CurrentBand=FM1_BAND;
	Radio_CurrentFreq=10310;//StationRecord[Radio_CurrentBand].Freq[Radio_CurrentStation];

  Serial.print("Frequency: ");
  Serial.println(Radio_CurrentFreq);
}

/*====================================================
 Function:Radio_Save_Station
 Input: 
      Null
 OutPut:
      Null
 Desp:
     Save one station to mem
=========================================================*/
void Radio_Save_Station(uint8_t StationNumber, uint16_t StationFreq)
{	
	StationRecord[Radio_CurrentBand].Freq[StationNumber]=StationFreq;

	//EEPROM_Write(EEPRom_StationRecordDefaultAddr + Radio_CurrentBand *sizeof(StationRecord[0]) ,
	//	(uint8_t *)(&StationRecord[Radio_CurrentBand].Freq[StationNumber]),
	//	2);

}
