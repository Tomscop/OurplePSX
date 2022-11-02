/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "henrym.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//HenryM character structure
enum
{
	HenryM_ArcMain_Henry0,
	HenryM_ArcMain_Henry1,
	HenryM_ArcMain_Henry2,
	HenryM_ArcMain_Henry3,
	HenryM_ArcMain_Henry4,
	HenryM_ArcMain_Henry5,
	
	HenryM_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[HenryM_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_HenryM;

//HenryM character definitions
static const CharFrame char_henrym_frame[] = {
	{HenryM_ArcMain_Henry0, {  0,  0, 79,109}, {  0,  0}}, //0 idle 1
	{HenryM_ArcMain_Henry0, { 80,  0, 80,107}, {  1, -2}}, //1 idle 2
	{HenryM_ArcMain_Henry0, {170,110, 77,111}, { -1,  2}}, //2 idle 3
	{HenryM_ArcMain_Henry0, {  0,110, 77,112}, { -1,  3}}, //3 idle 4
	
	{HenryM_ArcMain_Henry0, {161,  0, 92,100}, {  0,  0}}, //4 left 1
	{HenryM_ArcMain_Henry0, { 78,110, 91,100}, {  0,  0}}, //5 left 2
	
	{HenryM_ArcMain_Henry1, {  0,  0, 84, 68}, {  0,  0}}, //6 down 1
	{HenryM_ArcMain_Henry1, { 85,  0, 83, 70}, {  0,  0}}, //7 down 2
	
	{HenryM_ArcMain_Henry1, {169,  0, 69,119}, {  0,  0}}, //8 up 1
	{HenryM_ArcMain_Henry1, {  0, 69, 70,116}, {  0,  0}}, //9 up 2
	
	{HenryM_ArcMain_Henry1, { 71,120,101,102}, {  0,  0}}, //10 right 1
	{HenryM_ArcMain_Henry2, {  0,  0, 99,102}, {  0,  0}}, //11 right 2
	
	{HenryM_ArcMain_Henry2, {100,  0, 86, 92}, {  0,  0}}, //12 talk 1
	{HenryM_ArcMain_Henry2, {  0,103, 85, 92}, {  0,  0}}, //13 talk 2
	{HenryM_ArcMain_Henry2, { 86,103, 85, 92}, {  0,  0}}, //14 talk 3
	{HenryM_ArcMain_Henry5, { 86,  0, 85, 92}, {  0,  0}}, //15 talk 4
	{HenryM_ArcMain_Henry3, {  0,  0, 86, 92}, {  0,  0}}, //16 talk 5
	{HenryM_ArcMain_Henry3, { 87,  0, 85, 92}, {  0,  0}}, //17 talk 6
	{HenryM_ArcMain_Henry3, {  0, 93, 86, 92}, {  0,  0}}, //18 talk 7
	{HenryM_ArcMain_Henry3, { 87, 93, 85, 92}, {  0,  0}}, //19 talk 8
	{HenryM_ArcMain_Henry4, {  0,  0, 85, 92}, {  0,  0}}, //20 talk 9
	{HenryM_ArcMain_Henry4, {  0, 86, 85, 92}, {  0,  0}}, //21 talk 10
	{HenryM_ArcMain_Henry4, {  0, 93, 86, 92}, {  0,  0}}, //22 talk 11
	{HenryM_ArcMain_Henry4, { 87, 93, 85, 92}, {  0,  0}}, //23 talk 12
	{HenryM_ArcMain_Henry5, {  0,  0, 85, 92}, {  0,  0}}, //24 talk 13
};

static const Animation char_henrym_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4, 5, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 6, 7, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 8, 9, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 10, 11, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//HenryM character functions
void Char_HenryM_SetFrame(void *user, u8 frame)
{
	Char_HenryM *this = (Char_HenryM*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_henrym_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_HenryM_Tick(Character *character)
{
	Char_HenryM *this = (Char_HenryM*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_HenryM_SetFrame);
	Character_Draw(character, &this->tex, &char_henrym_frame[this->frame]);
}

void Char_HenryM_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_HenryM_Free(Character *character)
{
	Char_HenryM *this = (Char_HenryM*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_HenryM_New(fixed_t x, fixed_t y)
{
	//Allocate henrym object
	Char_HenryM *this = Mem_Alloc(sizeof(Char_HenryM));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_HenryM_New] Failed to allocate henrym object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_HenryM_Tick;
	this->character.set_anim = Char_HenryM_SetAnim;
	this->character.free = Char_HenryM_Free;
	
	Animatable_Init(&this->character.animatable, char_henrym_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 7;

	//health bar color
	this->character.health_bar = 0xFF62515B;
	
	this->character.focus_x = FIXED_DEC(111,1);
	this->character.focus_y = FIXED_DEC(47,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR2\\HENRYM.ARC;1");
	
	const char **pathp = (const char *[]){
		"henry0.tim", //HenryM_ArcMain_Henry0
		"henry1.tim", //HenryM_ArcMain_Henry1
		"henry2.tim", //HenryM_ArcMain_Henry2
		"henry3.tim", //HenryM_ArcMain_Henry3
		"henry4.tim", //HenryM_ArcMain_Henry4
		"henry5.tim", //HenryM_ArcMain_Henry5
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
