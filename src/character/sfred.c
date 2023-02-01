/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "sfred.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//SFred character structure
enum
{
	SFred_ArcMain_Idle0,
	SFred_ArcMain_Idle1,
	SFred_ArcMain_Idle2,
	SFred_ArcMain_Idle3,
	SFred_ArcMain_Idle4,
	SFred_ArcMain_Idle5,
	SFred_ArcMain_Left0,
	SFred_ArcMain_Left1,
	SFred_ArcMain_Left2,
	SFred_ArcMain_Left3,
	SFred_ArcMain_Left4,
	SFred_ArcMain_Left5,
	SFred_ArcMain_Left6,
	SFred_ArcMain_Left7,
	SFred_ArcMain_Left8,
	SFred_ArcMain_Left9,
	SFred_ArcMain_Left10,
	SFred_ArcMain_Left11,
	SFred_ArcMain_Down0,
	SFred_ArcMain_Down1,
	SFred_ArcMain_Down2,
	SFred_ArcMain_Down3,
	SFred_ArcMain_Down4,
	SFred_ArcMain_Down5,
	SFred_ArcMain_Down6,
	SFred_ArcMain_Down7,
	SFred_ArcMain_Down8,
	SFred_ArcMain_Down9,
	SFred_ArcMain_Down10,
	SFred_ArcMain_Down11,
	SFred_ArcMain_Down12,
	SFred_ArcMain_Down13,
	SFred_ArcMain_Down14,
	SFred_ArcMain_Up0,
	SFred_ArcMain_Up1,
	SFred_ArcMain_Up2,
	SFred_ArcMain_Up3,
	SFred_ArcMain_Up4,
	SFred_ArcMain_Up5,
	SFred_ArcMain_Up6,
	SFred_ArcMain_Up7,
	SFred_ArcMain_Up8,
	SFred_ArcMain_Up9,
	SFred_ArcMain_Up10,
	SFred_ArcMain_Up11,
	SFred_ArcMain_Up12,
	SFred_ArcMain_Up13,
	SFred_ArcMain_Right0,
	SFred_ArcMain_Right1,
	SFred_ArcMain_Right2,
	SFred_ArcMain_Right3,
	SFred_ArcMain_Right4,
	SFred_ArcMain_Right5,
	SFred_ArcMain_Right6,
	SFred_ArcMain_Right7,
	SFred_ArcMain_Right8,
	SFred_ArcMain_Bite0,
	SFred_ArcMain_Bite1,
	SFred_ArcMain_Bite2,
	SFred_ArcMain_Bite3,
	SFred_ArcMain_Bite4,
	SFred_ArcMain_Bite5,
	SFred_ArcMain_Bite6,
	SFred_ArcMain_Bite7,
	SFred_ArcMain_Bite8,
	SFred_ArcMain_Bite9,
	SFred_ArcMain_Bite10,
	SFred_ArcMain_Bite11,
	SFred_ArcMain_Bite12,
	SFred_ArcMain_Bite13,
	SFred_ArcMain_Bite14,
	SFred_ArcMain_Bite15,
	SFred_ArcMain_Bite16,
	SFred_ArcMain_Bite17,
	SFred_ArcMain_Bite18,
	SFred_ArcMain_Bite19,
	SFred_ArcMain_Bite20,
	SFred_ArcMain_Bite21,
	SFred_ArcMain_Bite22,
	
	SFred_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[SFred_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_SFred;

//SFred character definitions
static const CharFrame char_sfred_frame[] = {
	{SFred_ArcMain_Idle0, {  0,  0,136,142}, { 42,183}}, //0 idle 1
	{SFred_ArcMain_Idle1, {  0,  0,136,142}, { 42,183}}, //1 idle 2
	{SFred_ArcMain_Idle2, {  0,  0,136,142}, { 42,183}}, //2 idle 3
	{SFred_ArcMain_Idle3, {  0,  0,136,142}, { 42,183}}, //3 idle 4
	{SFred_ArcMain_Idle4, {  0,  0,136,142}, { 42,183}}, //4 idle 5
	{SFred_ArcMain_Idle5, {  0,  0,136,142}, { 42,183}}, //5 idle 6
	
	{SFred_ArcMain_Left0, {  0,  0,132,142}, { 42,183}}, //6 left 1
	{SFred_ArcMain_Left1, {  0,  0,132,142}, { 42,183}}, //7 left 2
	{SFred_ArcMain_Left2, {  0,  0,132,142}, { 42,183}}, //8 left 3
	{SFred_ArcMain_Left3, {  0,  0,132,142}, { 42,183}}, //9 left 4
	{SFred_ArcMain_Left4, {  0,  0,132,142}, { 42,183}}, //10 left 5
	{SFred_ArcMain_Left5, {  0,  0,132,142}, { 42,183}}, //11 left 6
	{SFred_ArcMain_Left6, {  0,  0,132,142}, { 42,183}}, //12 left 7
	{SFred_ArcMain_Left7, {  0,  0,132,142}, { 42,183}}, //13 left 8
	{SFred_ArcMain_Left8, {  0,  0,132,142}, { 42,183}}, //14 left 9
	{SFred_ArcMain_Left9, {  0,  0,132,142}, { 42,183}}, //15 left 10
	{SFred_ArcMain_Left10, {  0,  0,132,142}, { 42,183}}, //16 left 11
	{SFred_ArcMain_Left11, {  0,  0,132,142}, { 42,183}}, //17 left 12
	
	{SFred_ArcMain_Down0, {  0,  0,136,142}, { 42,183}}, //18 down 1
	{SFred_ArcMain_Down1, {  0,  0,136,142}, { 42,183}}, //19 down 2
	{SFred_ArcMain_Down2, {  0,  0,148,142}, { 42,183}}, //20 down 3
	{SFred_ArcMain_Down3, {  0,  0,136,142}, { 42,183}}, //21 down 4
	{SFred_ArcMain_Down4, {  0,  0,136,142}, { 42,183}}, //22 down 5
	{SFred_ArcMain_Down5, {  0,  0,140,142}, { 42,183}}, //23 down 6
	{SFred_ArcMain_Down6, {  0,  0,136,142}, { 42,183}}, //24 down 7
	{SFred_ArcMain_Down7, {  0,  0,140,142}, { 42,183}}, //25 down 8
	{SFred_ArcMain_Down8, {  0,  0,136,142}, { 42,183}}, //26 down 9
	{SFred_ArcMain_Down9, {  0,  0,136,142}, { 42,183}}, //27 down 10
	{SFred_ArcMain_Down10, {  0,  0,136,142}, { 42,183}}, //28 down 11
	{SFred_ArcMain_Down11, {  0,  0,136,142}, { 42,183}}, //29 down 12
	{SFred_ArcMain_Down12, {  0,  0,136,142}, { 42,183}}, //30 down 13
	{SFred_ArcMain_Down13, {  0,  0,140,142}, { 42,183}}, //31 down 14
	{SFred_ArcMain_Down14, {  0,  0,136,142}, { 42,183}}, //32 down 15
	
	{SFred_ArcMain_Up0, {  0,  0,136,142}, { 42,183}}, //33 up 1
	{SFred_ArcMain_Up1, {  0,  0,136,142}, { 42,183}}, //34 up 2
	{SFred_ArcMain_Up2, {  0,  0,136,142}, { 42,183}}, //35 up 3
	{SFred_ArcMain_Up3, {  0,  0,136,142}, { 42,183}}, //36 up 4
	{SFred_ArcMain_Up4, {  0,  0,140,142}, { 42,183}}, //37 up 5
	{SFred_ArcMain_Up5, {  0,  0,140,142}, { 42,183}}, //38 up 6
	{SFred_ArcMain_Up6, {  0,  0,136,142}, { 42,183}}, //39 up 7
	{SFred_ArcMain_Up7, {  0,  0,136,142}, { 42,183}}, //40 up 8
	{SFred_ArcMain_Up8, {  0,  0,140,142}, { 42,183}}, //41 up 9
	{SFred_ArcMain_Up9, {  0,  0,136,142}, { 42,183}}, //42 up 10
	{SFred_ArcMain_Up10, {  0,  0,136,142}, { 42,183}}, //43 up 11
	{SFred_ArcMain_Up11, {  0,  0,136,142}, { 42,183}}, //44 up 12
	{SFred_ArcMain_Up12, {  0,  0,136,142}, { 42,183}}, //45 up 13
	{SFred_ArcMain_Up13, {  0,  0,136,142}, { 42,183}}, //46 up 14
	
	{SFred_ArcMain_Right0, {  0,  0,197,142}, { 42,183}}, //47 right 1
	{SFred_ArcMain_Right1, {  0,  0,197,142}, { 42,183}}, //48 right 2
	{SFred_ArcMain_Right2, {  0,  0,197,142}, { 42,183}}, //49 right 3
	{SFred_ArcMain_Right3, {  0,  0,197,142}, { 42,183}}, //50 right 4
	{SFred_ArcMain_Right4, {  0,  0,197,142}, { 42,183}}, //51 right 5
	{SFred_ArcMain_Right5, {  0,  0,197,142}, { 42,183}}, //52 right 6
	{SFred_ArcMain_Right6, {  0,  0,197,142}, { 42,183}}, //53 right 7
	{SFred_ArcMain_Right7, {  0,  0,197,142}, { 42,183}}, //54 right 8
	{SFred_ArcMain_Right8, {  0,  0,197,142}, { 42,183}}, //55 right 9
	
	{SFred_ArcMain_Bite0, {  0,  0,136,142}, { 42,183}}, //56 bite 1
	{SFred_ArcMain_Bite1, {  0,  0,136,142}, { 42,183}}, //57 bite 2
	{SFred_ArcMain_Bite2, {  0,  0,136,142}, { 42,183}}, //58 bite 3
	{SFred_ArcMain_Bite3, {  0,  0,136,142}, { 42,183}}, //59 bite 4
	{SFred_ArcMain_Bite4, {  0,  0,140,142}, { 42,183}}, //60 bite 5
	{SFred_ArcMain_Bite5, {  0,  0,136,142}, { 42,183}}, //61 bite 6
	{SFred_ArcMain_Bite6, {  0,  0,140,142}, { 42,183}}, //62 bite 7
	{SFred_ArcMain_Bite7, {  0,  0,144,142}, { 42,183}}, //63 bite 8
	{SFred_ArcMain_Bite8, {  0,  0,144,142}, { 42,183}}, //64 bite 9
	{SFred_ArcMain_Bite9, {  0,  0,144,142}, { 42,183}}, //65 bite 10
	{SFred_ArcMain_Bite10, {  0,  0,144,142}, { 42,183}}, //66 bite 11
	{SFred_ArcMain_Bite11, {  0,  0,144,142}, { 42,183}}, //67 bite 12
	{SFred_ArcMain_Bite12, {  0,  0,144,142}, { 42,183}}, //68 bite 13
	{SFred_ArcMain_Bite13, {  0,  0,144,142}, { 42,183}}, //69 bite 14
	{SFred_ArcMain_Bite14, {  0,  0,136,142}, { 42,183}}, //70 bite 15
	{SFred_ArcMain_Bite15, {  0,  0,136,142}, { 42,183}}, //71 bite 16
	{SFred_ArcMain_Bite16, {  0,  0,197,142}, { 42,183}}, //72 bite 17
	{SFred_ArcMain_Bite17, {  0,  0,197,142}, { 42,183}}, //73 bite 18
	{SFred_ArcMain_Bite18, {  0,  0,197,142}, { 42,183}}, //74 bite 19
	{SFred_ArcMain_Bite19, {  0,  0,197,142}, { 42,183}}, //75 bite 20
	{SFred_ArcMain_Bite20, {  0,  0,197,142}, { 42,183}}, //76 bite 21
	{SFred_ArcMain_Bite21, {  0,  0,197,142}, { 42,183}}, //77 bite 22
	{SFred_ArcMain_Bite22, {  0,  0,197,142}, { 42,183}}, //78 bite 23
};

static const Animation char_sfred_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 0, 0, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 2, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){47, 48, 49, 50, 51, 52, 53, 54, 53, 54, 53, 54, 53, 54, 53, 55, ASCR_BACK, 1}},         //CharAnim_Right
	{1, (const u8[]){ 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 59, 59, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 
	62, 62, 63, 63, 64, 64, 65, 65, 64, 64, 63, 63, 64, 64, 63, 63, 64, 64, 63, 63, 64, 64, 63, 63, 64, 64, 63, 63, 64, 64, 63, 63, 66, 66, 67, 67, 66, 66, 67, 67, 66, 66, 63, 63,
	66, 66, 67, 67, 66, 66, 67, 67, 66, 66, 63, 63, 67, 67, 64, 64, 68, 68, 69, 69, 68, 68, 69, 69, 68, 68, 69, 69, 68, 68, 69, 69, 68, 68, 69, 69, 68, 68, 69, 69, 68, 68, 69, 69, 68, 68, 69, 69, 68, 68, 69, 69, 68, 68, 69, 69, 68, 68, 
	70, 70, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 77, 77, 78, 78, ASCR_CHGANI, CharAnim_Right}},   //CharAnim_RightAlt
};

//SFred character functions
void Char_SFred_SetFrame(void *user, u8 frame)
{
	Char_SFred *this = (Char_SFred*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_sfred_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_SFred_Tick(Character *character)
{
	Char_SFred *this = (Char_SFred*)character;
	
	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_LeftAlt &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_DownAlt &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_UpAlt &&
	     character->animatable.anim != CharAnim_Right &&
	     character->animatable.anim != CharAnim_RightAlt))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if (Animatable_Ended(&character->animatable) &&
			(character->animatable.anim != CharAnim_Left &&
		     character->animatable.anim != CharAnim_LeftAlt &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt) &&
			(stage.song_step & 0x3) == 0)
			character->set_anim(character, CharAnim_Idle);
	}
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_5_3: //Bite
				if (stage.song_step == -92)
					character->set_anim(character, CharAnim_RightAlt);
				break;
			default:
				break;
		}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_SFred_SetFrame);
	Character_Draw(character, &this->tex, &char_sfred_frame[this->frame]);
}

void Char_SFred_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_SFred_Free(Character *character)
{
	Char_SFred *this = (Char_SFred*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_SFred_New(fixed_t x, fixed_t y)
{
	//Allocate sfred object
	Char_SFred *this = Mem_Alloc(sizeof(Char_SFred));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_SFred_New] Failed to allocate sfred object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_SFred_Tick;
	this->character.set_anim = Char_SFred_SetAnim;
	this->character.free = Char_SFred_Free;
	
	Animatable_Init(&this->character.animatable, char_sfred_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 6;

	//health bar color
	this->character.health_bar = 0xFF1A0027;
	
	this->character.focus_x = FIXED_DEC(53,1);
	this->character.focus_y = FIXED_DEC(-127,1);
	this->character.focus_zoom = FIXED_DEC(203,128);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR2\\SFRED.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //SFred_ArcMain_Idle0
		"idle1.tim", //SFred_ArcMain_Idle1
		"idle2.tim", //SFred_ArcMain_Idle2
		"idle3.tim", //SFred_ArcMain_Idle3
		"idle4.tim", //SFred_ArcMain_Idle4
		"idle5.tim", //SFred_ArcMain_Idle5
		"left0.tim", //SFred_ArcMain_Left0
		"left1.tim", //SFred_ArcMain_Left1
		"left2.tim", //SFred_ArcMain_Left2
		"left3.tim", //SFred_ArcMain_Left3
		"left4.tim", //SFred_ArcMain_Left4
		"left5.tim", //SFred_ArcMain_Left5
		"left6.tim", //SFred_ArcMain_Left6
		"left7.tim", //SFred_ArcMain_Left7
		"left8.tim", //SFred_ArcMain_Left8
		"left9.tim", //SFred_ArcMain_Left9
		"left10.tim", //SFred_ArcMain_Left10
		"left11.tim", //SFred_ArcMain_Left11
		"down0.tim", //SFred_ArcMain_Down0
		"down1.tim", //SFred_ArcMain_Down1
		"down2.tim", //SFred_ArcMain_Down2
		"down3.tim", //SFred_ArcMain_Down3
		"down4.tim", //SFred_ArcMain_Down4
		"down5.tim", //SFred_ArcMain_Down5
		"down6.tim", //SFred_ArcMain_Down6
		"down7.tim", //SFred_ArcMain_Down7
		"down8.tim", //SFred_ArcMain_Down8
		"down9.tim", //SFred_ArcMain_Down9
		"down10.tim", //SFred_ArcMain_Down10
		"down11.tim", //SFred_ArcMain_Down11
		"down12.tim", //SFred_ArcMain_Down12
		"down13.tim", //SFred_ArcMain_Down13
		"down14.tim", //SFred_ArcMain_Down14
		"up0.tim", //SFred_ArcMain_Up0
		"up1.tim", //SFred_ArcMain_Up1
		"up2.tim", //SFred_ArcMain_Up2
		"up3.tim", //SFred_ArcMain_Up3
		"up4.tim", //SFred_ArcMain_Up4
		"up5.tim", //SFred_ArcMain_Up5
		"up6.tim", //SFred_ArcMain_Up6
		"up7.tim", //SFred_ArcMain_Up7
		"up8.tim", //SFred_ArcMain_Up8
		"up9.tim", //SFred_ArcMain_Up9
		"up10.tim", //SFred_ArcMain_Up10
		"up11.tim", //SFred_ArcMain_Up11
		"up12.tim", //SFred_ArcMain_Up12
		"up13.tim", //SFred_ArcMain_Up13
		"right0.tim", //SFred_ArcMain_Right0
		"right1.tim", //SFred_ArcMain_Right1
		"right2.tim", //SFred_ArcMain_Right2
		"right3.tim", //SFred_ArcMain_Right3
		"right4.tim", //SFred_ArcMain_Right4
		"right5.tim", //SFred_ArcMain_Right5
		"right6.tim", //SFred_ArcMain_Right6
		"right7.tim", //SFred_ArcMain_Right7
		"right8.tim", //SFred_ArcMain_Right8
		"bite0.tim", //SFred_ArcMain_Bite0
		"bite1.tim", //SFred_ArcMain_Bite1
		"bite2.tim", //SFred_ArcMain_Bite2
		"bite3.tim", //SFred_ArcMain_Bite3
		"bite4.tim", //SFred_ArcMain_Bite4
		"bite5.tim", //SFred_ArcMain_Bite5
		"bite6.tim", //SFred_ArcMain_Bite6
		"bite7.tim", //SFred_ArcMain_Bite7
		"bite8.tim", //SFred_ArcMain_Bite8
		"bite9.tim", //SFred_ArcMain_Bite9
		"bite10.tim", //SFred_ArcMain_Bite10
		"bite11.tim", //SFred_ArcMain_Bite11
		"bite12.tim", //SFred_ArcMain_Bite12
		"bite13.tim", //SFred_ArcMain_Bite13
		"bite14.tim", //SFred_ArcMain_Bite14
		"bite15.tim", //SFred_ArcMain_Bite15
		"bite16.tim", //SFred_ArcMain_Bite16
		"bite17.tim", //SFred_ArcMain_Bite17
		"bite18.tim", //SFred_ArcMain_Bite18
		"bite19.tim", //SFred_ArcMain_Bite19
		"bite20.tim", //SFred_ArcMain_Bite20
		"bite21.tim", //SFred_ArcMain_Bite21
		"bite22.tim", //SFred_ArcMain_Bite22
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
