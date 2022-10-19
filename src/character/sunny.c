/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "sunny.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Sunny character structure
enum
{
	Sunny_ArcMain_Idle0,
	Sunny_ArcMain_Idle1,
	Sunny_ArcMain_Idle2,
	Sunny_ArcMain_Idle3,
	Sunny_ArcMain_Idle4,
	Sunny_ArcMain_Left0,
	Sunny_ArcMain_Left1,
	Sunny_ArcMain_Left2,
	Sunny_ArcMain_Down0,
	Sunny_ArcMain_Down1,
	Sunny_ArcMain_Down2,
	Sunny_ArcMain_Up0,
	Sunny_ArcMain_Up1,
	Sunny_ArcMain_Up2,
	Sunny_ArcMain_Right0,
	Sunny_ArcMain_Right1,
	Sunny_ArcMain_Right2,
	
	Sunny_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Sunny_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Sunny;

//Sunny character definitions
static const CharFrame char_sunny_frame[] = {
	{Sunny_ArcMain_Idle0, {  0,  0,255,255}, { 42,183}}, //0 idle 1
	{Sunny_ArcMain_Idle1, {  0,  0,255,255}, { 42,183}}, //1 idle 2
	{Sunny_ArcMain_Idle2, {  0,  0,255,255}, { 42,183}}, //2 idle 3
	{Sunny_ArcMain_Idle3, {  0,  0,255,255}, { 42,183}}, //3 idle 4
	{Sunny_ArcMain_Idle4, {  0,  0,255,255}, { 42,183}}, //4 idle 5
	
	{Sunny_ArcMain_Left0, {  0,  0,255,255}, { 61,183}}, //5 left 1
	{Sunny_ArcMain_Left1, {  0,  0,255,255}, { 61,183}}, //6 left 2
	{Sunny_ArcMain_Left2, {  0,  0,255,255}, { 61,183}}, //7 left 3
	
	{Sunny_ArcMain_Down0, {  0,  0,255,255}, { 52, 76}}, //8 down 1
	{Sunny_ArcMain_Down1, {  0,  0,255,255}, { 52, 76}}, //9 down 2
	{Sunny_ArcMain_Down2, {  0,  0,255,255}, { 52, 76}}, //10 down 3
	
	{Sunny_ArcMain_Up0, {  0,  0,255,255}, { 42,211}}, //11 up 1
	{Sunny_ArcMain_Up1, {  0,  0,255,255}, { 42,211}}, //12 up 2
	{Sunny_ArcMain_Up2, {  0,  0,255,255}, { 42,191}}, //13 up 3
	
	{Sunny_ArcMain_Right0, {  0,  0,255,255}, { 42,183}}, //14 right 1
	{Sunny_ArcMain_Right1, {  0,  0,255,255}, { 42,183}}, //15 right 2
	{Sunny_ArcMain_Right2, {  0,  0,255,255}, { 42,183}}, //16 right 3
};

static const Animation char_sunny_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{3, (const u8[]){ 5, 6, 7, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{3, (const u8[]){ 8, 9, 10, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{3, (const u8[]){ 11, 12, 13, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{3, (const u8[]){ 14, 15, 16, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Sunny character functions
void Char_Sunny_SetFrame(void *user, u8 frame)
{
	Char_Sunny *this = (Char_Sunny*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_sunny_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Sunny_Tick(Character *character)
{
	Char_Sunny *this = (Char_Sunny*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Sunny_SetFrame);
	Character_Draw(character, &this->tex, &char_sunny_frame[this->frame]);
}

void Char_Sunny_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Sunny_Free(Character *character)
{
	Char_Sunny *this = (Char_Sunny*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Sunny_New(fixed_t x, fixed_t y)
{
	//Allocate sunny object
	Char_Sunny *this = Mem_Alloc(sizeof(Char_Sunny));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Sunny_New] Failed to allocate sunny object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Sunny_Tick;
	this->character.set_anim = Char_Sunny_SetAnim;
	this->character.free = Char_Sunny_Free;
	
	Animatable_Init(&this->character.animatable, char_sunny_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 6;

	//health bar color
	this->character.health_bar = 0xFF3D003C;
	
	this->character.focus_x = FIXED_DEC(97,1);
	this->character.focus_y = FIXED_DEC(-57,1);
	this->character.focus_zoom = FIXED_DEC(477,512);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SUNNY.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Sunny_ArcMain_Idle0
		"idle1.tim", //Sunny_ArcMain_Idle1
		"idle2.tim", //Sunny_ArcMain_Idle2
		"idle3.tim", //Sunny_ArcMain_Idle3
		"idle4.tim", //Sunny_ArcMain_Idle4
		"left0.tim", //Sunny_ArcMain_Left0
		"left1.tim", //Sunny_ArcMain_Left1
		"left2.tim", //Sunny_ArcMain_Left2
		"down0.tim", //Sunny_ArcMain_Down0
		"down1.tim", //Sunny_ArcMain_Down1
		"down2.tim", //Sunny_ArcMain_Down2
		"up0.tim", //Sunny_ArcMain_Up0
		"up1.tim", //Sunny_ArcMain_Up1
		"up2.tim", //Sunny_ArcMain_Up2
		"right0.tim", //Sunny_ArcMain_Right0
		"right1.tim", //Sunny_ArcMain_Right1
		"right2.tim", //Sunny_ArcMain_Right2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
