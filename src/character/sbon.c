/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "sbon.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//SBon character structure
enum
{
	SBon_ArcMain_Idle0,
	SBon_ArcMain_Idle1,
	SBon_ArcMain_Idle2,
	SBon_ArcMain_Idle3,
	SBon_ArcMain_Idle4,
	SBon_ArcMain_Left0,
	SBon_ArcMain_Left1,
	SBon_ArcMain_Left2,
	SBon_ArcMain_Down0,
	SBon_ArcMain_Down1,
	SBon_ArcMain_Down2,
	SBon_ArcMain_Up0,
	SBon_ArcMain_Up1,
	SBon_ArcMain_Up2,
	SBon_ArcMain_Right0,
	SBon_ArcMain_Right1,
	SBon_ArcMain_Right2,
	
	SBon_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[SBon_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_SBon;

//SBon character definitions
static const CharFrame char_sbon_frame[] = {
	{SBon_ArcMain_Idle0, {  0,  0,255,255}, { 42,183}}, //0 idle 1
	{SBon_ArcMain_Idle1, {  0,  0,255,255}, { 42,183}}, //1 idle 2
	{SBon_ArcMain_Idle2, {  0,  0,255,255}, { 42,183}}, //2 idle 3
	{SBon_ArcMain_Idle3, {  0,  0,255,255}, { 42,183}}, //3 idle 4
	{SBon_ArcMain_Idle4, {  0,  0,255,255}, { 42,183}}, //4 idle 5
	
	{SBon_ArcMain_Left0, {  0,  0,255,255}, { 61,183}}, //5 left 1
	{SBon_ArcMain_Left1, {  0,  0,255,255}, { 61,183}}, //6 left 2
	{SBon_ArcMain_Left2, {  0,  0,255,255}, { 61,183}}, //7 left 3
	
	{SBon_ArcMain_Down0, {  0,  0,255,255}, { 52, 76}}, //8 down 1
	{SBon_ArcMain_Down1, {  0,  0,255,255}, { 52, 76}}, //9 down 2
	{SBon_ArcMain_Down2, {  0,  0,255,255}, { 52, 76}}, //10 down 3
	
	{SBon_ArcMain_Up0, {  0,  0,255,255}, { 42,211}}, //11 up 1
	{SBon_ArcMain_Up1, {  0,  0,255,255}, { 42,211}}, //12 up 2
	{SBon_ArcMain_Up2, {  0,  0,255,255}, { 42,191}}, //13 up 3
	
	{SBon_ArcMain_Right0, {  0,  0,255,255}, { 42,183}}, //14 right 1
	{SBon_ArcMain_Right1, {  0,  0,255,255}, { 42,183}}, //15 right 2
	{SBon_ArcMain_Right2, {  0,  0,255,255}, { 42,183}}, //16 right 3
};

static const Animation char_sbon_anim[CharAnim_Max] = {
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

//SBon character functions
void Char_SBon_SetFrame(void *user, u8 frame)
{
	Char_SBon *this = (Char_SBon*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_sbon_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_SBon_Tick(Character *character)
{
	Char_SBon *this = (Char_SBon*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_SBon_SetFrame);
	Character_Draw(character, &this->tex, &char_sbon_frame[this->frame]);
}

void Char_SBon_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_SBon_Free(Character *character)
{
	Char_SBon *this = (Char_SBon*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_SBon_New(fixed_t x, fixed_t y)
{
	//Allocate sbon object
	Char_SBon *this = Mem_Alloc(sizeof(Char_SBon));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_SBon_New] Failed to allocate sbon object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_SBon_Tick;
	this->character.set_anim = Char_SBon_SetAnim;
	this->character.free = Char_SBon_Free;
	
	Animatable_Init(&this->character.animatable, char_sbon_anim);
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
	this->arc_main = IO_Read("\\CHAR\\SBON.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //SBon_ArcMain_Idle0
		"idle1.tim", //SBon_ArcMain_Idle1
		"idle2.tim", //SBon_ArcMain_Idle2
		"idle3.tim", //SBon_ArcMain_Idle3
		"idle4.tim", //SBon_ArcMain_Idle4
		"left0.tim", //SBon_ArcMain_Left0
		"left1.tim", //SBon_ArcMain_Left1
		"left2.tim", //SBon_ArcMain_Left2
		"down0.tim", //SBon_ArcMain_Down0
		"down1.tim", //SBon_ArcMain_Down1
		"down2.tim", //SBon_ArcMain_Down2
		"up0.tim", //SBon_ArcMain_Up0
		"up1.tim", //SBon_ArcMain_Up1
		"up2.tim", //SBon_ArcMain_Up2
		"right0.tim", //SBon_ArcMain_Right0
		"right1.tim", //SBon_ArcMain_Right1
		"right2.tim", //SBon_ArcMain_Right2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
