/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "henryp.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//HenryP character structure
enum
{
	HenryP_ArcMain_HenryP0,
	HenryP_ArcMain_HenryP1,
	HenryP_ArcMain_HenryP2,
	
	HenryP_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[HenryP_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_HenryP;

//HenryP character definitions
static const CharFrame char_henryp_frame[] = {
	{HenryP_ArcMain_HenryP0, {  0,  0, 91, 96}, { 42,183+4}}, //0 idle 1
	{HenryP_ArcMain_HenryP0, { 92,  0, 91, 96}, { 42,183+4}}, //1 idle 2
	
	{HenryP_ArcMain_HenryP0, {  0, 97, 91, 96}, { 42,183+4}}, //2 left 1
	{HenryP_ArcMain_HenryP0, { 92, 97, 91, 96}, { 42,183+4}}, //3 left 2
	
	{HenryP_ArcMain_HenryP1, {  0,  0, 91, 96}, { 42,183+4}}, //4 down 1
	{HenryP_ArcMain_HenryP1, { 92,  0, 91, 96}, { 42,183+4}}, //5 down 2
	
	{HenryP_ArcMain_HenryP1, {  0, 97, 91, 96}, { 42,183+4}}, //6 up 1
	{HenryP_ArcMain_HenryP1, { 92, 97, 91, 96}, { 42,183+4}}, //7 up 2
	
	{HenryP_ArcMain_HenryP2, {  0,  0, 91, 96}, { 42,183+4}}, //8 right 1
	{HenryP_ArcMain_HenryP2, { 92,  0, 91, 96}, { 42,183+4}}, //9 right 2
};

static const Animation char_henryp_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 2,  3, ASCR_CHGANI, CharAnim_Left}},         //CharAnim_Left
	{0, (const u8[]){ 1, ASCR_BACK, 1}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 4,  5, ASCR_CHGANI, CharAnim_Down}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 6,  7, ASCR_CHGANI, CharAnim_Up}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){8, 9, ASCR_CHGANI, CharAnim_Right}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//HenryP character functions
void Char_HenryP_SetFrame(void *user, u8 frame)
{
	Char_HenryP *this = (Char_HenryP*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_henryp_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_HenryP_Tick(Character *character)
{
	Char_HenryP *this = (Char_HenryP*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_HenryP_SetFrame);
	if (stage.song_step >= 912 && stage.song_step <= 1167 || stage.song_step >= 1423 && stage.song_step <= 1680)
		Character_Draw(character, &this->tex, &char_henryp_frame[this->frame]);
}

void Char_HenryP_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_HenryP_Free(Character *character)
{
	Char_HenryP *this = (Char_HenryP*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_HenryP_New(fixed_t x, fixed_t y)
{
	//Allocate henryp object
	Char_HenryP *this = Mem_Alloc(sizeof(Char_HenryP));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_HenryP_New] Failed to allocate henryp object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_HenryP_Tick;
	this->character.set_anim = Char_HenryP_SetAnim;
	this->character.free = Char_HenryP_Free;
	
	Animatable_Init(&this->character.animatable, char_henryp_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	this->character.idle2 = 1;
	
	this->character.health_i = 4;

	//health bar color
	this->character.health_bar = 0xFF383838;
	
	this->character.focus_x = FIXED_DEC(1,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\HENRYP.ARC;1");
	
	const char **pathp = (const char *[]){
		"henryp0.tim", //HenryP_ArcMain_HenryP0
		"henryp1.tim", //HenryP_ArcMain_HenryP1
		"henryp2.tim",  //HenryP_ArcMain_HenryP2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
