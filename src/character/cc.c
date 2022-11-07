/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "cc.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//CC player types
enum
{
	CC_ArcMain_CC0,
	CC_ArcMain_CC1,
	CC_ArcMain_CC2,
	CC_ArcMain_CC3,
	CC_ArcMain_CC4,
	
	CC_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[CC_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_CC;

//CC player definitions
static const CharFrame char_cc_frame[] = 
{
	{CC_ArcMain_CC0, {  0,  0, 82,113}, { 53, 92}}, //0 idle 1
	{CC_ArcMain_CC0, { 83,  0, 82,113}, { 53, 92}}, //1 idle 2
	{CC_ArcMain_CC0, {166,  0, 82,113}, { 53, 92}}, //2 idle 3
	{CC_ArcMain_CC0, {  0,114, 82,113}, { 53, 92}}, //3 idle 4
	
	{CC_ArcMain_CC0, { 83,114, 82,113}, { 53, 92}}, //4 left 1
	{CC_ArcMain_CC0, {166,114, 82,113}, { 53, 92}}, //5 left 2
	
	{CC_ArcMain_CC1, {  0,  0, 82,113}, { 53, 92}}, //6 down 1
	{CC_ArcMain_CC1, { 83,  0, 82,113}, { 53, 92}}, //7 down 2
	
	{CC_ArcMain_CC1, {166,  0, 82,113}, { 53, 92}}, //8 up 1
	{CC_ArcMain_CC1, {  0,114, 82,113}, { 53, 92}}, //9 up 2
	
	{CC_ArcMain_CC1, { 83,114, 82,113}, { 53, 92}}, //10 right 1
	{CC_ArcMain_CC1, {166,114, 82,113}, { 53, 92}}, //11 right 2
	
	{CC_ArcMain_CC2, {  0,  0, 82,113}, { 53, 92}}, //12 left miss 1
	{CC_ArcMain_CC2, { 83,  0, 82,113}, { 53, 92}}, //13 left miss 2
	
	{CC_ArcMain_CC2, {166,  0, 82,113}, { 53, 92}}, //14 down miss 1
	{CC_ArcMain_CC2, {  0,114, 82,113}, { 53, 92}}, //15 down miss 2
	
	{CC_ArcMain_CC2, { 83,114, 82,113}, { 53, 92}}, //16 up miss 1
	{CC_ArcMain_CC2, {166,114, 82,113}, { 53, 92}}, //17 up miss 2
	
	{CC_ArcMain_CC3, {  0,  0, 82,113}, { 53, 92}}, //18 right miss 1
	{CC_ArcMain_CC3, { 83,  0, 82,113}, { 53, 92}}, //19 right miss 2
	
	{CC_ArcMain_CC3, {166,  0, 82,113}, { 53, 92}}, //20 intro 1
	{CC_ArcMain_CC4, {  0,  0, 82,113}, { 53, 92}}, //21 intro 2
	{CC_ArcMain_CC4, { 83,  0, 82,113}, { 53, 92}}, //22 intro 3
	{CC_ArcMain_CC4, {166,  0, 82,113}, { 53, 92}}, //23 intro 4
	{CC_ArcMain_CC4, {  0,114, 82,113}, { 53, 92}}, //24 intro 5
};
static const Animation char_cc_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2, 3, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4, 5, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 6, 7, ASCR_BACK, 1}},             //CharAnim_Down
	{3, (const u8[]){ 20, 20, 20, 20, 20, 20, 20, 21, 22, 23, 24, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_DownAlt
	{2, (const u8[]){ 8, 9, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 10, 11, ASCR_BACK, 1}},             //CharAnim_Right
	{3, (const u8[]){ 20, 20, 20, 20, 20, 20, 20, 21, 22, 23, 24, ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{1, (const u8[]){ 12, 13, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 14, 15, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 16, 17, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 18, 19, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//CC player functions
void Char_CC_SetFrame(void *user, u8 frame)
{
	Char_CC *this = (Char_CC*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		CharFrame *cframe;

 		cframe = &char_cc_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_CC_Tick(Character *character)
{
	Char_CC *this = (Char_CC*)character;
	
	//Handle animation updates
	if(character->animatable.anim  != CharAnim_DownAlt)
	{
	    if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	        (character->animatable.anim != CharAnim_Left &&
	         character->animatable.anim != CharAnim_LeftAlt &&
	         character->animatable.anim != CharAnim_Down &&
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
				character->animatable.anim != PlayerAnim_LeftMiss &&
				character->animatable.anim != CharAnim_Down &&
				character->animatable.anim != PlayerAnim_DownMiss &&
				character->animatable.anim != CharAnim_Up &&
				character->animatable.anim != CharAnim_UpAlt &&
				character->animatable.anim != PlayerAnim_UpMiss &&
				character->animatable.anim != CharAnim_Right &&
				character->animatable.anim != CharAnim_RightAlt &&
				character->animatable.anim != PlayerAnim_RightMiss) &&
				(stage.song_step & 0x7) == 0)
				character->set_anim(character, CharAnim_Idle);
	}
}
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_4_3: //Death
				if (stage.song_step == -48)
					character->set_anim(character, CharAnim_DownAlt);
				break;
			default:
				break;
		}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_CC_SetFrame);

	Character_Draw(character, &this->tex, &char_cc_frame[this->frame]);
}

void Char_CC_SetAnim(Character *character, u8 anim)
{
	Char_CC *this = (Char_CC*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_CC_Free(Character *character)
{
	Char_CC *this = (Char_CC*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_CC_New(fixed_t x, fixed_t y)
{
	//Allocate cc object
	Char_CC *this = Mem_Alloc(sizeof(Char_CC));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_CC_New] Failed to allocate cc object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_CC_Tick;
	this->character.set_anim = Char_CC_SetAnim;
	this->character.free = Char_CC_Free;
	
	Animatable_Init(&this->character.animatable, char_cc_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 6;

	//health bar color
	this->character.health_bar = 0xFFB1B1B1;

	//Camera stuff is in fbear.c btw
	this->character.focus_x = FIXED_DEC(-30,1);
	this->character.focus_y = FIXED_DEC(-59,1);
	this->character.focus_zoom = FIXED_DEC(1064,1024);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR2\\CC.ARC;1");
	
	const char **pathp = (const char *[]){
		"cc0.tim",   //CC_ArcMain_CC0
		"cc1.tim",   //CC_ArcMain_CC1
		"cc2.tim",   //CC_ArcMain_CC2
		"cc3.tim",   //CC_ArcMain_CC3
		"cc4.tim",   //CC_ArcMain_CC4
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);

	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
