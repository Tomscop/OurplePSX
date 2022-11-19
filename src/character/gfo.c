/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gf.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

#include "speaker.h"

//GFO character structure
enum
{
	GFO_ArcMain_GFO0,
	GFO_ArcMain_GFO1,
	GFO_ArcMain_GFO2,
	GFO_ArcMain_GFO3,
	GFO_ArcMain_GFO4,
	GFO_ArcMain_GFO5,
	GFO_ArcMain_GFO6,
	GFO_ArcMain_GFO7,
	
	GFO_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFO_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_GFO;

//GF character definitions
static const CharFrame char_gfo_frame[] = {
	{GFO_ArcMain_GFO0, {  0,  0,136,152}, { 52, 92}}, //0 left 0
	{GFO_ArcMain_GFO1, {  0,  0,136,152}, { 52, 92}}, //1 left 1
	{GFO_ArcMain_GFO2, {  0,  0,136,152}, { 52, 92}}, //2 left 2
	{GFO_ArcMain_GFO3, {  0,  0,136,152}, { 52, 92}}, //3 left 3
	
	{GFO_ArcMain_GFO4, {  0,  0,136,152}, { 52, 92}}, //4 right 0
	{GFO_ArcMain_GFO5, {  0,  0,136,152}, { 52, 92}}, //5 right 1
	{GFO_ArcMain_GFO6, {  0,  0,136,152}, { 52, 92}}, //6 right 2
	{GFO_ArcMain_GFO7, {  0,  0,136,152}, { 52, 92}}, //7 right 3
};

static const CharFrame char_gfo_frame2[] = {
	{GFO_ArcMain_GFO0, {  0,  0, 63,112}, { 52, 92}}, //0 left 0
	{GFO_ArcMain_GFO0, { 64,  0, 63,112}, { 52, 92}}, //1 left 1
	{GFO_ArcMain_GFO0, {128,  0, 63,112}, { 52, 92}}, //2 left 2
	{GFO_ArcMain_GFO0, {192,  0, 63,112}, { 52, 92}}, //3 left 3
	
	{GFO_ArcMain_GFO0, {  0,113, 63,112}, { 52, 92}}, //4 right 0
	{GFO_ArcMain_GFO0, { 64,113, 63,112}, { 52, 92}}, //5 right 1
	{GFO_ArcMain_GFO0, {128,113, 63,112}, { 52, 92}}, //6 right 2
	{GFO_ArcMain_GFO0, {192,113, 63,112}, { 52, 92}}, //7 right 3
};

static const Animation char_gfo_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_Idle
	{1, (const u8[]){ 0,  0,  1,  1,  1,  2,  2,  2,  2,  3, ASCR_BACK, 1}}, //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_UpAlt
	{1, (const u8[]){ 4,  4,  5,  5,  5,  6,  6,  6,  6,  7, ASCR_BACK, 1}}, //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_RightAlt
};

//GFO character functions
void Char_GFO_SetFrame(void *user, u8 frame)
{
	Char_GFO *this = (Char_GFO*)user;
	
	//Check if this is a new frame
	CharFrame *cframe;
	
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		if (stage.stage_id == StageId_1_2 || stage.stage_id == StageId_1_4 || stage.stage_id == StageId_5_1)
			cframe = &char_gfo_frame2[this->frame = frame];
		else
			cframe = &char_gfo_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFO_Tick(Character *character)
{
	Char_GFO *this = (Char_GFO*)character;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform dance
		if ((stage.song_step % stage.gf_speed) == 0)
		{
			//Switch animation
			if (character->animatable.anim == CharAnim_Left)
				character->set_anim(character, CharAnim_Right);
			else
				character->set_anim(character, CharAnim_Left);
		}
	}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFO_SetFrame);
	if (stage.stage_id == StageId_1_2 || stage.stage_id == StageId_1_4 || stage.stage_id == StageId_5_1)
	{
		Character_Draw(character, &this->tex, &char_gfo_frame2[this->frame]);
	}
	else
	{
		if (stage.stage_id == StageId_1_3 && stage.song_step <= 270 || stage.stage_id == StageId_1_3 && stage.song_step >= 1680 && stage.song_step <= 2200)
			Character_Draw(character, &this->tex, &char_gfo_frame[this->frame]);
		else if (stage.stage_id != StageId_1_3)
			Character_Draw(character, &this->tex, &char_gfo_frame[this->frame]);
	}
}

void Char_GFO_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim != CharAnim_Idle && anim != CharAnim_Left && anim != CharAnim_Right)
		return;
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFO_Free(Character *character)
{
	Char_GFO *this = (Char_GFO*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GFO_New(fixed_t x, fixed_t y)
{
	//Allocate gfo object
	Char_GFO *this = Mem_Alloc(sizeof(Char_GFO));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFO_New] Failed to allocate gfo object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFO_Tick;
	this->character.set_anim = Char_GFO_SetAnim;
	this->character.free = Char_GFO_Free;
	
	Animatable_Init(&this->character.animatable, char_gfo_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	this->character.idle2 = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(16,1);
	this->character.focus_y = FIXED_DEC(-50,1);
	this->character.focus_zoom = FIXED_DEC(13,10);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	if (stage.stage_id == StageId_1_2 || stage.stage_id == StageId_1_4 || stage.stage_id == StageId_5_1)
	{
		this->arc_main = IO_Read("\\CHAR\\GFOS.ARC;1");
	
		const char **pathp = (const char *[]){
			"gfos.tim",  //GFO_ArcMain_GFO0
			NULL
		};
		IO_Data *arc_ptr = this->arc_ptr;
		for (; *pathp != NULL; pathp++)
			*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	}
	else
	{
		this->arc_main = IO_Read("\\CHAR\\GFO.ARC;1");
	
		const char **pathp = (const char *[]){
			"gfo0.tim",  //GFO_ArcMain_GFO0
			"gfo1.tim",  //GFO_ArcMain_GFO1
			"gfo2.tim",  //GFO_ArcMain_GFO2
			"gfo3.tim",  //GFO_ArcMain_GFO3
			"gfo4.tim",  //GFO_ArcMain_GFO4
			"gfo5.tim",  //GFO_ArcMain_GFO5
			"gfo6.tim",  //GFO_ArcMain_GFO6
			"gfo7.tim",  //GFO_ArcMain_GFO7
			NULL
		};
		IO_Data *arc_ptr = this->arc_ptr;
		for (; *pathp != NULL; pathp++)
			*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	}
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
