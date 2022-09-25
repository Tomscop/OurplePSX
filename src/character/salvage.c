/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "salvage.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Salvage character structure
enum
{
	Salvage_ArcMain_Idle0,
	Salvage_ArcMain_Idle1,
	Salvage_ArcMain_Idle2,
	Salvage_ArcMain_Left0,
	Salvage_ArcMain_Left1,
	Salvage_ArcMain_Left2,
	Salvage_ArcMain_Down0,
	Salvage_ArcMain_Down1,
	Salvage_ArcMain_Down2,
	Salvage_ArcMain_Up0,
	Salvage_ArcMain_Up1,
	Salvage_ArcMain_Up2,
	Salvage_ArcMain_Right0,
	Salvage_ArcMain_Right1,
	Salvage_ArcMain_Right2,
	
	Salvage_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Salvage_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Salvage;

//Salvage character definitions
static const CharFrame char_salvage_frame[] = {
	{Salvage_ArcMain_Idle0, {  0,  0,195,253}, { 42,183+4}}, //0 idle 1
	{Salvage_ArcMain_Idle1, {  0,  0,195,253}, { 42,183+4}}, //1 idle 2
	{Salvage_ArcMain_Idle2, {  0,  0,195,253}, { 42,183+4}}, //2 idle 3
	
	{Salvage_ArcMain_Left0, {  0,  0,195,253}, { 42,183+4}}, //3 left 1
	{Salvage_ArcMain_Left1, {  0,  0,195,253}, { 42,183+4}}, //4 left 2
	{Salvage_ArcMain_Left2, {  0,  0,195,253}, { 42,183+4}}, //5 left 3
	
	{Salvage_ArcMain_Down0, {  0,  0,195,253}, { 42,183+4}}, //6 down 1
	{Salvage_ArcMain_Down1, {  0,  0,195,253}, { 42,183+4}}, //7 down 2
	{Salvage_ArcMain_Down2, {  0,  0,195,253}, { 42,183+4}}, //8 down 3
	
	{Salvage_ArcMain_Up0, {  0,  0,195,253}, { 42,183+4}}, //9 up 1
	{Salvage_ArcMain_Up1, {  0,  0,195,253}, { 42,183+4}}, //10 up 2
	{Salvage_ArcMain_Up2, {  0,  0,195,253}, { 42,183+4}}, //11 up 3
	
	{Salvage_ArcMain_Right0, {  0,  0,195,253}, { 42,183+4}}, //12 right 1
	{Salvage_ArcMain_Right1, {  0,  0,195,253}, { 42,183+4}}, //13 right 2
	{Salvage_ArcMain_Right2, {  0,  0,195,253}, { 42,183+4}}, //14 right 3
};

static const Animation char_salvage_anim[CharAnim_Max] = {
	{2, (const u8[]){ 2, 1, 0, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 3, 4,  5, ASCR_BACK, 1}},         //CharAnim_Left
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 6,  7, 8, ASCR_BACK, 1}},         //CharAnim_Down
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, 11, ASCR_BACK, 1}},         //CharAnim_Up
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 12, 13, 14, ASCR_BACK, 1}},         //CharAnim_Right
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Salvage character functions
void Char_Salvage_SetFrame(void *user, u8 frame)
{
	Char_Salvage *this = (Char_Salvage*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_salvage_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Salvage_Tick(Character *character)
{
	Char_Salvage *this = (Char_Salvage*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Salvage_SetFrame);
	Character_Draw(character, &this->tex, &char_salvage_frame[this->frame]);
}

void Char_Salvage_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Salvage_Free(Character *character)
{
	Char_Salvage *this = (Char_Salvage*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Salvage_New(fixed_t x, fixed_t y)
{
	//Allocate salvage object
	Char_Salvage *this = Mem_Alloc(sizeof(Char_Salvage));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Salvage_New] Failed to allocate salvage object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Salvage_Tick;
	this->character.set_anim = Char_Salvage_SetAnim;
	this->character.free = Char_Salvage_Free;
	
	Animatable_Init(&this->character.animatable, char_salvage_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 9;

	//health bar color
	this->character.health_bar = 0xFFFFCB05;
	
	this->character.focus_x = FIXED_DEC(92,1);
	this->character.focus_y = FIXED_DEC(-86,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SALVAGE.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Salvage_ArcMain_Idle0
		"idle1.tim", //Salvage_ArcMain_Idle1
		"idle2.tim", //Salvage_ArcMain_Idle2
		"left0.tim", //Salvage_ArcMain_Left0
		"left1.tim", //Salvage_ArcMain_Left1
		"left2.tim", //Salvage_ArcMain_Left2
		"down0.tim", //Salvage_ArcMain_Down0
		"down1.tim", //Salvage_ArcMain_Down1
		"down2.tim", //Salvage_ArcMain_Down2
		"up0.tim", //Salvage_ArcMain_Up0
		"up1.tim", //Salvage_ArcMain_Up1
		"up2.tim", //Salvage_ArcMain_Up2
		"right0.tim", //Salvage_ArcMain_Right0
		"right1.tim", //Salvage_ArcMain_Right1
		"right2.tim", //Salvage_ArcMain_Right2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
