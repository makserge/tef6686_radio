/***********************************************************************//**
 * @file		tuner_proc.h
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
#ifndef 	__TUNER_H
#define	__TUNER_H

/*Scan play time*/
#define TUNER_SCAN_PLAY_TIME   5000

/*Fm stereo check time*/
#define FM_CHECK_STEREO_TIME   100

/*Key Define*/
#define Radio_ConfirmKey	SK1
//#define K_TUNE_UP		SK5//TUNE_UP 
//#define K_TUNE_DN		SK10//TUNE_DOWN

//#define  K_SEEK_UP          LK5
//#define  K_SEEK_DN          LK10
//#define  K_SCAN 			SK9  //scan key
//#define  K_BAND 			SK4  //band key
//#define  K_AS                    LK9   //auto store
//#define K_RDS    SK11
//#define K_SM1  SK17
//#define K_SM2  SK16
//#define K_SM3  SK15
//#define K_SM4  SK14
//#define K_SM5  SK13
//#define K_SM6  SK12

//#define K_LM1  LK17
//#define K_LM2  LK16
//#define K_LM3  LK15
//#define K_LM4  LK14
//#define K_LM5  LK13
//#define K_LM6  LK12


#define LOW  0;
#define HIGH 1
/*
typedef struct Bit_Char{
	unsigned char B0:1;
	unsigned char B1:1;
	unsigned char B2:1;
	unsigned char B3:1;
	unsigned char B4:1;
	unsigned char B5:1;
	unsigned char B6:1;
	unsigned char B7:1;
}BitChar;

typedef union  byte_format{
	unsigned char	data_byte;
	BitChar	data_Bbit;
} byte_field;
*/   
extern unsigned int	TunerKeyCode;   //tuner key
extern unsigned int	TunerWorkMode;  //tuner work mode
extern unsigned char  	TunerSubMode;//as loop  tuning step
extern uint8_t SeekSensLevel;
//extern byte_field TunerFlag1;
//#define F_Tuner_Mute		TunerFlag1.data_Bbit.B0  
//#define F_Tune_UP		TunerFlag1.data_Bbit.B1
//#define F_Stereo_Disp		TunerFlag1.data_Bbit.B2
//#define F_Band_Edge		TunerFlag1.data_Bbit.B3
//#define F_Find_Station	TunerFlag1.data_Bbit.B4
//#define F_Store_Station_Ok	TunerFlag1.data_Bbit.B5

extern uint8_t Radio_ScanStepCount;//scan seak step count;


/*function */

extern void Tuner_Para_Init(void);
extern void Tuner_Update_ProgCnt(uint8_t mode);

extern void Tuner_Para_Align_Mode_Proc(void);
void Tuner_Process(void);
uint16_t Tuner_Power_on(void); 
void Tuner_Turn_ON(void);

#endif
