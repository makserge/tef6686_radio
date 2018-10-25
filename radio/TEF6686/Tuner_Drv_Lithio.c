

#include "public.h"
#include <stdarg.h>

/* -----------------------------------------------------------------------------
 * Internal define:
 *-----------------------------------------------------------------------------*/
 
#define High_16bto8b(a)	((uint8_t)((a) >> 8))
#define Low_16bto8b(a) 	((uint8_t)(a )) 

#define Convert8bto16b(a)	((uint16_t)(((uint16_t)(*(a))) << 8 |((uint16_t)(*(a+1)))))

#define TAB_NUM(tab)	(sizeof(tab)/sizeof(tab[0]))

/* -----------------------------------------------------------------------------
 * Internal Prototypes:
 *-----------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------------
* Exported functions:
* -----------------------------------------------------------------------------
*/

/****************************************************************/
/* Function Calls for Control Commands and Information Requests */
/****************************************************************/
typedef struct{
	TEF668x_MODULE module;
	U8 cmd;
	int len;	//buffer size of all data
}TEF668x_CMD_LEN;

#define TEF668x_CMD_LEN_MAX	20

int devTEF668x_Set_Cmd(TEF668x_MODULE module, U8 cmd, int len,...)
{
	int i;
	U8 buf[TEF668x_CMD_LEN_MAX];
	U16 temp;
    	va_list     vArgs;

    	va_start(vArgs, len);
		
	buf[0]= module;			//module,		FM/AM/APP
	buf[1]= cmd;		//cmd,		1,2,10,... 
	buf[2]= 1;	//index, 		always 1

//fill buffer with 16bits one by one
	for(i=3;i<len;i++)
	{
		temp = va_arg(vArgs,int);	//the size only int valid for compile
		
		buf[i++]=High_16bto8b(temp);		
		buf[i]=Low_16bto8b(temp);		
	}
	
	va_end(vArgs);
	
	return Tuner_WriteBuffer(buf, len);
}


static int devTEF668x_Get_Cmd(TEF668x_MODULE module, U8 cmd, U8 *receive,int len)
{
	U8 buf[3];

	buf[0]= module;			//module,		FM/AM/APP
	buf[1]= cmd;		//cmd,		1,2,10,... 
	buf[2]= 1;	//index, 		always 1

	Tuner_WriteBuffer(buf, 3);

	return Tuner_ReadBuffer(receive,len);
}

/*
module 32 / 33 FM / AM
cmd 1 Tune_To mode, frequency

index 
1 mode
	[ 15:0 ]
	tuning actions
	0 = no action (radio mode does not change as function of module band)
	1 = Preset Tune to new program with short mute time
	2 = Search Tune to new program and stay muted
	FM 3 = AF-Update Tune to alternative frequency, store quality
	and tune back with inaudible mute
	4 = Jump Tune to alternative frequency with short
	inaudible mute
	5 = Check Tune to alternative frequency and stay
	muted
	AM 3 … 5 = reserved
	6 = reserved
	7 = End Release the mute of a Search or Check action
	(frequency is not required and ignored)
2 frequency
[ 15:0 ]
	tuning frequency
	FM 6500 … 10800 65.00 … 108.00 MHz / 10 kHz step size
	AM LW 144 … 288 144 … 288 kHz / 1 kHz step size
	MW 522 … 1710 522 … 1710 kHz / 1 kHz step size
	SW 2300 … 27000 2.3 … 27 MHz / 1 kHz step size
*/
int devTEF668x_Radio_Tune_To (bool fm,U16 mode,U16 frequency )
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Tune_To, 
			(mode<=5)? 7 : 5,
			mode, frequency);
}

/*
module 32 FM
cmd 2 Set_Tune_Options afu_bw_mode, afu_bandwidth, afu_mute_time, afu_sample_time

index 
1 afu_bw_mode
	[ 15:0 ]
	IF bandwidth control mode during AF_Update
	0 = fixed (default)
2 afu_bandwidth
	[ 15:0 ]
	fixed IF bandwidth during AF_Update
	560 … 3110 [*0.1 kHz] = IF bandwidth 56 … 311 kHz; narrow … wide
	2360 = 236 kHz (default)
3 afu_mute_time
	[ 15:0 ]
	AF_update inaudible mute slope time
	250 … 1000 [* 1 us] = 0.25 … 1 ms
	1000 = 1 ms (default)
4 afu_sample_time
	[ 15:0 ]
	AF_update sampling time
	1000 … 20000 [* 1 us] = 1 … 20 ms
	2000 = 2 ms (default)
*/
int devTEF668x_Radio_Set_Tune_Options(bool fm,U16 afu_bw_mode,U16 afu_bandwidth,U16 afu_mute_time,U16 afu_sample_time)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Set_Tune_Options, 
			11,
			afu_bw_mode, afu_bandwidth,afu_mute_time,afu_sample_time);
}

/*
module 32 / 33 FM / AM
cmd 10 Set_Bandwidth FM : mode, bandwidth, control_sensitivity, low_level_sensitivity
AM : mode, bandwidth

index 
1 mode
	[ 15:0 ]
	IF bandwidth control mode
	FM 0 = fixed
	1 = automatic (default)
	AM 0 = fixed (default)
2 bandwidth
	[ 15:0 ]
	fixed IF bandwidth
	FM 560 … 3110 [*0.1 kHz] = IF bandwidth 56 … 311 kHz; narrow … wide
	2360 = 236 kHz (default)
	AM 30 … 80 [*0.1 kHz] = IF bandwidth 3 … 8 kHz; narrow … wide
	40 = 4.0 kHz (default)
3 control_sensitivity
	[ 15:0 ]
	FM automatic IF bandwidth control sensitivity
	500 … 1500 [*0.1 %] = 50 … 150 % relative adjacent channel sensitivity
	1000 = 100 % (default)
4 low_level_sensitivity
	[ 15:0 ]
	FM automatic IF bandwidth control sensitivity for low level conditions
	500 … 1500 [*0.1 %] = 50 … 150 % relative adjacent channel sensitivity
	1000 = 100 % (default)
*/
int devTEF668x_Radio_Set_Bandwidth(bool fm,U16 mode,U16 bandwidth,U16 control_sensitivity,U16 low_level_sensitivity)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_Bandwidth,
			fm? 11: 7,
			mode,bandwidth,control_sensitivity,low_level_sensitivity);
}

/*
module 32 / 33 FM / AM
cmd 11 Set_RFAGC start, extension

index
1 start
	[ 15:0 ]
	RF AGC start
	FM 830 ¡K 920 [*0.1 dBƒÝV) = 83 ¡K 92 dBƒÝV
	920 = 92 dBƒÝV (default)
	AM 940 ¡K 1030 (*0.1 dBƒÝV) = 94 ¡K 103 dBƒÝV
	1000 = 100 dBƒÝV (default)
2 extension
	[ 15:0 ]
	RF AGC step extension
	FM 0 = integrated steps only (default)
	1 = AGC step extension from control output (GPIO feature ¡¥AGC¡¦)
	AM reserved
*/
int devTEF668x_Radio_Set_RFAGC(bool fm,U16 start,U16 extension)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_RFAGC, 
			fm? 7: 5,
			start,extension);
}

/*
module 33 AM
cmd 12 Set_Antenna attenuation

1 attenuation
	[ 15:0 ]
	LNA gain reduction
	0 / 60 / 120 / 180 / 240 / 300 / 360 (*0.1 dB) = 0 … 36 dB antenna
	attenuation (6 dB step size)
	0 = no attenuation (default)
*/
int devTEF668x_Radio_Set_Antenna(bool fm,U16 attenuation)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_Antenna, 
			5,
			attenuation);
}

/*
module 32 FM
cmd 20 Set_MphSuppression mode

index 
1 mode
	[ 15:0 ]
	FM multipath suppression
	0 = off (default)
	1 = on
*/
int devTEF668x_Radio_Set_MphSuppression(bool fm,U16 mode)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Set_MphSuppression,
			5,
			mode);
}
/*
FM cmd 22 Set_ChannelEqualizer
Optional use of the FM channel equalizer. 
module   32  FM
cmd  22  Set_ChannelEqualizer  mode
index  1  mode
[ 15:0 ]
FM channel equalizer
0 = off (default)
1 = on
*/
int devTEF668x_Radio_Set_ChannelEqualizer(bool fm,U16 mode)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF668X_Cmd_Set_ChannelEqualizer,
			5,
			mode);
}
/*
module    32  FM 
cmd  32  Set_StereoImprovement  mode
index  1  mode
[ 15:0 ]
FM stereo extended weak signal handling
0 = stereo high blend (default)
1 = FMSI stereo band blend system
*/
int devTEF668x_Radio_Set_StereoImprovement(bool fm,U16 mode)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF668X_Cmd_Set_StereoImprovement,
			5,
			mode);
}
/*
cmd  70  Set_StHiBlend_Time  slow_attack, slow_decay, fast_attack, fast_decay
index  1  slow_attack 
[ 15:0 ]
slow attack time of weak signal handling
60 ¡­ 2000 (ms) = 60 ms ¡­ 2 s slow attack time
500 = 500 ms (default)
2  slow_decay 
[ 15:0 ]
slow decay time of weak signal handling
120 ¡­ 12500 (ms) = 120 ms ¡­ 12.5 s slow attack time
2000 = 2 s (default)
3  fast_attack 
[ 15:0 ]
fast attack time of weak signal handling
10 ¡­ 1200 (*0.1 ms) = 1 ms ¡­ 120 ms fast attack time
20 = 2 ms (default)
4  fast_decay 
[ 15:0 ]
fast decay time of weak signal handling
20 ¡­ 5000 ( *0.1 ms) = 2 ms ¡­ 500 ms fast attack time
20 = 2 ms (default)

*/
int devTEF668x_Radio_Set_StHiBlend_Time(bool fm,U16 slow_attack,U16 slow_decay,U16 fast_attack,U16 fast_decay)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF668X_Cmd_Set_StHiBlend_Time,
			11,
			slow_attack,slow_decay,fast_attack,fast_decay);
}
/*
module 32 / 33 FM / AM
cmd 23 Set_NoiseBlanker mode, sensitivity

index 
1 mode
	[ 15:0 ]
	noise blanker
	0 = off
	1 = on (default)
2 sensitivity
	[ 15:0 ]
	trigger sensitivity
	500 … 1500 [*0.1 %] = 50 … 150 % relative trigger sensitivity
	1000 = 100 % (default)
*/
int devTEF668x_Radio_Set_NoiseBlanker(bool fm,U16 mode,U16 sensitivity)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_NoiseBlanker, 
			7,
			mode,sensitivity);
}
/*
module 33 AM
cmd 24 Set_NoiseBlanker_Audio mode, sensitivity

index 
1 mode
	[ 15:0 ]
	AM audio noise blanker (audio frequency detection)
	0 = off
	1 = on (default)
2 sensitivity
	[ 15:0 ]
	AM audio noise blanker trigger sensitivity
	500 … 1500 [*0.1 %] = 50 … 150 % relative trigger sensitivity
	1000 = 100 % (default)
*/
int devTEF668x_Radio_Set_NoiseBlanker_Audio(bool fm,U16 mode,U16 sensitivity)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_NoiseBlanker_Audio, 
			7,
			mode,sensitivity);
}
/*
module 32 / 33 FM / AM
cmd 30 Set_DigitalRadio mode

index 
1 mode
	[ 15:0 ]
	digital radio
	0 = off (default)
	1 = on
*/
int devTEF668x_Radio_Set_DigitalRadio(bool fm,U16 mode)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_DigitalRadio, 
			5,
			mode);
}
/*
module 32 FM
cmd 31 Set_Deemphasis timeconstant

index 
1 timeconstant
	[ 15:0 ]
	deemphasis time constant
	0 = off; for evaluation purposes only
	500 = 50 ƒÝs deemphasis (default)
	750 = 75 ƒÝs deemphasis
*/
int devTEF668x_Radio_Set_Deemphasis(bool fm,U16 timeconstant)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Set_Deemphasis, 
			5,
			timeconstant);
}

/*
module 32 / 33 FM / AM
cmd 38 Set_LevelStep step1, step2, step3, step4, step5, step6, step7

index 1 step1
	[ 15:0 ]
	level offset for an AGC step from 0 to 1
	-60 … 0 (*0.1 dB) = -6 … 0 dB
	-20 = -2 dB (FM default) / -10 = -1 dB (AM default)
2 step2
	[ 15:0 ]
	level offset for an AGC step from 1 to 2
	-60 … 0 (*0.1 dB) = -6 … 0 dB
	-30 = -3 dB (FM default) / -20 = -2 dB (AM default)
3 step3
	[ 15:0 ]
	level offset for an AGC step from 2 to 3
	-60 … 0 (*0.1 dB) = -6 … 0 dB
	-40 = -4 dB (FM default) / -30 = -3 dB (AM default)
4 step4
	[ 15:0 ]
	level offset for an AGC step from 3 to 4
	-60 … 0 (*0.1 dB) = -6 … 0 dB
	-50 = -5 dB (FM default) / -40 = -4 dB (AM default)
5 step5
	[ 15:0 ]
	level offset for an AGC step from 4 to 5
	-60 … 0 (*0.1 dB) = -6 … 0 dB
	-60 = -6 dB (FM default) / -50 = -5 dB (AM default)
6 step6
	[ 15:0 ]
	level offset for an AGC step from 5 to 6
	-60 … 0 (*0.1 dB) = -6 … 0 dB
	-60 = -6 dB (default)
7 step7
	[ 15:0 ]
	level offset for an AGC step from 6 to 7 (or higher)
	-60 … 0 (*0.1 dB) = -6 … 0 dB
	-60 = -6 dB (default)
*/
int devTEF668x_Radio_Set_LevelStep(bool fm,int step1,int step2,int step3,int step4,int step5,int step6,int step7)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_LevelStep, 
			17,
			(U16)step1,(U16)step2,(U16)step3,(U16)step4,(U16)step5,(U16)step6,(U16)step7);
}

/*
module 32 / 33 FM / AM
cmd 39 Set_LevelOffset offset

1 offset
	[ 15:0 ] signed
	level offset
	-480 … +150 (*0.1 dB) = -48 … +15 dB
	0 = 0 dB (default)
*/
int devTEF668x_Radio_Set_LevelOffset(bool fm,int offset)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_LevelOffset, 
			5,
			(U16)offset);
}

/*
module 32 / 33 FM / AM
cmd 40 Set_Softmute_Time slow_attack, slow_decay, fast_attack, fast_decay

1 slow_attack
	[ 15:0 ]
	slow attack time of weak signal handling
	60 … 2000 (ms) = 60 ms … 2 s slow attack time
	120 = 120 ms (default)
2 slow_decay
	[ 15:0 ]
	slow decay time of weak signal handling
	120 … 12500 (ms) = 120 ms … 12.5 s slow attack time
	500 = 500 ms (default)
3 fast_attack
	[ 15:0 ]
	fast attack time of weak signal handling
	10 … 1200 (*0.1 ms) = 1 ms … 120 ms fast attack time
	20 = 2 ms (FM default) / 120 = 12 ms (AM default)
4 fast_decay
	[ 15:0 ]
	fast decay time of weak signal handling
	20 … 5000 ( *0.1 ms) = 2 ms … 500 ms fast attack time
	20 = 2 ms (FM default) / 500 = 50 ms (AM default)
*/
int devTEF668x_Radio_Set_Softmute_Time(bool fm,U16 slow_attack,U16 slow_decay,U16 fast_attack,U16 fast_decay)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_Softmute_Time, 
			11,
			slow_attack,slow_decay,fast_attack,fast_decay);
}

/*
module 32 / 33 FM / AM
cmd 42 Set_Softmute_Level mode, start, slope

1 mode
	[ 15:0 ]
	timer selection
	0 = off (only for evaluation)
	1 = fast timer control
	2 = slow timer control (default)
	3 = dual timer control; combined fast and slow timer control
2 start 
	[ 15:0 ] 0 ¡K 500 [*0.1 dBƒÝV] = control when level falls below 0 dBƒÝV ¡K 50 dBƒÝV
	150 = 15 dBƒÝV (FM default) / 280 = 28 dBƒÝV (AM default)
3 slope
	60 ¡K 300 [*0.1 dB] = control over level range of 6 dB ¡K 30 dB
	220 = 22 dB (default) / 250 = 25 dB (default)
*/
int devTEF668x_Radio_Set_Softmute_Level(bool fm,U16 mode,U16 start,U16 slope)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_Softmute_Level, 
			9,
			mode,start,slope);
}

/*
module 32 FM
cmd 43 Set_Softmute_Noise mode, start, slope

1 mode
	[ 15:0 ]
	timer selection
	0 = off (default)
	1 = fast timer control
	2 = slow timer control
	3 = dual timer control; combined fast and slow timer control
2 start
	[ 15:0 ]
	FM weak signal handling noise start
	0 … 800 [*0.1 %] = control when noise above 0… 80% of USN detector
	500 = 50% (default)
3 slope
	[ 15:0 ]
	FM weak signal handling noise range
	100 … 1000 [*0.1 %] = control over range of 10… 100% of USN detector
	1000 = 100% (default)
*/
int devTEF668x_Radio_Set_Softmute_Noise(bool fm,U16 mode,U16 start,U16 slope)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Set_Softmute_Noise, 
			9,
			mode,start,slope);
}

/*
module 32 FM
cmd 44 Set_Softmute_Mph mode, start, slope

1 mode
	[ 15:0 ]
	timer selection
	0 = off (default)
	1 = fast timer control
	2 = slow timer control
	3 = dual timer control; combined fast and slow timer control
2 start
	[ 15:0 ]
	FM weak signal handling multipath start
	0 … 800 [*0.1 %] = control when mph above 0… 80% of WAM detector
	500 = 50% (default)
3 slope
	[ 15:0 ]
	FM weak signal handling multipath range
	100 … 1000 [*0.1 %] = control over range of 10… 100% of WAM detector
	1000 = 100% (default)
*/
int devTEF668x_Radio_Set_Softmute_Mph(bool fm,U16 mode,U16 start,U16 slope)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Set_Softmute_Mph,
			9,
			mode,start,slope);
}

/*
module 32 / 33 FM / AM
cmd 45 Set_Softmute_Max mode, limit

index
1 mode
	[ 15:0 ]
	weak signal handling (dynamic control)
	0 = off (for evaluation only)
	1 = on; maximum dynamic control defined by limit parameter (default)
2 limit
	[ 15:0 ]
	softmute dynamic attenuation limit
	0 … 400 [*0.1 dB] = 0 … 40 dB softmute maximum attenuation
	200 = 20 dB (FM default) / 250 = 25 dB (AM default)

*/
int devTEF668x_Radio_Set_Softmute_Max(bool fm,U16 mode,U16 limit)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_Softmute_Max,
			7,
			mode,limit);
}

/*
module 32 / 33 FM / AM
cmd 50 Set_Highcut_Time slow_attack, slow_decay, fast_attack, fast_decay

index 
1 slow_attack
	[ 15:0 ]
	slow attack time of weak signal handling
	60 … 2000 (ms) = 60 ms … 2 s slow attack time
	500 = 500 ms (default)
2 slow_decay
	[ 15:0 ]
	slow decay time of weak signal handling
	120 … 12500 (ms) = 120 ms … 12.5 s slow attack time
	2000 = 2 s (default)
3 fast_attack
	[ 15:0 ]
	fast attack time of weak signal handling
	10 … 1200 (*0.1 ms) = 1 ms … 120 ms fast attack time
	20 = 2 ms (FM default) / 120 = 12 ms (AM default)
4 fast_decay
	[ 15:0 ]
	fast decay time of weak signal handling
	20 … 5000 ( *0.1 ms) = 2 ms … 500 ms fast attack time
	20 = 2 ms (FM default) / 500 = 50 ms (AM default)

*/
int devTEF668x_Radio_Set_Highcut_Time(bool fm,U16 slow_attack,U16 slow_decay,U16 fast_attack,U16 fast_decay)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_Highcut_Time,
			11,
			slow_attack,slow_decay,fast_attack,fast_decay);
}

/*
module 32 / 33 FM / AM
cmd 51 Set_Highcut_Mod mode, start, slope, shift

index 
1 mode modulation dependent weak signal handling
	0 = off (default)
	1 = on (independent modulation timer)
2 start
	[ 15:0 ]
	weak signal handling modulation start
	100 … 1000 [*0.1 %] = control when modulation falls below 10% … 100%
	250 = 25% (default)
	( note : for FM band 100% modulation equals 75 kHz deviation )
3 slope
	[ 15:0 ]
	weak signal handling modulation range
	30 … 1000 (*0.1 %) = control over modulation range of 3% … 100%
	130 = 13% (default)
4 shift
	[ 15:0 ]
	weak signal handling control shift
	50 … 1000 (*0.1 %) = maximum weak signal control shift of 5% … 100%
	500 = 50% (default)
	(percentage of the linear control range from _Min limit to _Max limit)

*/
int devTEF668x_Radio_Set_Highcut_Mod(bool fm,U16 mode,U16 start,U16 slope,U16 shift)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_Highcut_Mod,
			11,
			mode,start,slope,shift);
}

/*
module 32 / 33 FM / AM
cmd 52 Set_Highcut_Level mode, start, slope

index
1 mode
	[ 15:0 ]
	timer selection
	0 = off (only for evaluation)
	1 = fast timer control
	2 = slow timer control (AM default)
	3 = dual timer control; combined fast and slow timer control (FM default)
2 start
	[ 15:0 ]
	weak signal handling level start
	200 ¡K 600 [*0.1 dBƒÝV] = control when level is below 20 dBƒÝV ¡K 60 dBƒÝV
	360 = 36 dBƒÝV (FM default) / 400 = 40 dBƒÝV (AM default)
3 slope
	[ 15:0 ]
	weak signal handling level range
	60 ¡K 300 [*0.1 dB] = control over level range of 6 dB ¡K 30 dB
	300 = 30 dB (FM default) / 200 = 20 dB (AM default)

*/
int devTEF668x_Radio_Set_Highcut_Level(bool fm,U16 mode,U16 start,U16 slope)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_Highcut_Level,
			9,
			mode,start,slope);
}


/*
module 32 FM
cmd 53 Set_Highcut_Noise mode, start, slope

index 
1 mode
	[ 15:0 ]
	timer selection
	0 = off
	1 = fast timer control
	2 = slow timer control (default)
	3 = dual timer control; combined fast and slow timer control
2 start
	[ 15:0 ]
	FM weak signal handling noise start
	0 … 800 [*0.1 %] = control when noise above 0… 80% of USN detector
	360 = 36% (default)
3 slope
	[ 15:0 ]
	FM weak signal handling noise range
	100 … 1000 [*0.1 %] = control over range of 10… 100% of USN detector
	300 = 30% (default)

*/
int devTEF668x_Radio_Set_Highcut_Noise(bool fm,U16 mode,U16 start,U16 slope)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Set_Highcut_Noise,
			9,
			mode,start,slope);
}


/*
module 32 FM
cmd 54 Set_Highcut_Mph mode, start, slope

index 
1 mode
	[ 15:0 ]
	timer selection
	0 = off (only for evaluation)
	1 = fast timer control
	2 = slow timer control (default)
	3 = dual timer control; combined fast and slow timer control
2 start
	[ 15:0 ]
	FM weak signal handling multipath start
	0 … 800 [*0.1 %] = control when mph above 0… 80% of WAM detector
	360 = 36% (default)
3 slope
	[ 15:0 ]
	FM weak signal handling multipath range
	100 … 1000 [*0.1 %] = control over range of 10… 100% of WAM detector
	300 = 30% (default)

*/
int devTEF668x_Radio_Set_Highcut_Mph(bool fm,U16 mode,U16 start,U16 slope)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Set_Highcut_Mph,
			9,
			mode,start,slope);
}

/*
module 32 / 33 FM / AM
cmd 55 Set_Highcut_Max mode, limit

index 
1 mode
	[ 15:0 ]
	weak signal handling (dynamic control)
	0 = off; for evaluation only
	1 = on; maximum dynamic control set by limit parameter (default)
2 limit
	[ 15:0 ]
	Highcut attenuation limit
	FM 1500 … 7000 [*1 Hz] = 1.5 … 7 kHz Highcut maximum -3 dB att.
	4000 = 4 kHz (default)
	AM 1350 … 7000 [*1 Hz] = 1.35 … 7 kHz Highcut maximum -3 dB att.
	1800 = 1.8 kHz (default)

*/
int devTEF668x_Radio_Set_Highcut_Max(bool fm,U16 mode,U16 limit)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_Highcut_Max,
			7,
			mode,limit);
}


/*
module 32 / 33 FM / AM
cmd 56 Set_Highcut_Min mode, limit

index
1 mode
	[ 15:0 ]
	strong signal handling
	0 = off; high audio frequency bandwidth is not limited (FM default)
	1 = on; minimum control limit set by limit parameter (AM default)
2 limit
	[ 15:0 ]
	Highcut fixed attenuation limit
	2700 … 15000 [*1 Hz] = 2.7 … 15 kHz -3 dB attenuation fo

*/
int devTEF668x_Radio_Set_Highcut_Min(bool fm,U16 mode,U16 limit)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_Highcut_Min,
			7,
			mode,limit);
}

/*
module 32 / 33 FM / AM
cmd 58 Set_Lowcut_Min mode, limit

index 
1 mode
	[ 15:0 ]
	strong signal handling
	0 = off; low audio frequency bandwidth is not limited (FM default)
	1 = on; minimum control limit set by limit parameter (AM default)
2 limit
	[ 15:0 ]
	Lowcut fixed attenuation limit
	10 … 200 [Hz] = 10 … 200 Hz Lowcut minimum -3 dB attenuation
	20 = 20 Hz (default)

*/
int devTEF668x_Radio_Set_Lowcut_Min(bool fm,U16 mode,U16 limit)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_Lowcut_Min,
			7,
			mode,limit);
}

/*
module 32 FM
cmd 60 Set_Stereo_Time slow_attack, slow_decay, fast_attack, fast_decay

index 
1 slow_attack
	[ 15:0 ]
	slow attack time of weak signal handling
	60 … 2000 (ms) = 60 ms … 2 s slow attack time
	1000 = 1 s (default)
2 slow_decay
	[ 15:0 ]
	slow decay time of weak signal handling
	120 … 12500 (ms) = 120 ms … 12.5 s slow attack time
	4000 = 4 s (default)
3 fast_attack
	[ 15:0 ]
	fast attack time of weak signal handling
	10 … 1200 (*0.1 ms) = 1 ms … 120 ms fast attack time
	80 = 8 ms
4 fast_decay
	[ 15:0 ]
	fast decay time of weak signal handling
	20 … 5000 ( *0.1 ms) = 2 ms … 500 ms fast attack time
	80 = 8 ms

*/
int devTEF668x_Radio_Set_Stereo_Time(bool fm,U16 slow_attack,U16 slow_decay,U16 fast_attack,U16 fast_decay)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Set_Stereo_Time,
			11,
			slow_attack,slow_decay,fast_attack,fast_decay);
}

/*
module 32 FM
cmd 61 Set_Stereo_Mod mode, start, slope, shift

index 
1 mode
	[ 15:0 ]
	modulation dependent weak signal handling
	0 = off (default)
	1 = on (independent modulation timer)
2 start
	[ 15:0 ]
	weak signal handling modulation start
	100 … 1000 [*0.1 %] = control when modulation falls below 10% … 100%
	210 = 21% (default)
	( note : for FM band 100% modulation equals 75 kHz deviation )
3 slope
	[ 15:0 ] 
	30 … 1000 (*0.1 %) = control over modulation range of 3% … 100%
	90 = 9% (default)
4 shift
	[ 15:0 ]
	weak signal handling control shift
	50 … 1000 (*0.1 %) = maximum weak signal control shift of 5% … 100%
	500 = 50% (default)
	(percentage of the linear control range from _Min limit to ‘mono’)

*/
int devTEF668x_Radio_Set_Stereo_Mod(bool fm,U16 mode,U16 start,U16 slope,U16 shift)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Set_Stereo_Mod,
			11,
			mode,start,slope,shift);
}

/*
module 32 FM
cmd 62 Set_Stereo_Level mode, start, slope

index 
1 mode
	[ 15:0 ]
	timer selection
	0 = off (only for evaluation)
	1 = fast timer control
	2 = slow timer control
	3 = dual timer control; combined fast and slow timer control (default)
2 start
	[ 15:0 ]
	weak signal handling level start
	300 ¡K 600 [*0.1 dBƒÝV] = control when level below 30 dBƒÝV ¡K 60 dBƒÝV
	460 = 46 dBƒÝV (default)
3 slope
	[ 15:0 ]
	weak signal handling level range
	60 ¡K 300 [*0.1 dB] = control over level range of 6 dB ¡K 30 dB
	240 = 24 dB (default)

*/
int devTEF668x_Radio_Set_Stereo_Level(bool fm,U16 mode,U16 start,U16 slope)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Set_Stereo_Level,
			9,
			mode,start,slope);
}

/*
module 32 FM
cmd 63 Set_Stereo_Noise mode, start, slope

index 
1 mode
	[ 15:0 ]
	timer selection
	0 = off
	1 = fast timer control
	2 = slow timer control
	3 = dual timer control; combined fast and slow timer control (default)
2 start
	[ 15:0 ]
	FM weak signal handling noise start
	0 … 800 [*0.1 %] = control when noise above 0… 80% of USN detector
	240 = 24% (default)
3 slope
	[ 15:0 ]
	FM weak signal handling noise range
	100 … 1000 [*0.1 %] = control over range of 10… 100% of USN detector
	200 = 20% (default)

*/
int devTEF668x_Radio_Set_Stereo_Noise(bool fm,U16 mode,U16 start,U16 slope)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Set_Stereo_Noise,
			9,
			mode,start,slope);
}

/*
module 32 FM
cmd 64 Set_Stereo_Mph mode, start, slope

index 
1 mode
	[ 15:0 ]
	timer selection
	0 = off
	1 = fast timer control
	2 = slow timer control
	3 = dual timer control; combined fast and slow timer control (default)
2 start 
	0 … 800 [*0.1 %] = control when mph above 0… 80% of WAM detector
	240 = 24% (default)
3 slope
	[ 15:0 ]
	FM weak signal handling multipath range
	100 … 1000 [*0.1 %] = control over range of 10… 100% of WAM detector
	200 = 20% (default)

*/
int devTEF668x_Radio_Set_Stereo_Mph(bool fm,U16 mode,U16 start,U16 slope)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Set_Stereo_Mph,
			9,
			mode,start,slope);
}

/*
module 32 FM
cmd 65 Set_Stereo_Max mode, limit

index
1 mode
	[ 15:0 ]
	weak signal handling (dynamic control)
	0 = off (for evaluation only)
	1 = on; maximum dynamic control is 0 dB channel sep, i.e. mono (default)

*/
int devTEF668x_Radio_Set_Stereo_Max(bool fm,U16 mode)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Set_Stereo_Max,
			5,
			mode);
}

/*
module 32 FM
cmd 66 Set_Stereo_Min mode, limit

index 
1 mode
	[ 15:0 ]
	strong signal handling
	0 = off; channel separation is not limited (default)
	1 = on; minimum control limit set by limit parameter
	2 = forced mono
2 limit
	[ 15:0 ]
	Stereo fixed attenuation limit
	60 … 400 [0.1* dB] = 6 … 40 dB Stereo minimum channel separation
	400 = 40 dB (default)

*/
int devTEF668x_Radio_Set_Stereo_Min(bool fm,U16 mode,U16 limit)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Set_Stereo_Min,
			7,
			mode,limit);
}

/*
module 32 / 33 FM / AM
cmd 80 Set_Scaler gain

index 
1 gain
	[ 15:0 ] (signed)
	channel gain
	-120 … +60 [*0.1 dB] = -12 … +6 dB analog radio signal gain
	0 = 0 dB (default)
*/
int devTEF668x_Radio_Set_Scaler(bool fm,U16 gain)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_Scaler,
			5,
			gain);
}

/*
module 32 FM
cmd 81 Set_RDS mode, restart, interface

index 
1 mode
	[ 15:0 ]
	RDS operation control
	0 = off (RDS function disabled)
	1 = decoder mode (default); output of RDS group data (block A, B, C, D)
	from Get_RDS_Status/Get_RDS_Data (FM cmd = 130/131)
	2 = demodulator mode; output of raw demodulator data from
	Get_RDS_Status/Get_RDS_Data; FM cmd = 130/131)
2 restart
	[ 15:0 ]
	RDS decoder restart
	0 = no control
	1 = manual restart; start looking for new RDS signal immediately
	2 = automatic restart after tuning (default); start looking for new RDS signal
	after Preset, Search, Jump or Check tuning action (see FM cmd = 1)
3 interface
	[ 15:0 ]
	RDS pin signal functionality
	0 = no pin interface (default)
	2 = data-available status output; active low (GPIO feature ‘DAVN’)
	4 = legacy 2-wire demodulator data and clock output (‘RDDA’ and ‘RDCL’)
*/
int devTEF668x_Radio_Set_RDS(bool fm,U16 mode,U16 restart,U16 interfac)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Set_RDS,
			9,
			mode,restart,interfac);
}

/*
module 32 / 33 FM / AM
cmd 82 Set_QualityStatus mode, interface

index 
1 mode
[ 15:0 ]
	quality status flag after tuning ready
	0 = no flag set after tuning (default)
	[ 8:0 ] : 1 … 320 (* 0.1 ms) = set flag at 0.1 … 32 ms after tuning ready
	[15] : 1 = set flag when FM AF_Update quality result is available
2 interface
	[ 15:0 ]
	quality status pin signal functionality
	0 = no pin interface (default)
	2 = quality status output; active low (‘QSI’)
*/
int devTEF668x_Radio_Set_QualityStatus(bool fm,U16 mode,U16 interfac)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_QualityStatus,
			7,
			mode,interfac);
}

/*
module 32 / 33 FM / AM
cmd 83 Set_DR_Blend mode, in_time, out_time, gain

index 
1 mode
	[ 15:0 ]
	blend pin use (DR_BL input)
	0 = Standard pin use : DR Blend pin High = digital radio (default)
	1 = Inverted pin use : DR Blend pin Low = digital radio
	2 = No pin use; Force blend to digital radio
	3 = No pin use; Force blend to analog radio
2 in_time
	[ 15:0 ]
	blend time from analog radio to digital radio
	10 … 5000 [*0.1 ms] = 1 … 500 ms
	50 = 5 ms (default)
3 out_time
	[ 15:0 ]
	blend time from digital radio to analog radio
	10 … 5000 [*0.1 ms] = 1 … 500 ms
	50 = 5 ms (default)
4 gain
	[ 15:0 ] (signed)
	digital radio channel gain
	-180 … +60 [*0.1 dB] = -18 … +6 dB digital radio signal gain
	0 = 0 dB (default)
*/
int devTEF668x_Radio_Set_DR_Blend(bool fm,U16 mode,U16 in_time,U16 out_time,U16 gain)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_DR_Blend,
			11,
			mode,in_time,out_time,gain);
}


/*
module 32 / 33 FM / AM
cmd 84 Set_DR_Options samplerate

index 
1 samplerate
[ 15:0 ]
	baseband digital radio sample rate (DR_I2S output)
	0 = automatic frequency selection based on tuning frequency (default)
	6500 = 650 kHz (not for normal application use)
	6750 = 675 kHz (not for normal application use)
*/
int devTEF668x_Radio_Set_DR_Options(bool fm,U16 samplerate)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_DR_Options,
			5,
			samplerate);
}

/*
module 32 / 33 FM / AM
cmd 85 Set_Specials ana_out, dig_out

index 
1 ana_out
	[ 15:0 ]
	analog output use
	0 = normal operation (default)
	1 = DAC_L : FM MPX wideband (DARC) signal / DAC_R : mono audio
	2 = DAC_L : digital radio left channel / DAC_R : analog radio left channel
*/
int devTEF668x_Radio_Set_Specials(bool fm,U16 ana_out)
{
	return devTEF668x_Set_Cmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Set_Specials,
			5,
			ana_out);
}

/*
module 48 AUDIO
cmd 10 Set_Volume volume

index 
1 volume
	[ 15:0 ] (signed)
	audio volume
	-599 … +240 = -60 … +24 dB volume
	0 = 0 dB (default)
*/
int devTEF668x_Audio_Set_Volume(int16_t volume)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_AUDIO,
			TEF665X_Cmd_Set_Volume,
			5,
			volume*10);
}

/*
module 48 AUDIO
cmd 11 Set_Mute mode

index 
1 mode
	[ 15:0 ]
	audio mute
	0 = mute disabled
	1 = mute active (default)
*/
int devTEF668x_Audio_Set_Mute(U16 mode)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_AUDIO,
			TEF665X_Cmd_Set_Mute, 
			5,
			mode);
}


/*
module 48 AUDIO
cmd 12 Set_Input source

index 
1 source
	[ 15:0 ]
	audio source select
	0 = radio (default)
	(analog radio or digital radio when enabled and available)
	32 = I²S digital audio input (IIS_SD_0)
	240 = sine wave generator
*/
int devTEF668x_Audio_Set_Input(U16 source)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_AUDIO,
			TEF665X_Cmd_Set_Input, 
			5,
			source);
}

/*
module 48 AUDIO
cmd 13 Set_Output_Source signal, source

index 
1 signal
	[ 15:0 ]
	audio output
	33 = I²S digital audio IIS_SD_1 (output)
	128 = DAC L/R output
2 source
	[ 15:0 ]
	source
	4 = analog radio
	32 = I²S digital audio IIS_SD0 (input)
	224 = audio processor (default)
	240 = sine wave generator
*/
int devTEF668x_Audio_Set_Output_Source(U16 signal,U16 source)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_AUDIO,
			TEF665X_Cmd_Set_Output_Source, 
			7,
			signal,source);
}

/*
module 48 AUDIO
cmd 21 Set_Ana_Out signal, mode

index 
1 signal
	[ 15:0 ]
	analog audio output
	128 = DAC L/R output
2 mode
	[ 15:0 ]
	output mode
	0 = off (power down)
	1 = output enabled (default)
*/
int devTEF668x_Audio_Set_Ana_Out(U16 signal,U16 mode)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_AUDIO,
			TEF665X_Cmd_Set_Ana_Out, 
			7,
			signal,mode);
}

/*
module 48 AUDIO
cmd 22 Set_Dig_IO signal, mode, format, operation, samplerate

index 
1 signal
[ 15:0 ]
	digital audio input / output
	32 = I²S digital audio IIS_SD_0 (input)
	33 = I²S digital audio IIS_SD_1 (output)
(2) mode
	0 = off (default)
	1 = input (only available for signal = 32)
	2 = output (only available for signal = 33)
(3) format
	[ 15:0 ]
	digital audio format select
	16 = I²S 16 bits (fIIS_BCK = 32 * samplerate)
	32 = I²S 32 bits (fIIS_BCK = 64 * samplerate) (default)
	272 = lsb aligned 16 bit (fIIS_BCK = 64 * samplerate)
	274 = lsb aligned 18 bit (fIIS_BCK = 64 * samplerate)
	276 = lsb aligned 20 bit (fIIS_BCK = 64 * samplerate)
	280 = lsb aligned 24 bit (fIIS_BCK = 64 * samplerate)
(4) operation
	[ 15:0 ]
	operation mode
	0 = slave mode; IIS_BCK and IIS_WS input defined by source (default)
	256 = master mode; IIS_BCK and IIS_WS output defined by device
(5) samplerate
	[ 15:0 ] 3200 = 32.0 kHz
	4410 = 44.1 kHz (default)
	4800 = 48.0 kHz
*/
int devTEF668x_Audio_Set_Dig_IO(U16 signal,U16 mode,U16 format,U16 operation,U16 samplerate)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_AUDIO,
			TEF665X_Cmd_Set_Dig_IO, 
			13,
			signal,mode,format,operation,samplerate);
}

/*
module 48 AUDIO
cmd 23 Set_Input_Scaler source, gain

index 
1 source
	[ 15:0 ]
	audio source
	32 = I²S digital audio input : IIS_SD_0
2 gain
	[ 15:0 ] (signed)
	external source channel gain
	-120 … +60 [*0.1 dB] = -12 … +6 dB external source signal gain
	0 = 0 dB (default)
*/
int devTEF668x_Audio_Set_Input_Scaler(U16 source,U16 gain)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_AUDIO,
			TEF665X_Cmd_Set_Input_Scaler, 
			7,
			source,gain);
}

/*
module 48 AUDIO
cmd 24 Set_WaveGen mode, offset, amplitude1, frequency1, amplitude2, frequency2

index 
1 mode
	[ 15:0 ]
	mode
	0 = wave signal off (default)
	1 = wave 1 signal on Left channel
	2 = wave 2 signal on Right channel
	3 = wave 1 signal on Left channel and wave 2 signal on Right channel
	5 = wave 1 signal on Left and Right channel
	6 = wave 2 signal on Left and Right channel
	7 = wave 1 + wave 2 signal on Left and Right channel
2 offset
	[ 15:0 ]
	DC offset
	-32768 … + 32767 (* 1 LSB of 16 bit) = max negative … max positive.
	0 = no offset (default)
3 amplitude1
	[ 15:0 ] signed
	wave 1 amplitude
	-300 … 0 (*0.1 dB) = -30 … 0 dB
	-200 = -20 dB (default)
4 frequency1
	[ 15:0 ]
	wave 1 frequency
	10 … 20000 (*1 Hz) = 10 Hz … 20 kHz
	400 = 400 Hz (default)
5 amplitude2
	[ 15:0 ] signed
	wave 2 amplitude
	-300 … 0 (*0.1 dB) = -30 … 0 dB
	-200 = -20 dB (default)
6 frequency2
	[ 15:0 ]
	wave 2 frequency
	10 … 20000 (*1 Hz) = 10 Hz … 20 kHz
	1000 = 1 kHz (default)
*/
int devTEF668x_Audio_Set_WaveGen(U16 mode,U16 offset,U16 amplitude1,U16 frequency1,U16 amplitude2,U16 frequency2)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_AUDIO,
			TEF665X_Cmd_Set_WaveGen, 
			15,
			mode,offset,amplitude1,frequency1,amplitude2,frequency2);
}

/*
module 64 APPL
cmd 1 Set_OperationMode mode

index 
1 mode
	[ 15:0 ]
	device operation mode
	0 = normal operation
	1 = radio standby mode (low-power mode without radio functionality)
	(default)
*/
int devTEF668x_APPL_Set_OperationMode(U16 mode)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_APPL,
			TEF665X_Cmd_Set_OperationMode, 
			5,
			mode);
}

/*
module 64 APPL
cmd 3 Set_GPIO pin, module, feature

index 
1 pin
	[ 15:0 ]
	GPIO number
	0 … 2 = GPIO number
2 module
	[ 15:0 ]
	module
	32 = FM
	33 = AM
3 feature
	[ 15:0 ]
	feature
	0 = no use (default) (FM / AM)
	1 = general purpose input (FM / AM)
	2 = general purpose output ‘0’ (FM / AM)
	3 = general purpose output ‘1’ (FM / AM)
	257 = output RDS (FM : see cmd 81 ‘DAVN’)
	258 = output QSI (FM / AM : see cmd 82 ‘timer and AF_Update flag’)
	259 = output QSI + RDS (active ‘low’ if ‘DAVN’ is active or ‘QSI’ is active)
	260 = output RDDA (FM : see cmd 81 ‘RDDA, RDCL legacy option’)
	261 = output RDCL (FM : see cmd 81 ‘RDDA, RDCL legacy option’)
	262 = output AGC (FM : see cmd 11 ‘AGC step extension’)
*/
int devTEF668x_APPL_Set_GPIO(U16 pin,U16 module,U16 feature)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_APPL,
			TEF665X_Cmd_Set_GPIO, 
			9,
			pin,module,feature);
}

/*
module 64 APPL
cmd 4 Set_ReferenceClock frequency

index 
1 frequency_high
	MSB part of the reference clock frequency
	[ 31:16 ]
2 frequency_low
	LSB part of the reference clock frequency
	[ 15:0 ]
	frequency [*1 Hz] (default = 9216000)
3 type
	clock type
	0 = crystal oscillator operation (default)
	1 = external clock input operation
*/
int devTEF668x_APPL_Set_ReferenceClock(U16 frequency_high,U16 frequency_low,U16 type)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_APPL,
			TEF665X_Cmd_Set_ReferenceClock, 
			9,
			frequency_high,frequency_low,type);
}

/*
module 64 APPL
cmd 5 Activate mode

index 
1 mode
	[ 15:0 ]
	1 = goto ‘active’ state with operation mode of ‘radio standby’
*/
int devTEF668x_APPL_Activate(U16 mode)
{
	return devTEF668x_Set_Cmd(TEF665X_MODULE_APPL,
			TEF665X_Cmd_Activate, 
			5,
			mode);
}

/*
module 32 / 33 FM / AM
cmd 128 / 129 Get_Quality_Status/ Get_Quality_Data
FM : | status, level, usn, wam, offset, bandwidth, modulation
AM : | status, level, -, -, offset, bandwidth, modulation

index 
1 status
	[ 15:0 ]
	quality detector status
	[15] =
	AF_update flag
	0 = continuous quality data with time stamp
	1 = AF_Update sampled data
	[14:10] = reserved
	[9:0] = quality time stamp
	0 = tuning is in progress, no quality data available
	1 ... 320 (* 0.1 ms) = 0.1 ... 32 ms after tuning,
	quality data available, reliability depending on time stamp
	1000 = > 32 ms after tuning, quality data continuously updated
2 level
	[ 15:0 ] (signed)
	level detector result
	-200 ... 1200 (0.1 * dBuV) = -20 ... 120 dBuV RF input level
	actual range and accuracy is limited by noise and agc
3 usn
	[ 15:0 ]
	FM ultrasonic noise detector
	0 ... 1000 (*0.1 %) = 0 ... 100% relative usn detector result
4 wam
	[ 15:0 ]
	FM wideband-AM¡ multipath detector
	0 ... 1000 (*0.1 %) = 0 ... 100% relative wam detector result
5 offset
	[ 15:0 ] (signed)
	radio frequency offset
	-1200 ... 1200 (*0.1 kHz) = -120 kHz ... 120 kHz radio frequency error
	actual range and accuracy is limited by noise and bandwidth
6 bandwidth
	[ 15:0 ]
	IF bandwidth
	FM 560 ...3110 [*0.1 kHz] = IF bandwidth 56 ... 311 kHz; narrow ¡K wide
	AM 30 ... 80 [*0.1 kHz] = IF bandwidth 3 ... 8 kHz; narrow ¡K wide
7 modulation
	[ 15:0 ]
	modulation detector
	FM 0 ... 1000 [*0.1 %] = 0 ... 100% modulation = 0 ... 75 kHz FM dev.
	1000 ... 2000 [*0.1 %] = 100% ... 200% over-modulation range
	(modulation results are an approximate indication of actual FM dev.)
	AM 0 ... 1000 [*0.1 %] = 0 ... 100% AM modulation index
	1000 ... 2000 [*0.1 %] = 100% ... 200% peak modulation range
*/
int devTEF668x_Radio_Get_Quality_Status (bool fm,U8 *status)
{
	U8 buf[2];
	int r;
	
	r = devTEF668x_Get_Cmd(fm ? TEF665X_MODULE_FM : TEF665X_MODULE_AM,
			TEF665X_Cmd_Get_Operation_Status,
			buf,sizeof(buf));

	if(r == SUCCESS){
		*status = ((0x3fff&Convert8bto16b(buf))/10);
	}

	return r;
}
Bool devTEF668x_Radio_Is_AF_Update_Available (void)
{
	U8 buf[2];
	int r;
	
	r = devTEF668x_Get_Cmd(TEF665X_MODULE_FM,TEF665X_Cmd_Get_Operation_Status,
			buf,sizeof(buf));

	if(r == SUCCESS){
		return ((buf[0]&0xC0)== 0x80);
	}

	return FALSE;
}
Bool devTEF668x_Radio_Is_RDAV_Available (void)
{
	U8 buf[2];
	int r;
	
	r = devTEF668x_Get_Cmd(TEF665X_MODULE_FM,TEF665X_Cmd_Get_RDS_Status,
			buf,sizeof(buf));

	if(r == SUCCESS){
		return ((buf[0]&0x02)== 0x02);
	}

	return FALSE;
}

int devTEF668x_Radio_Get_Quality_Level (bool fm,U8 *status,int16_t *level)
{
	U8 buf[4];
	int r;
	
	r = devTEF668x_Get_Cmd(fm ? TEF665X_MODULE_FM : TEF665X_MODULE_AM,
			TEF665X_Cmd_Get_Quality_Data,
			buf,sizeof(buf));

	if(r == SUCCESS){
		*status = (int16_t)((0x3fff&Convert8bto16b(buf))/10);
		*level = (int16_t)(((int16_t)Convert8bto16b(buf+2))/10);
	}

	return r;
}

/*
status	= 1 … 320 (* 0.1 ms) = 0.1 … 32 ms after tuning
level		= -200 ... 1200 (0.1 * dBuV) = -20 ... 120 dBuV RF input level
usn		=  0 … 1000 (*0.1 %) = 0 … 100% relative usn detector result
wam		=  0 … 1000 (*0.1 %) = 0 … 100% relative wam detector result
offset	 = -1200 … 1200 (*0.1 kHz) = -120 kHz … 120 kHz radio frequency error
bandwith	 = FM 560 … 3110 [*0.1 kHz] = IF bandwidth 56 … 311 kHz; narrow … wide
			AM 30 … 80 [*0.1 kHz] = IF bandwidth 3 … 8 kHz; narrow … wide
modulation	 = FM 0 … 1000 [*0.1 %] = 0 … 100% modulation = 0 … 75 kHz FM dev.
				1000 … 2000 [*0.1 %] = 100% … 200% over-modulation range
			AM 0 … 1000 [*0.1 %] = 0 … 100% AM modulation index
			1000 … 2000 [*0.1 %] = 100% … 200% peak modulation range
			
*/
int devTEF668x_Radio_Get_Quality_Data (bool fm,U8 *usn,U8 *wam,int16_t *offset)
{
	U8 buf[14];
	int r;
	int16_t temp;
	
	r = devTEF668x_Get_Cmd(fm ? TEF665X_MODULE_FM : TEF665X_MODULE_AM,
			TEF665X_Cmd_Get_Quality_Data,
			buf,sizeof(buf));

	if(r == SUCCESS)
	{
		//*status = ((0x3fff&Convert8bto16b(buf))/10);
		//*level = (((int16_t)Convert8bto16b(buf+2))/10);
		*usn = (Convert8bto16b(buf+4)/10);
		*wam = (Convert8bto16b(buf+6)/10);
		temp = (((int16_t)Convert8bto16b(buf+8))/1);
		*offset = temp<0? (0-temp) : temp;
		//*bandwidth = (Convert8bto16b(buf+10)/1);
		//*modulation = (Convert8bto16b(buf+12)/10);
	}

	return r;
}


/*
module 32 FM
cmd 130 / 131 Get_RDS_Status
/ Get_RDS_Data
| status, A_block, B_block, C_block, D_block, dec_error

index 
1 status
	[ 15:0 ]
	FM RDS reception status
	[15] = data available flag
	0 = no data available (incomplete group or no first PI)
	1 = RDS group data or first PI data available
	[14] =
	data loss flag
	0 = no data loss
	1 = previous data was not read, replaced by newer data
	[13] =
	data available type
	0 = group data; continuous operation
	1 = first PI data; data with PI code following decoder sync.
	[12] = group type
	0 = type A; A-B-C-D group (with PI code in block A)
	1 = type B; A-B-C’-D group (with PI code in block A and C’)
	[11:10] = reserved
	[9] = synchronization status
	0 = RDS decoder not synchronized; no RDS data found
	1 = RDS decoder synchronized; RDS data reception active
	[8: 0] = reserved
2 A_block
	[ 15:0 ]
	A block data
3 B_block
	[ 15:0 ]
	B block data
4 C_block
	[ 15:0 ]
	C block data
5 D_block
	[ 15:0 ]
	D block data
6 dec_error error code (determined by decoder)
	[ 15:0 ] [15:14] = A block error code
	[13:12] = B block error code
	[11:10] = C block error code
	[9:8] = D block error code
	0 : no error; block data was received with matching data and syndrome
	1 : small error; possible 1 bit reception error detected; data is corrected
	2 : large error; theoretical correctable error detected; data is corrected
	3 : uncorrectable error; no data correction possible
	[7:0] = reserved
*/
int devTEF668x_Radio_Get_RDS_Status(bool fm,U16 *status)
{
	U8 buf[2];
	int r;
	
	r = devTEF668x_Get_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Get_RDS_Status,
			buf,sizeof(buf));

	if(r == SUCCESS){
		*status = Convert8bto16b(buf);
	}

	return r;
}
int devTEF668x_Radio_Get_RDS_Data (bool fm,U16 *status,U16 *A_block,U16 *B_block,U16 *C_block,U16 *D_block,
	U16 *dec_error)
{
	U8 buf[12];
	int r;
	
	r = devTEF668x_Get_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Get_RDS_Data,
			buf,sizeof(buf));

	if(r == SUCCESS){
		*status = Convert8bto16b(buf);
		*A_block = Convert8bto16b(buf+2);
		*B_block = Convert8bto16b(buf+4);
		*C_block = Convert8bto16b(buf+6);
		*D_block = Convert8bto16b(buf+8);
		*dec_error = Convert8bto16b(buf+10);
	}

	return r;
}
int devTEF668x_Radio_Get_RDS_DataRaw (bool fm,U16 *status,U32 *raw_data)
{
	U8 buf[6];
	int r;
	
	r = devTEF668x_Get_Cmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Get_RDS_Data,
			buf,sizeof(buf));

	if(r == SUCCESS){
		*status = Convert8bto16b(buf);
		*raw_data = ((unsigned long)buf[2]<<24) | ((unsigned long)buf[3]<<16) | ((unsigned long)buf[4]<<8) | ((unsigned long)buf[5]);
	}

	return r;
}

/*
module 32 / 33 FM / AM
cmd 132 Get_AGC | input_att, feedback_att

index 
1 input_att
	[ 15:0 ]
	RF AGC input attenuation
	FM 0 … 420 (0.1* dB) = 0 … 42 dB attenuation
	AM 0 … 420 (0.1* dB) = 0 … 42 dB attenuation
2 feedback_att
	[ 15:0 ]
	RF AGC feedback attenuation
	FM 0 … 60 (0.1* dB) = 0 … 6 dB attenuation
	AM 0 … 180 (0.1* dB) = 0 … 18 dB attenuation
*/
int devTEF668x_Radio_Get_AGC(bool fm,U16 *input_att,U16 *feedback_att)
{
	U8 buf[4];
	int r;
	
	r = devTEF668x_Get_Cmd(fm ? TEF665X_MODULE_FM : TEF665X_MODULE_AM,
			TEF665X_Cmd_Get_AGC,
			buf,sizeof(buf));

	if(r == SUCCESS){
		*input_att = Convert8bto16b(buf);
		*feedback_att = Convert8bto16b(buf+2);
	}

	return r;
}

/*
module 32 / 33 FM / AM
cmd 133 Get_Signal_Status | status

index 
1 status 
	Radio signal information
	[15] = 0 : mono signal
	[15] = 1 : FM stereo signal (stereo pilot detected)
	[14] = 0 : analog signal
	[14] = 1 : digital signal (blend input activated by digital processor or control)
*/
int devTEF668x_Radio_Get_Signal_Status(bool fm,U16 *status)
{
	U8 buf[2];
	int r;
	
	r = devTEF668x_Get_Cmd(fm ? TEF665X_MODULE_FM : TEF665X_MODULE_AM,
			TEF665X_Cmd_Get_Signal_Status,
			buf,sizeof(buf));

	if(r == SUCCESS){
		*status = Convert8bto16b(buf);
	}

	return r;
}

/*
module 32/33 FM / AM
cmd 134 Get_Processing_Status | softmute, highcut, stereo
index
1 softmute Softmute control state
	0 … 1000 (*0.1%) = 0 % minimum … 100 % max. softmute attenuation
2 highcut Highcut control state
	0 … 1000 (*0.1%) = 0 % minimum … 100 % max. audio freq. limitation
3 stereo FM Stereo blend control state
	0 … 1000 (*0.1%) = 0 % minimum … 100 % max. stereo att. (= mono)
*/
int devTEF668x_Radio_Get_Processing_Status(bool fm,U16 *softmute,U16 *highcut,U16 *stereo)
{
	U8 buf[6];
	int r;
	
	r = devTEF668x_Get_Cmd(fm ? TEF665X_MODULE_FM : TEF665X_MODULE_AM,
			TEF665X_Cmd_Get_Processing_Status,
			buf,sizeof(buf));

	if(r == SUCCESS){
		*softmute = Convert8bto16b(buf);
		*highcut = Convert8bto16b(buf+2);
		*stereo = Convert8bto16b(buf+4);
	}

	return r;
}

/*
module 32 / 33 FM / AM
cmd 135 Get_Interface_Status | samplerate
index 
1 samplerate
	[ 15:0 ]
	Baseband digital radio sample rate (DR_I2S output)
	0 = interface disabled (digital radio disabled)
	6500 = 650 kHz
	6750 = 675 kHz
*/
int devTEF668x_Radio_Get_Interface_Status(bool fm,U16 *samplerate)
{
	U8 buf[2];
	int r;
	
	r = devTEF668x_Get_Cmd(fm ? TEF665X_MODULE_FM : TEF665X_MODULE_AM,
			TEF665X_Cmd_Get_Interface_Status,
			buf,sizeof(buf));

	if(r == SUCCESS){
		*samplerate = Convert8bto16b(buf);
	}

	return r;
}

/*
module 64 APPL
cmd 128 Get_Operation_Status | status
index 
1 status
	Device operation status
	0 = boot state; no command support
	1 = idle state
	2 = active state; radio standby
	3 = active state; FM
	4 = active state; AM
*/
int devTEF668x_APPL_Get_Operation_Status(U8 *status)
{
	U8 buf[2];
	int r;
	
	r = devTEF668x_Get_Cmd(TEF665X_MODULE_APPL,
			TEF665X_Cmd_Get_Operation_Status,
			buf,sizeof(buf));

	if(r == SUCCESS){
		*status = Convert8bto16b(buf);
	}

	return r;
}


/*
module 64 APPL
cmd 129 Get_GPIO_Status | status
index 
1 status 
	input state (when assigned for input use)
	[2] = input state of GPIO_2 (no input use suggested for TEF668x)
	[1] = input state of GPIO_1 (no input use suggested for TEF668x)
	[0] = input state of GPIO_0 (0 = low, 1 = high)
*/
int devTEF668x_APPL_Get_GPIO_Status(U16 *status)
{
	U8 buf[2];
	int r;
	
	r = devTEF668x_Get_Cmd(TEF665X_MODULE_APPL,
			TEF665X_Cmd_Get_GPIO_Status,
			buf,sizeof(buf));

	if(r == SUCCESS){
		*status = Convert8bto16b(buf);
	}

	return r;
}



/*
module 64 APPL
cmd 130 Get_Identification | device, hw_version, sw_version
index 
1 device 
	device type and variant
	[ 15:8 ] type identifier
	8 = TEF668x ‘Atomic-2’ series
	[ 7:0 ] variant identifier
	15 = TEF6653 ‘Atomic-2 Standard’
	13 = TEF6657 ‘Atomic-2 Premium’
	12 = TEF6659 ‘Atomic-2 Premium DR’
2 hw_version 
	hardware version
	[ 15:8 ] major number
	1 = ‘V1’
	[ 7:0 ] minor number
	0 = ‘B’
3 sw_version 
	firmware version
	[ 15:8 ] major number
	1 = ‘1’
	[ 7:0 ] minor number
	1 = ‘.01’
*/
int devTEF668x_APPL_Get_Identification(U16 *device,U16 *hw_version,U16 *sw_version)
{
	U8 buf[6];
	int r;
	
	r = devTEF668x_Get_Cmd(TEF665X_MODULE_APPL,
			TEF665X_Cmd_Get_Operation_Status,
			buf,sizeof(buf));

	if(r == SUCCESS){
		*device = Convert8bto16b(buf);
		*hw_version = Convert8bto16b(buf+2);
		*sw_version = Convert8bto16b(buf+4);
	}

	return r;
}


/*

module 64 APPL
cmd 131 Get_LastWrite | size/module, cmd/ index, parameter1, parameter2, parameter3, …

index 
1 size/module transmission size (number of parameters) and module number
	[ 15:8 ] = 0 … 6 : number of parameters of the last write transmission
	[ 7:0 ] = 0 … 255 : module value of the last write transmission
2 cmd/index
	command byte number and index byte value
	[ 15:8 ] = 0 … 255 : cmd value of the last write transmission
	[ 7:0 ] = 0 ... 255 : index value of the last write transmission
3 parameter1 first parameter
	0 … 65535 = value of the first parameter (when available)
4 parameter2 second parameter
	0 … 65535 = value of the second parameter (when available)
5 parameter3 third parameter
	0 … 65535 = value of the third parameter (when available)
6 parameter4 fourth parameter
	0 … 65535 = value of the fourth parameter (when available)
7 parameter5 fifth parameter
	0 … 65535 = value of the fifth parameter (when available)
*/
int devTEF668x_APPL_Get_LastWrite(U8 *buf,int len)
{
	int r;
	
	r = devTEF668x_Get_Cmd(TEF665X_MODULE_APPL,
			TEF665X_Cmd_Get_LastWrite,
			buf,len);

	return r;
}


