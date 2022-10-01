/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gfreddy.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//GFreddy character structure
enum
{
	GFreddy_ArcMain_Idle,
	GFreddy_ArcMain_Left0,
	GFreddy_ArcMain_Left1,
	GFreddy_ArcMain_Down0,
	GFreddy_ArcMain_Down1,
	GFreddy_ArcMain_Up0,
	GFreddy_ArcMain_Up1,
	GFreddy_ArcMain_Right0,
	GFreddy_ArcMain_Right1,
	
	GFreddy_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFreddy_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_GFreddy;

//GFreddy character definitions
static const CharFrame char_gfreddy_frame[] = {
	{GFreddy_ArcMain_Idle, {  0,  0,122,180}, { 42,183+4}}, //0 idle 1
	
	{GFreddy_ArcMain_Left0, {  0,  0,122,180}, { 42,183+4}}, //1 left 1
	{GFreddy_ArcMain_Left0, {123,  0,122,180}, { 42,183+4}}, //2 left 2
	{GFreddy_ArcMain_Left1, {  0,  0,122,180}, { 42,183+4}}, //3 left 3
	
	{GFreddy_ArcMain_Down0, {  0,  0,122,180}, { 42,183+4}}, //4 down 1
	{GFreddy_ArcMain_Down0, {123,  0,122,180}, { 42,183+4}}, //5 down 2
	{GFreddy_ArcMain_Down1, {  0,  0,122,180}, { 42,183+4}}, //6 down 3
	
	{GFreddy_ArcMain_Up0, {  0,  0,122,180}, { 42,183+4}}, //7 up 1
	{GFreddy_ArcMain_Up0, {123,  0,122,180}, { 42,183+4}}, //8 up 2
	{GFreddy_ArcMain_Up1, {  0,  0,122,180}, { 42,183+4}}, //9 up 3
	
	{GFreddy_ArcMain_Right0, {  0,  0,122,180}, { 42,183+4}}, //10 right 1
	{GFreddy_ArcMain_Right0, {123,  0,122,180}, { 42,183+4}}, //11 right 2
	{GFreddy_ArcMain_Right1, {  0,  0,122,180}, { 42,183+4}}, //12 right 3
};

static const Animation char_gfreddy_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 1, 2, 3, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 4, 5, 6, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 7, 8, 9, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 10, 11, 12, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//GFreddy character functions
void Char_GFreddy_SetFrame(void *user, u8 frame)
{
	Char_GFreddy *this = (Char_GFreddy*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfreddy_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFreddy_Tick(Character *character)
{
	Char_GFreddy *this = (Char_GFreddy*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFreddy_SetFrame);
	if (stage.song_step >= 496 && stage.song_step <= 1036)
		Character_Draw(character, &this->tex, &char_gfreddy_frame[this->frame]);
}

void Char_GFreddy_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_GFreddy_Free(Character *character)
{
	Char_GFreddy *this = (Char_GFreddy*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GFreddy_New(fixed_t x, fixed_t y)
{
	//Allocate gfreddy object
	Char_GFreddy *this = Mem_Alloc(sizeof(Char_GFreddy));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFreddy_New] Failed to allocate gfreddy object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFreddy_Tick;
	this->character.set_anim = Char_GFreddy_SetAnim;
	this->character.free = Char_GFreddy_Free;
	
	Animatable_Init(&this->character.animatable, char_gfreddy_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 8;

	//health bar color
	this->character.health_bar = 0xFF663810;
	
	this->character.focus_x = FIXED_DEC(47,1);
	this->character.focus_y = FIXED_DEC(-104,1);
	this->character.focus_zoom = FIXED_DEC(1205,512);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GFREDDY.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle.tim", //GFreddy_ArcMain_Idle
		"left0.tim",  //GFreddy_ArcMain_Left0
		"left1.tim",  //GFreddy_ArcMain_Left1
		"down0.tim",  //GFreddy_ArcMain_Down0
		"down1.tim",  //GFreddy_ArcMain_Down1
		"up0.tim",    //GFreddy_ArcMain_Up0
		"up1.tim",    //GFreddy_ArcMain_Up1
		"right0.tim", //GFreddy_ArcMain_Right0
		"right1.tim", //GFreddy_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
