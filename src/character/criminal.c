/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "criminal.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Criminal character structure
enum
{
	Criminal_ArcMain_Intro,
	Criminal_ArcMain_Idle,
	Criminal_ArcMain_Left0,
	Criminal_ArcMain_Left1,
	Criminal_ArcMain_Down0,
	Criminal_ArcMain_Down1,
	Criminal_ArcMain_Up0,
	Criminal_ArcMain_Up1,
	Criminal_ArcMain_Right0,
	Criminal_ArcMain_Right1,
	
	Criminal_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Criminal_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
	u8 opacity;
} Char_Criminal;

static void Character_DrawBlend(Character *this, Gfx_Tex *tex, const CharFrame *cframe, u8 opacity)
{
	//Draw character
	fixed_t x = this->x - stage.camera.x - FIXED_DEC(cframe->off[0],1);
	fixed_t y = this->y - stage.camera.y - FIXED_DEC(cframe->off[1],1);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {x, y, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Stage_BlendTexV2(tex, &src, &dst, stage.camera.bzoom, 0, opacity);
}

//Criminal character definitions
static const CharFrame char_criminal_frame[] = {
	{Criminal_ArcMain_Intro, {  0,  0,223,226}, { 42,183+4}}, //0 intro 1
	
	{Criminal_ArcMain_Idle, {  0,  0,223,226}, { 42,183+4}}, //1 idle 1
	
	{Criminal_ArcMain_Left0, {  0,  0,223,226}, { 42,183+4}}, //2 left 1
	{Criminal_ArcMain_Left1, {  0,  0,223,226}, { 42,183+4}}, //3 left 2
	
	{Criminal_ArcMain_Down0, {  0,  0,223,226}, { 42,183+4}}, //4 down 1
	{Criminal_ArcMain_Down1, {  0,  0,223,226}, { 42,183+4}}, //5 down 2
	
	{Criminal_ArcMain_Up0, {  0,  0,223,226}, { 42,183+4}}, //6 up 1
	{Criminal_ArcMain_Up1, {  0,  0,223,226}, { 42,183+4}}, //7 up 2
	
	{Criminal_ArcMain_Right0, {  0,  0,223,226}, { 42,183+4}}, //8 right 1
	{Criminal_ArcMain_Right1, {  0,  0,223,226}, { 42,183+4}}, //9 right 2
};

static const Animation char_criminal_anim[CharAnim_Max] = {
	{3, (const u8[]){ 1, ASCR_BACK, 1}},             //CharAnim_Idle
	{3, (const u8[]){ 2,  3, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{3, (const u8[]){ 4,  5, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{3, (const u8[]){ 6,  7, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{3, (const u8[]){ 8, 9, ASCR_BACK, 1}},          //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

static const Animation char_criminal_anim2[CharAnim_Max] = {
	{3, (const u8[]){ 0, ASCR_BACK, 1}},             //CharAnim_Idle
	{3, (const u8[]){ 2,  3, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{3, (const u8[]){ 4,  5, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{3, (const u8[]){ 6,  7, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{3, (const u8[]){ 8, 9, ASCR_BACK, 1}},          //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Criminal character functions
void Char_Criminal_SetFrame(void *user, u8 frame)
{
	Char_Criminal *this = (Char_Criminal*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_criminal_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Criminal_Tick(Character *character)
{
	Char_Criminal *this = (Char_Criminal*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
		
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Criminal_SetFrame);
	
	if (stage.song_step >= -24)
		Animatable_Init(&this->character.animatable, char_criminal_anim);
	
	if (stage.song_step >= -122 && this->opacity < 99)
	{
		this->opacity++;
		Character_DrawBlend(character, &this->tex, &char_criminal_frame[this->frame], this->opacity);

		//FntPrint("opacity is %d", this->opacity);
	}

	else if (stage.song_step >= -24)
		Character_Draw(character, &this->tex, &char_criminal_frame[this->frame]);
}

void Char_Criminal_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Criminal_Free(Character *character)
{
	Char_Criminal *this = (Char_Criminal*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Criminal_New(fixed_t x, fixed_t y)
{
	//Allocate criminal object
	Char_Criminal *this = Mem_Alloc(sizeof(Char_Criminal));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Criminal_New] Failed to allocate criminal object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Criminal_Tick;
	this->character.set_anim = Char_Criminal_SetAnim;
	this->character.free = Char_Criminal_Free;
	
	Animatable_Init(&this->character.animatable, char_criminal_anim2);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 3;

	//health bar color
	this->character.health_bar = 0xFFAB21D7;
	
	this->character.focus_x = FIXED_DEC(72,1);
	this->character.focus_y = FIXED_DEC(-60,1);
	this->character.focus_zoom = FIXED_DEC(62,100);
	
	this->character.size = FIXED_DEC(1,1);
	
	this->opacity = 0;
	
	//Load art
	this->arc_main = IO_Read("\\CHAR2\\CRIMINAL.ARC;1");
	
	const char **pathp = (const char *[]){
		"intro.tim", //Criminal_ArcMain_Intro
		"idle.tim", //Criminal_ArcMain_Idle
		"left0.tim", //Criminal_ArcMain_Left0
		"left1.tim", //Criminal_ArcMain_Left1
		"down0.tim", //Criminal_ArcMain_Down0
		"down1.tim", //Criminal_ArcMain_Down1
		"up0.tim", //Criminal_ArcMain_Up0
		"up1.tim", //Criminal_ArcMain_Up1
		"right0.tim", //Criminal_ArcMain_Right0
		"right1.tim", //Criminal_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
