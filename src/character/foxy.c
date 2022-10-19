/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "foxy.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Foxy character structure
enum
{
	Foxy_ArcMain_Idle0,
	Foxy_ArcMain_Idle1,
	Foxy_ArcMain_Idle2,
	Foxy_ArcMain_Left0,
	Foxy_ArcMain_Left1,
	Foxy_ArcMain_Left2,
	Foxy_ArcMain_Down0,
	Foxy_ArcMain_Down1,
	Foxy_ArcMain_Down2,
	Foxy_ArcMain_Up0,
	Foxy_ArcMain_Up1,
	Foxy_ArcMain_Up2,
	Foxy_ArcMain_Right0,
	Foxy_ArcMain_Right1,
	Foxy_ArcMain_Right2,
	
	Foxy_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Foxy_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Foxy;

//Foxy character definitions
static const CharFrame char_foxy_frame[] = {
	{Foxy_ArcMain_Idle0, {  0,  0,138,162}, { 42,183+4}}, //0 idle 1
	{Foxy_ArcMain_Idle1, {  0,  0,138,162}, { 42,183+4}}, //1 idle 2
	{Foxy_ArcMain_Idle2, {  0,  0,138,162}, { 42,183+4}}, //2 idle 3
	
	{Foxy_ArcMain_Left0, {  0,  0,138,162}, { 42,183+4}}, //3 left 1
	{Foxy_ArcMain_Left1, {  0,  0,138,162}, { 42,183+4}}, //4 left 2
	{Foxy_ArcMain_Left2, {  0,  0,138,162}, { 42,183+4}}, //5 left 3
	
	{Foxy_ArcMain_Down0, {  0,  0,138,162}, { 42,183+4}}, //6 down 1
	{Foxy_ArcMain_Down1, {  0,  0,138,162}, { 42,183+4}}, //7 down 2
	{Foxy_ArcMain_Down2, {  0,  0,138,162}, { 42,183+4}}, //8 down 3
	
	{Foxy_ArcMain_Up0, {  0,  0,138,162}, { 42,183+4}}, //9 up 1
	{Foxy_ArcMain_Up1, {  0,  0,138,162}, { 42,183+4}}, //10 up 2
	{Foxy_ArcMain_Up2, {  0,  0,138,162}, { 42,183+4}}, //11 up 3
	
	{Foxy_ArcMain_Right0, {  0,  0,138,162}, { 42,183+4}}, //12 right 1
	{Foxy_ArcMain_Right1, {  0,  0,138,162}, { 42,183+4}}, //13 right 2
	{Foxy_ArcMain_Right2, {  0,  0,138,162}, { 42,183+4}}, //14 right 3
};

static const Animation char_foxy_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 3, 4, 5, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 6,  7, 8, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, 11, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){12, 13, 14, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Foxy character functions
void Char_Foxy_SetFrame(void *user, u8 frame)
{
	Char_Foxy *this = (Char_Foxy*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_foxy_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Foxy_Tick(Character *character)
{
	Char_Foxy *this = (Char_Foxy*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Foxy_SetFrame);
	if ((stage.song_step >= 1037 && stage.song_step <= 1470))
		Character_Draw(character, &this->tex, &char_foxy_frame[this->frame]);
}

void Char_Foxy_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Foxy_Free(Character *character)
{
	Char_Foxy *this = (Char_Foxy*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Foxy_New(fixed_t x, fixed_t y)
{
	//Allocate foxy object
	Char_Foxy *this = Mem_Alloc(sizeof(Char_Foxy));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Foxy_New] Failed to allocate foxy object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Foxy_Tick;
	this->character.set_anim = Char_Foxy_SetAnim;
	this->character.free = Char_Foxy_Free;
	
	Animatable_Init(&this->character.animatable, char_foxy_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 9;

	//health bar color
	this->character.health_bar = 0xFF663810;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-102,1);
	this->character.focus_zoom = FIXED_DEC(1113,512);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\FOXY.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Foxy_ArcMain_Idle0
		"idle1.tim", //Foxy_ArcMain_Idle1
		"idle2.tim", //Foxy_ArcMain_Idle2
		"left0.tim",  //Foxy_ArcMain_Left0
		"left1.tim",  //Foxy_ArcMain_Left1
		"left2.tim",  //Foxy_ArcMain_Left2
		"down0.tim",  //Foxy_ArcMain_Down0
		"down1.tim",  //Foxy_ArcMain_Down1
		"down2.tim",  //Foxy_ArcMain_Down2
		"up0.tim",    //Foxy_ArcMain_Up0
		"up1.tim",    //Foxy_ArcMain_Up1
		"up2.tim",    //Foxy_ArcMain_Up2
		"right0.tim", //Foxy_ArcMain_Right0
		"right1.tim", //Foxy_ArcMain_Right1
		"right2.tim", //Foxy_ArcMain_Right2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
