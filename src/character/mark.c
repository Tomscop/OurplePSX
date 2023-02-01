/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "mark.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Mark player types
enum
{
	Mark_ArcMain_Idle,
	Mark_ArcMain_Left,
	Mark_ArcMain_LeftMiss,
	Mark_ArcMain_Down,
	Mark_ArcMain_DownMiss,
	Mark_ArcMain_Up,
	Mark_ArcMain_UpMiss,
	Mark_ArcMain_Right,
	Mark_ArcMain_RightMiss,
	
	Mark_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Mark_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_Mark;

//Mark player definitions
static const CharFrame char_mark_frame[] = 
{
	{Mark_ArcMain_Idle, {  0,  0, 77,143}, { 53, 92}}, //0 idle 1
	{Mark_ArcMain_Idle, { 78,  0, 77,143}, { 53, 92}}, //1 idle 2
	{Mark_ArcMain_Idle, {156,  0, 77,143}, { 53, 92}}, //2 idle 3
	
	{Mark_ArcMain_Left, {  0,  0, 77,143}, { 53, 92}}, //3 left 1
	{Mark_ArcMain_Left, { 78,  0, 77,143}, { 53, 92}}, //4 left 2
	{Mark_ArcMain_Left, {156,  0, 77,143}, { 53, 92}}, //5 left 3
	
	{Mark_ArcMain_Down, {  0,  0, 77,143}, { 53, 92}}, //6 down 1
	{Mark_ArcMain_Down, { 78,  0, 77,143}, { 53, 92}}, //7 down 2
	{Mark_ArcMain_Down, {156,  0, 77,143}, { 53, 92}}, //8 down 3
	
	{Mark_ArcMain_Up, {  0,  0, 77,143}, { 53, 92}}, //9 up 1
	{Mark_ArcMain_Up, { 78,  0, 77,143}, { 53, 92}}, //10 up 2
	{Mark_ArcMain_Up, {156,  0, 77,143}, { 53, 92}}, //11 up 3
	
	{Mark_ArcMain_Right, {  0,  0, 77,143}, { 53, 92}}, //12 right 1
	{Mark_ArcMain_Right, { 78,  0, 77,143}, { 53, 92}}, //13 right 2
	{Mark_ArcMain_Right, {156,  0, 77,143}, { 53, 92}}, //14 right 3
	
	{Mark_ArcMain_LeftMiss, {  0,  0, 77,143}, { 53, 92}}, //15 left miss 1
	{Mark_ArcMain_LeftMiss, { 78,  0, 77,143}, { 53, 92}}, //16 left miss 2
	{Mark_ArcMain_LeftMiss, {156,  0, 77,143}, { 53, 92}}, //17 left miss 3
	
	{Mark_ArcMain_DownMiss, {  0,  0, 77,143}, { 53, 92}}, //18 down miss 1
	{Mark_ArcMain_DownMiss, { 78,  0, 77,143}, { 53, 92}}, //19 down miss 2
	{Mark_ArcMain_DownMiss, {156,  0, 77,143}, { 53, 92}}, //20 down miss 3
	
	{Mark_ArcMain_UpMiss, {  0,  0, 77,143}, { 53, 92}}, //21 up miss 1
	{Mark_ArcMain_UpMiss, { 78,  0, 77,143}, { 53, 92}}, //22 up miss 2
	{Mark_ArcMain_UpMiss, {156,  0, 77,143}, { 53, 92}}, //23 up miss 3
	
	{Mark_ArcMain_RightMiss, {  0,  0, 77,143}, { 53, 92}}, //24 right miss 1
	{Mark_ArcMain_RightMiss, { 78,  0, 77,143}, { 53, 92}}, //25 right miss 2
	{Mark_ArcMain_RightMiss, {156,  0, 77,143}, { 53, 92}}, //26 right miss 3
};
static const Animation char_mark_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 3, 4, 5, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 6, 7, 8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, 11, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){12, 13, 14, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{1, (const u8[]){ 15, 16, 17, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 18, 19, 20, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 21, 22, 23, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 24, 25, 26, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Mark player functions
void Char_Mark_SetFrame(void *user, u8 frame)
{
	Char_Mark *this = (Char_Mark*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		CharFrame *cframe;

 		cframe = &char_mark_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Mark_Tick(Character *character)
{
	Char_Mark *this = (Char_Mark*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_3_2)
	{
		if (stage.song_step == 120)
		{
			this->character.focus_x = FIXED_DEC(-87,1);
			this->character.focus_y = FIXED_DEC(-64,1);
		}
		if (stage.song_step == 166)
		{
			this->character.focus_x = FIXED_DEC(-51,1);
			this->character.focus_y = FIXED_DEC(-64,1);
		}
	}

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
	Animatable_Animate(&character->animatable, (void*)this, Char_Mark_SetFrame);

	Character_Draw(character, &this->tex, &char_mark_frame[this->frame]);
}

void Char_Mark_SetAnim(Character *character, u8 anim)
{
	Char_Mark *this = (Char_Mark*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Mark_Free(Character *character)
{
	Char_Mark *this = (Char_Mark*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Mark_New(fixed_t x, fixed_t y)
{
	//Allocate mark object
	Char_Mark *this = Mem_Alloc(sizeof(Char_Mark));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Mark_New] Failed to allocate mark object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Mark_Tick;
	this->character.set_anim = Char_Mark_SetAnim;
	this->character.free = Char_Mark_Free;
	
	Animatable_Init(&this->character.animatable, char_mark_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 6;

	//health bar color
	this->character.health_bar = 0xFFEE0A5E;

	this->character.focus_x = FIXED_DEC(-51,1);
	this->character.focus_y = FIXED_DEC(-64,1);
	this->character.focus_zoom = FIXED_DEC(897,512);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\MARK.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle.tim",   //Mark_ArcMain_Idle
		"left.tim",   //Mark_ArcMain_Left
		"leftm.tim",   //Mark_ArcMain_LeftMiss
		"down.tim",   //Mark_ArcMain_Down
		"downm.tim",   //Mark_ArcMain_DownMiss
		"up.tim",   //Mark_ArcMain_Up
		"upm.tim",   //Mark_ArcMain_UpMiss
		"right.tim",   //Mark_ArcMain_Right
		"rightm.tim",   //Mark_ArcMain_RightMiss
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);

	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
