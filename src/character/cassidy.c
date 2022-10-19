/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "cassidy.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Cassidy player types
enum
{
	Cassidy_ArcMain_Cassidy0,
	Cassidy_ArcMain_Cassidy1,
	
	Cassidy_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Cassidy_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_Cassidy;

//Cassidy player definitions
static const CharFrame char_cassidy_frame[] = 
{
	{Cassidy_ArcMain_Cassidy0, {  0,  0, 55, 68}, { 53, 92}}, //0 idle 1
	{Cassidy_ArcMain_Cassidy0, { 56,  0, 55, 68}, { 53, 92}}, //1 idle 2
	{Cassidy_ArcMain_Cassidy0, {112,  0, 55, 68}, { 53, 92}}, //2 idle 3
	{Cassidy_ArcMain_Cassidy0, {168,  0, 55, 68}, { 53, 92}}, //3 idle 3
	
	{Cassidy_ArcMain_Cassidy0, {  0, 69, 55, 68}, { 53, 92}}, //4 left 1
	{Cassidy_ArcMain_Cassidy0, { 56, 69, 55, 68}, { 53, 92}}, //5 left 2
	
	{Cassidy_ArcMain_Cassidy0, {112, 69, 55, 68}, { 53, 92}}, //6 down 1
	{Cassidy_ArcMain_Cassidy0, {168, 69, 55, 68}, { 53, 92}}, //7 down 2
	
	{Cassidy_ArcMain_Cassidy0, {  0,138, 55, 68}, { 53, 92}}, //8 up 1
	{Cassidy_ArcMain_Cassidy0, { 56,138, 55, 68}, { 53, 92}}, //9 up 2
	
	{Cassidy_ArcMain_Cassidy0, {112,138, 55, 68}, { 53, 92}}, //10 right 1
	{Cassidy_ArcMain_Cassidy0, {168,138, 55, 68}, { 53, 92}}, //11 right 2
	
	{Cassidy_ArcMain_Cassidy1, {  0,  0, 55, 68}, { 53, 92}}, //12 left miss 1
	{Cassidy_ArcMain_Cassidy1, { 56,  0, 55, 68}, { 53, 92}}, //13 left miss 2
	
	{Cassidy_ArcMain_Cassidy1, {112,  0, 55, 68}, { 53, 92}}, //14 down miss 1
	{Cassidy_ArcMain_Cassidy1, {168,  0, 55, 68}, { 53, 92}}, //15 down miss 2
	
	{Cassidy_ArcMain_Cassidy1, {  0, 69, 55, 68}, { 53, 92}}, //16 up miss 1
	{Cassidy_ArcMain_Cassidy1, { 56, 69, 55, 68}, { 53, 92}}, //17 up miss 2
	
	{Cassidy_ArcMain_Cassidy1, {112, 69, 55, 68}, { 53, 92}}, //18 right miss 1
	{Cassidy_ArcMain_Cassidy1, {168, 69, 55, 68}, { 53, 92}}, //19 right miss 2
};
static const Animation char_cassidy_anim[PlayerAnim_Max] = {
	{4, (const u8[]){ 0,  1,  2, 3, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{2, (const u8[]){ 4, 5, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 6, 7, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 8, 9, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 10, 11, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{1, (const u8[]){ 4, 12, 13, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 6, 14, 15, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 8, 16, 17, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 10, 18, 19, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Cassidy player functions
void Char_Cassidy_SetFrame(void *user, u8 frame)
{
	Char_Cassidy *this = (Char_Cassidy*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		CharFrame *cframe;

 		cframe = &char_cassidy_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Cassidy_Tick(Character *character)
{
	Char_Cassidy *this = (Char_Cassidy*)character;

	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_LeftAlt &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_DownAlt &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_UpAlt &&
	     character->animatable.anim != CharAnim_Right &&
	     character->animatable.anim != CharAnim_RightAlt))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if (Animatable_Ended(&character->animatable) &&
			(character->animatable.anim != CharAnim_Left &&
		     character->animatable.anim != CharAnim_LeftAlt &&
		     character->animatable.anim != PlayerAnim_LeftMiss &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != PlayerAnim_DownMiss &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != PlayerAnim_UpMiss &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt &&
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_Cassidy_SetFrame);

	Character_Draw(character, &this->tex, &char_cassidy_frame[this->frame]);
}

void Char_Cassidy_SetAnim(Character *character, u8 anim)
{
	Char_Cassidy *this = (Char_Cassidy*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Cassidy_Free(Character *character)
{
	Char_Cassidy *this = (Char_Cassidy*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Cassidy_New(fixed_t x, fixed_t y)
{
	//Allocate cassidy object
	Char_Cassidy *this = Mem_Alloc(sizeof(Char_Cassidy));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Cassidy_New] Failed to allocate cassidy object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Cassidy_Tick;
	this->character.set_anim = Char_Cassidy_SetAnim;
	this->character.free = Char_Cassidy_Free;
	
	Animatable_Init(&this->character.animatable, char_cassidy_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFFF94345;

	this->character.focus_x = FIXED_DEC(-126,1);
	this->character.focus_y = FIXED_DEC(-46,1);
	this->character.focus_zoom = FIXED_DEC(465,512);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR2\\CASSIDY.ARC;1");
	
	const char **pathp = (const char *[]){
		"cassidy0.tim",   //Cassidy_ArcMain_Cassidy0
		"cassidy1.tim",   //Cassidy_ArcMain_Cassidy1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);

	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
