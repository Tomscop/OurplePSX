/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ngt.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//NGT character structure
enum
{
	NGT_ArcMain_Idle0,
	NGT_ArcMain_Idle1,
	NGT_ArcMain_Idle2,
	NGT_ArcMain_Left0,
	NGT_ArcMain_Left1,
	NGT_ArcMain_Down0,
	NGT_ArcMain_Down1,
	NGT_ArcMain_Up0,
	NGT_ArcMain_Up1,
	NGT_ArcMain_Right0,
	NGT_ArcMain_Right1,
	NGT_ArcMain_Turn0,
	NGT_ArcMain_Turn1,
	NGT_ArcMain_Turn2,
	NGT_ArcMain_Turn3,
	NGT_ArcMain_Turn4,
	NGT_ArcMain_Alt0,
	NGT_ArcMain_Alt1,
	
	NGT_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[NGT_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_NGT;

//NGT character definitions
static const CharFrame char_ngt_frame[] = {
	{NGT_ArcMain_Idle0, {  0,  0,131,150}, { 42,183}}, //0 idle 1
	{NGT_ArcMain_Idle1, {  0,  0,131,150}, { 42,183}}, //1 idle 2
	{NGT_ArcMain_Idle2, {  0,  0,131,150}, { 42,183}}, //2 idle 3
	
	{NGT_ArcMain_Left0, {  0,  0,131,150}, { 42,183}}, //3 left 1
	{NGT_ArcMain_Left1, {  0,  0,131,150}, { 42,183}}, //4 left 2
	
	{NGT_ArcMain_Down0, {  0,  0,131,150}, { 42,183}}, //5 down 1
	{NGT_ArcMain_Down1, {  0,  0,131,150}, { 42,183}}, //6 down 2
	
	{NGT_ArcMain_Up0, {  0,  0,131,150}, { 42,183}}, //7 up 1
	{NGT_ArcMain_Up1, {  0,  0,131,150}, { 42,183}}, //8 up 2
	
	{NGT_ArcMain_Right0, {  0,  0,131,150}, { 42,183}}, //9 right 1
	{NGT_ArcMain_Right1, {  0,  0,131,150}, { 42,183}}, //10 right 2
	
	{NGT_ArcMain_Turn0, {  0,  0,131,150}, { 42,183}}, //11 turn 1
	{NGT_ArcMain_Turn1, {  0,  0,131,150}, { 42,183}}, //12 turn 2
	{NGT_ArcMain_Turn2, {  0,  0,131,150}, { 42,183}}, //13 turn 3
	{NGT_ArcMain_Turn3, {  0,  0,131,150}, { 42,183}}, //14 turn 4
	{NGT_ArcMain_Turn4, {  0,  0,131,150}, { 42,183}}, //15 turn 5
	
	{NGT_ArcMain_Alt0, {  0,  0,131,150}, { 42,183}}, //16 alt 1
	{NGT_ArcMain_Alt1, {  0,  0,131,150}, { 42,183}}, //17 alt 2
};

static const Animation char_ngt_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 3, 4, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 5, 6, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 7, 8, ASCR_BACK, 1}},         //CharAnim_Up
	{4, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},         //CharAnim_Right
	{4, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

static const Animation char_ngt_anim2[CharAnim_Max] = {
	{4, (const u8[]){11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, ASCR_CHGANI, CharAnim_LeftAlt}}, //CharAnim_Idle
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Left
	{0, (const u8[]){ 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, ASCR_CHGANI, CharAnim_LeftAlt}},   //CharAnim_LeftAlt
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

static const Animation char_ngt_anim3[CharAnim_Max] = {
	{2, (const u8[]){ 15, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 16, 17, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 16, 17, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 16, 17, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 16, 17, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//NGT character functions
void Char_NGT_SetFrame(void *user, u8 frame)
{
	Char_NGT *this = (Char_NGT*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_ngt_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_NGT_Tick(Character *character)
{
	Char_NGT *this = (Char_NGT*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
			Character_PerformIdle(character);
	
	if (stage.song_step >= 1024)
		Animatable_Init(&this->character.animatable, char_ngt_anim2);
	if (stage.song_step >= 1032)
		Animatable_Init(&this->character.animatable, char_ngt_anim3);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_NGT_SetFrame);
	Character_Draw(character, &this->tex, &char_ngt_frame[this->frame]);
}

void Char_NGT_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_NGT_Free(Character *character)
{
	Char_NGT *this = (Char_NGT*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_NGT_New(fixed_t x, fixed_t y)
{
	//Allocate ngt object
	Char_NGT *this = Mem_Alloc(sizeof(Char_NGT));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_NGT_New] Failed to allocate ngt object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_NGT_Tick;
	this->character.set_anim = Char_NGT_SetAnim;
	this->character.free = Char_NGT_Free;
	
	Animatable_Init(&this->character.animatable, char_ngt_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 4;

	//health bar color
	this->character.health_bar = 0xFFFDF22E;
	
	this->character.focus_x = FIXED_DEC(90,1);
	this->character.focus_y = FIXED_DEC(-124,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR2\\NGT.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //NGT_ArcMain_Idle0
		"idle1.tim", //NGT_ArcMain_Idle1
		"idle2.tim", //NGT_ArcMain_Idle2
		"left0.tim", //NGT_ArcMain_Left0
		"left1.tim", //NGT_ArcMain_Left1
		"down0.tim", //NGT_ArcMain_Down0
		"down1.tim", //NGT_ArcMain_Down1
		"up0.tim", //NGT_ArcMain_Up0
		"up1.tim", //NGT_ArcMain_Up1
		"right0.tim", //NGT_ArcMain_Right0
		"right1.tim", //NGT_ArcMain_Right1
		"turn0.tim", //NGT_ArcMain_Turn0
		"turn1.tim", //NGT_ArcMain_Turn1
		"turn2.tim", //NGT_ArcMain_Turn2
		"turn3.tim", //NGT_ArcMain_Turn3
		"turn4.tim", //NGT_ArcMain_Turn4
		"alt0.tim", //NGT_ArcMain_Alt0
		"alt1.tim", //NGT_ArcMain_Alt1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
