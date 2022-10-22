/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "cakebear.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Cakebear character structure
enum
{
	Cakebear_ArcMain_Cake0,
	Cakebear_ArcMain_Cake1,
	Cakebear_ArcMain_Cake2,
	Cakebear_ArcMain_Cake3,
	
	Cakebear_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Cakebear_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Cakebear;

//Cakebear character definitions
static const CharFrame char_cakebear_frame[] = {
	{Cakebear_ArcMain_Cake0, {  0,  0,115,104}, { 42,183}}, //0 idle 1
	{Cakebear_ArcMain_Cake0, {116,  0,115,104}, { 42,183}}, //1 idle 2
	{Cakebear_ArcMain_Cake0, {  0,105,115,104}, { 42,183}}, //2 idle 3
	{Cakebear_ArcMain_Cake0, {116,105,115,104}, { 42,183}}, //3 idle 4
	{Cakebear_ArcMain_Cake1, {  0,  0,115,104}, { 42,183}}, //4 idle 5
	
	{Cakebear_ArcMain_Cake1, {116,  0,115,104}, { 42,183}}, //5 left 1
	{Cakebear_ArcMain_Cake1, {  0,105,115,104}, { 42,183}}, //6 left 2
	
	{Cakebear_ArcMain_Cake1, {116,105,115,104}, { 42,183}}, //7 down 1
	{Cakebear_ArcMain_Cake2, {  0,  0,115,104}, { 42,183}}, //8 down 2
	
	{Cakebear_ArcMain_Cake2, {116,  0,115,104}, { 42,183}}, //9 up 1
	{Cakebear_ArcMain_Cake2, {  0,105,115,104}, { 42,183}}, //10 up 2
	
	{Cakebear_ArcMain_Cake2, {116,105,115,104}, { 42,183}}, //11 right 1
	{Cakebear_ArcMain_Cake3, {  0,  0,115,104}, { 42,183}}, //12 right 2
};

static const Animation char_cakebear_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Cakebear character functions
void Char_Cakebear_SetFrame(void *user, u8 frame)
{
	Char_Cakebear *this = (Char_Cakebear*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_cakebear_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Cakebear_Tick(Character *character)
{
	Char_Cakebear *this = (Char_Cakebear*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Cakebear_SetFrame);
	Character_Draw(character, &this->tex, &char_cakebear_frame[this->frame]);
}

void Char_Cakebear_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Cakebear_Free(Character *character)
{
	Char_Cakebear *this = (Char_Cakebear*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Cakebear_New(fixed_t x, fixed_t y)
{
	//Allocate cakebear object
	Char_Cakebear *this = Mem_Alloc(sizeof(Char_Cakebear));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Cakebear_New] Failed to allocate cakebear object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Cakebear_Tick;
	this->character.set_anim = Char_Cakebear_SetAnim;
	this->character.free = Char_Cakebear_Free;
	
	Animatable_Init(&this->character.animatable, char_cakebear_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFFF77601;
	
	this->character.focus_x = FIXED_DEC(47,1);
	this->character.focus_y = FIXED_DEC(-121,1);
	this->character.focus_zoom = FIXED_DEC(241,256);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR2\\CAKEBEAR.ARC;1");
	
	const char **pathp = (const char *[]){
		"cake0.tim", //Cakebear_ArcMain_Cake0
		"cake1.tim", //Cakebear_ArcMain_Cake1
		"cake2.tim", //Cakebear_ArcMain_Cake2
		"cake3.tim", //Cakebear_ArcMain_Cake3
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
