/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef PSXF_GUARD_AUDIO_H
#define PSXF_GUARD_AUDIO_H

#include "psx.h"

//XA enumerations
typedef enum
{
	XA_Menu,   //MENU.XA
	XA_Week1A, //WEEK1A.XA
	XA_Week1B, //WEEK1B.XA
	XA_Week2A, //WEEK2A.XA
	XA_Week2B, //WEEK2B.XA
	XA_Week3A, //WEEK3A.XA
	XA_Week3B, //WEEK3B.XA
	XA_Week4A, //WEEK4A.XA
	XA_Week4B, //WEEK4B.XA
	XA_Week5A, //WEEK5A.XA
	XA_Week5B, //WEEK5B.XA
	XA_Week6A, //WEEK6A.XA
	XA_Week6B, //WEEK6B.XA
	
	XA_Max,
} XA_File;

typedef enum
{
	//MENU.XA
	XA_MainMenu, //Main Menu
	XA_GameOver,     //Game Over
	//WEEK1A.XA
	XA_Guy, //Guy
	XA_Midnight,   //Midnight
	//WEEK1B.XA
	XA_Terminated, //Terminated
	XA_Lurking,  //Lurking
	//WEEK2A.XA
	XA_Lore, //Lore
	XA_Blubber,    //Blubber
	//WEEK2B.XA
	XA_Golden, //Golden
	//WEEK3A.XA
	XA_Performance,   //Performance
	XA_Bite, //Bite
	//WEEK3B.XA
	XA_Trapped, //Trapped
	//WEEK4A.XA
	XA_GoFish, //Go Fish
	XA_Watchful,         //Watchful
	//WEEK4B.XA
	XA_Restless, //Restless
	XA_Beatbox, //Beatbox
	//WEEK5A.XA
	XA_Showtime,  //Showtime
	XA_Man, //Man
	//WEEK5B.XA
	XA_Followed, //Followed
	//WEEK6A.XA
	XA_FazfuckNews, //Fazfuck News
	XA_Criminal,  //Criminal
	//WEEK6B.XA
	XA_MILLER, //MILLER
	
	XA_TrackMax,
} XA_Track;

//Audio functions
u32 Audio_GetLength(XA_Track lengthtrack);
void Audio_Init(void);
void Audio_Quit(void);
void Audio_Reset(void);
void Audio_PlayXA_Track(XA_Track track, u8 volume, u8 channel, boolean loop);
void Audio_SeekXA_Track(XA_Track track);
void Audio_PauseXA(void);
void Audio_ResumeXA(void);
void Audio_StopXA(void);
void Audio_ChannelXA(u8 channel);
s32 Audio_TellXA_Sector(void);
s32 Audio_TellXA_Milli(void);
boolean Audio_PlayingXA(void);
void Audio_WaitPlayXA(void);
void Audio_ProcessXA(void);
void findFreeChannel(void);
u32 Audio_LoadVAGData(u32 *sound, u32 sound_size);
void AudioPlayVAG(int channel, u32 addr);
void Audio_PlaySoundOnChannel(u32 addr, u32 channel, int volume);
void Audio_PlaySound(u32 addr, int volume);
void Audio_ClearAlloc(void);

#endif
