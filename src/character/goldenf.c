/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "goldenf.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//GoldenF character structure
enum
{
	GoldenF_ArcMain_GoldenF0,
	GoldenF_ArcMain_GoldenF1,
	
	GoldenF_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GoldenF_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_GoldenF;

//Golden Phone character definitions
static const CharFrame char_goldenf_frame[] = {
	{GoldenF_ArcMain_GoldenF0, {  0,  0,116,105}, { 35,52}}, //0 idle 1
	{GoldenF_ArcMain_GoldenF0, {117,  0,116,105}, { 35,52}}, //1 idle 2
	{GoldenF_ArcMain_GoldenF0, {  0,106,116,105}, { 35,52}}, //2 idle 3
	{GoldenF_ArcMain_GoldenF0, {117,106,116,105}, { 35,52}}, //3 idle 4
	
	{GoldenF_ArcMain_GoldenF1, {117,106,116,105}, { 35,52}}, //4 left 1
	
	{GoldenF_ArcMain_GoldenF1, {117,  0,116,105}, { 35,52}}, //5 down 1

	{GoldenF_ArcMain_GoldenF1, {  0,106,116,105}, { 35,52}}, //6 up 1
	
	{GoldenF_ArcMain_GoldenF1, {  0,  0,116,105}, { 35,52}}, //7 right 1
};

static const Animation char_goldenf_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, ASCR_BACK, 1}},             //CharAnim_Idle
	{1, (const u8[]){ 4, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 5, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 6, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 7, ASCR_BACK, 1}},          //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Golden Phone character functions
void Char_GoldenF_SetFrame(void *user, u8 frame)
{
	Char_GoldenF *this = (Char_GoldenF*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_goldenf_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GoldenF_Tick(Character *character)
{
	Char_GoldenF *this = (Char_GoldenF*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GoldenF_SetFrame);
	if (stage.stage_id == StageId_2_3 && stage.song_step >= 1336 && stage.song_step <= 1401)
		Character_DrawFlipped(character, &this->tex, &char_goldenf_frame[this->frame]);
}

void Char_GoldenF_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_GoldenF_Free(Character *character)
{
	Char_GoldenF *this = (Char_GoldenF*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GoldenF_New(fixed_t x, fixed_t y)
{
	//Allocate goldenf object
	Char_GoldenF *this = Mem_Alloc(sizeof(Char_GoldenF));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GoldenF_New] Failed to allocate goldenf object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GoldenF_Tick;
	this->character.set_anim = Char_GoldenF_SetAnim;
	this->character.free = Char_GoldenF_Free;
	
	Animatable_Init(&this->character.animatable, char_goldenf_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 8;

	//health bar color
	this->character.health_bar = 0xFFAD63D6;
	
	if (stage.stage_id == StageId_2_3)
	{	
		this->character.focus_x = FIXED_DEC(-21,1);
		this->character.focus_y = FIXED_DEC(-28,1);
		this->character.focus_zoom = FIXED_DEC(597,512);
	}
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GOLDENF.ARC;1");
	
	const char **pathp = (const char *[]){
		"goldenf0.tim", //GoldenF_ArcMain_GoldenF0
		"goldenf1.tim", //GoldenF_ArcMain_GoldenF1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
