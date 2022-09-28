/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfp.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//BFP player types
enum
{
	BFP_ArcMain_BFP0,
	
	BFP_ArcMain_Max,
};

#define BFP_Arc_Max BFP_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFP_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BFP;

//BFP player definitions
static const CharFrame char_bfp_frame[] = 
{
	{BFP_ArcMain_BFP0, {  0,  0, 44, 51}, { 53, 92}}, //0 idle 1
	{BFP_ArcMain_BFP0, { 45,  0, 44, 51}, { 53, 92}}, //1 idle 2
	
	{BFP_ArcMain_BFP0, { 90,  0, 44, 51}, { 53, 92}}, //2 left 1
	{BFP_ArcMain_BFP0, {135,  0, 44, 51}, { 53, 92}}, //3 left 2
	
	{BFP_ArcMain_BFP0, {  0,104, 44, 51}, { 53, 92}}, //4 left miss 1
	{BFP_ArcMain_BFP0, { 45,104, 44, 51}, { 53, 92}}, //5 left miss 2
	
	{BFP_ArcMain_BFP0, {180,  0, 44, 51}, { 53, 92}}, //6 down 1
	{BFP_ArcMain_BFP0, {  0, 52, 44, 51}, { 53, 92}}, //7 down 2
	
	{BFP_ArcMain_BFP0, { 90,104, 44, 51}, { 53, 92}}, //8 down miss 1
	{BFP_ArcMain_BFP0, {135,104, 44, 51}, { 53, 92}}, //9 down miss 2
	
	{BFP_ArcMain_BFP0, { 45, 52, 44, 51}, { 53, 92}}, //10 up 1
	{BFP_ArcMain_BFP0, { 90, 52, 44, 51}, { 53, 92}}, //11 up 2
	
	{BFP_ArcMain_BFP0, {180,104, 44, 51}, { 53, 92}}, //12 up miss 1
	{BFP_ArcMain_BFP0, {  0,156, 44, 51}, { 53, 92}}, //13 up miss 2
	
	{BFP_ArcMain_BFP0, {135, 52, 44, 51}, { 53, 92}}, //14 right 1
	{BFP_ArcMain_BFP0, {180, 52, 44, 51}, { 53, 92}}, //15 right 2
	
	{BFP_ArcMain_BFP0, { 45,156, 44, 51}, { 53, 92}}, //16 right miss 1
	{BFP_ArcMain_BFP0, { 90,156, 44, 51}, { 53, 92}}, //17 right miss 2
};
static const Animation char_bfp_anim[PlayerAnim_Max] = {
	{8, (const u8[]){ 0, 1, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{2, (const u8[]){ 2,  3, ASCR_CHGANI, CharAnim_Left}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 6,  7, ASCR_CHGANI, CharAnim_Down}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){10, 11, ASCR_CHGANI, CharAnim_Up}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){14, 15, ASCR_CHGANI, CharAnim_Right}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{2, (const u8[]){ 2, 4, 5, ASCR_CHGANI, PlayerAnim_LeftMiss}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 6, 8, 9, ASCR_CHGANI, PlayerAnim_DownMiss}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 10, 12, 13, ASCR_CHGANI, PlayerAnim_UpMiss}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 14, 16, 17, ASCR_CHGANI, PlayerAnim_RightMiss}},     //PlayerAnim_RightMiss
};

//BFP player functions
void Char_BFP_SetFrame(void *user, u8 frame)
{
	Char_BFP *this = (Char_BFP*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		CharFrame *cframe;

 			cframe = &char_bfp_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFP_Tick(Character *character)
{
	Char_BFP *this = (Char_BFP*)character;

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
		     character->animatable.anim != PlayerAnim_LeftMiss &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
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

	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BFP_SetFrame);
	if (stage.song_step >= 912 && stage.song_step <= 1167 || stage.song_step >= 1423 && stage.song_step <= 1680)
		Character_Draw(character, &this->tex, &char_bfp_frame[this->frame]);
}

void Char_BFP_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFP_Free(Character *character)
{
	Char_BFP *this = (Char_BFP*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFP_New(fixed_t x, fixed_t y)
{
	//Allocate bfp object
	Char_BFP *this = Mem_Alloc(sizeof(Char_BFP));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFP_New] Failed to allocate bfp object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFP_Tick;
	this->character.set_anim = Char_BFP_SetAnim;
	this->character.free = Char_BFP_Free;
	Animatable_Init(&this->character.animatable, char_bfp_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFF7CD6F5;
	
	this->character.focus_x = FIXED_DEC(-1,1);
	this->character.focus_y = FIXED_DEC(-5,1);
	this->character.focus_zoom = FIXED_DEC(597,512);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BFP.ARC;1");
	
	const char **pathp = (const char *[]){
		"bfp0.tim",   //BFP0_ArcMain_BFP0
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}