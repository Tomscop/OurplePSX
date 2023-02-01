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
	
	u8 opacity;
} Char_HenryM;

static void Character_DrawBlend(Character *this, Gfx_Tex *tex, const CharFrame *cframe, u8 opacity)
{
	//Draw character
	fixed_t x = this->x - stage.camera.x - FIXED_DEC(cframe->off[0],1);
	fixed_t y = this->y - stage.camera.y - FIXED_DEC(cframe->off[1],1);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {x, y, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Stage_BlendTexV2(tex, &src, &dst, stage.camera.bzoom, 0, opacity);
}

//HenryM character definitions
static const CharFrame char_henrym_frame[] = {
	{HenryM_ArcMain_Henry0, {  0,  0, 79,109}, {  0,  0}}, //0 idle 1
	{HenryM_ArcMain_Henry0, { 80,  0, 80,107}, {  1, -2}}, //1 idle 2
	{HenryM_ArcMain_Henry0, {170,110, 77,111}, { -1,  2}}, //2 idle 3
	{HenryM_ArcMain_Henry0, {  0,110, 77,112}, { -1,  3}}, //3 idle 4
	
	{HenryM_ArcMain_Henry0, {161,  0, 92,100}, { 15,-13}}, //4 left 1
	{HenryM_ArcMain_Henry0, { 78,110, 91,100}, { 15,-13}}, //5 left 2
	
	{HenryM_ArcMain_Henry1, {  0,  0, 84, 68}, {  5,-42}}, //6 down 1
	{HenryM_ArcMain_Henry1, { 85,  0, 83, 70}, {  5,-40}}, //7 down 2
	
	{HenryM_ArcMain_Henry1, {169,  0, 69,119}, { -4, 10}}, //8 up 1
	{HenryM_ArcMain_Henry1, {  0, 69, 70,116}, { -4,  7}}, //9 up 2
	
	{HenryM_ArcMain_Henry1, { 71,120,101,102}, { 10, -8}}, //10 right 1
	{HenryM_ArcMain_Henry2, {  0,  0, 99,102}, { 10, -8}}, //11 right 2
	
	{HenryM_ArcMain_Henry2, {100,  0, 86, 92}, {  0,-17}}, //12 talk 1
	{HenryM_ArcMain_Henry2, {  0,103, 85, 92}, { -1,-17}}, //13 talk 2
	{HenryM_ArcMain_Henry2, { 86,103, 85, 92}, { -1,-17}}, //14 talk 3
	{HenryM_ArcMain_Henry5, { 86,  0, 85, 92}, {  0,-17}}, //15 talk 4
	{HenryM_ArcMain_Henry3, {  0,  0, 86, 92}, {  0,-17}}, //16 talk 5
	{HenryM_ArcMain_Henry3, { 87,  0, 85, 92}, { -1,-17}}, //17 talk 6
	{HenryM_ArcMain_Henry3, {  0, 93, 86, 92}, {  0,-17}}, //18 talk 7
	{HenryM_ArcMain_Henry3, { 87, 93, 85, 92}, { -1,-17}}, //19 talk 8
	{HenryM_ArcMain_Henry4, {  0,  0, 85, 92}, {  0,-17}}, //20 talk 9
	{HenryM_ArcMain_Henry4, { 86,  0, 85, 92}, {  0,-17}}, //21 talk 10
	{HenryM_ArcMain_Henry4, {  0, 93, 86, 92}, {  0,-17}}, //22 talk 11
	{HenryM_ArcMain_Henry4, { 87, 93, 85, 92}, { -1,-17}}, //23 talk 12
	{HenryM_ArcMain_Henry5, {  0,  0, 85, 92}, {  0,-17}}, //24 talk 13
};

static const Animation char_henrym_anim[CharAnim_Max] = {
	{3, (const u8[]){ 0, 1, 2, 3, ASCR_BACK, 1}}, //CharAnim_Idle
	{3, (const u8[]){ 4, 5, 5, 5, ASCR_BACK, 1}},         //CharAnim_Left
	{1, (const u8[]){ 21, 21, 21, 12, 13, 13, 19, 19, 16, 19, 19, 20, 20, 23, 23, 12, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{3, (const u8[]){ 6, 7, 7, 7, ASCR_BACK, 1}},         //CharAnim_Down
	{1, (const u8[]){ 14, 14, 15, 18, 18, 22, 22, 16, 17, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{3, (const u8[]){ 8, 9, 9, 9, ASCR_BACK, 1}},         //CharAnim_Up
	{1, (const u8[]){ 22, 22, 22, 22, 16, 18, 18, 23, 23, 24, 19, 19, 20, 20, 20, 19, 19, 16, 17, 22, 22, 13, 13, 16, 16, 18, 18, 18, 18, 14, 14, 20, 20, 20, 20, 20, 20, ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{3, (const u8[]){ 10, 11, 11, 11, ASCR_BACK, 1}},         //CharAnim_Right
	{1, (const u8[]){ 20, 20, 20, 20, 16, 17, 17, 19, 19, 20, 20, 20, 20, 19, 19, 13, 12, 12, 21, 21, 21, 19, 19, 20, 20, 19, 19, 17, 17, 14, 14, 16, 16, 18, 18, 18, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

static const Animation char_henrym_anim2[CharAnim_Max] = {
	{3, (const u8[]){ 0, 1, 2, 3, ASCR_BACK, 1}}, //CharAnim_Idle
	{3, (const u8[]){ 4, 5, 5, 5, ASCR_BACK, 1}},         //CharAnim_Left
	{1, (const u8[]){ 12, 12, 13, 13, 14, 14, 15, 15, 15, 15, 16, 17, 17, 18, 18, 19, 19, 16, 16, 17, 17, 19, 19, 20, 20, 20, 20, 20, 16, 16, 17, 17, 19, 19, 20, 20, 21, 16, 16, 17, 17, 17, 19, 20, 20, 16, 16, 17, 19, 20, 20, 21, 21, 21, 13, 13, 12, 12, 12, 12, 12, 12, 12, ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{3, (const u8[]){ 6, 7, 7, 7, ASCR_BACK, 1}},         //CharAnim_Down
	{1, (const u8[]){ 12, 12, 13, 13, 14, 15, 15, 15, 15, 16, 17, 17, 18, 18, 18, 17, 17, 19, 19, 16, 17, 17, 19, 19, 20, 20, 20, 19, 19, 18, 18, 18, 15, 15, 20, 20, 20, 20, 20, ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{3, (const u8[]){ 8, 9, 9, 9, ASCR_BACK, 1}},         //CharAnim_Up
	{1, (const u8[]){ 22, 22, 22, 22, 16, 18, 18, 23, 23, 24, 19, 19, 20, 20, 20, 19, 19, 16, 17, 22, 22, 13, 13, 16, 16, 18, 18, 18, 18, 14, 14, 20, 20, 20, 20, 20, 20, ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{3, (const u8[]){ 10, 11, 11, 11, ASCR_BACK, 1}},         //CharAnim_Right
	{1, (const u8[]){ 20, 20, 20, 20, 16, 17, 17, 19, 19, 20, 20, 20, 20, 19, 19, 13, 12, 12, 21, 21, 21, 19, 19, 20, 20, 19, 19, 17, 17, 14, 14, 16, 16, 18, 18, 18, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
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
	
	//Camera stuff
	if (stage.stage_id == StageId_6_3)
	{
		if (stage.song_step == 2432)
		{
			this->character.focus_zoom = FIXED_DEC(1851,512);
		}
		if (stage.song_step == 2433)
		{
			this->character.focus_zoom = FIXED_DEC(1,1);
		}
	}
	
	if((character->animatable.anim  != CharAnim_LeftAlt) && (character->animatable.anim  != CharAnim_DownAlt) && (character->animatable.anim  != CharAnim_UpAlt) && (character->animatable.anim  != CharAnim_RightAlt))
	{
	   //Perform idle dance
	   if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
			Character_PerformIdle(character);
	}
    
    if (stage.song_step == 3743)
		Animatable_Init(&this->character.animatable, char_henrym_anim2);	
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_6_3: //Meaningless
				if (stage.song_step == 1527)
					character->set_anim(character, CharAnim_LeftAlt);
				break;
			default:
				break;
		}
		switch (stage.stage_id)
		{
			case StageId_6_3: //Orphan
				if (stage.song_step == 1768)
					character->set_anim(character, CharAnim_DownAlt);
				break;
			default:
				break;
		}
		switch (stage.stage_id)
		{
			case StageId_6_3: //False Savior
				if (stage.song_step == 2450)
					character->set_anim(character, CharAnim_UpAlt);
				break;
			default:
				break;
		}
		switch (stage.stage_id)
		{
			case StageId_6_3: //God Damn Coward
				if (stage.song_step == 2952)
					character->set_anim(character, CharAnim_RightAlt);
				break;
			default:
				break;
		}
		switch (stage.stage_id)
		{
			case StageId_6_3: //You are just as bad as me
				if (stage.song_step == 3744)
					character->set_anim(character, CharAnim_LeftAlt);
				break;
			default:
				break;
		}
		switch (stage.stage_id)
		{
			case StageId_6_3: //You are a cancer
				if (stage.song_step == 4016)
					character->set_anim(character, CharAnim_DownAlt);
				break;
			default:
				break;
		}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_HenryM_SetFrame);
	
	if (stage.song_step >= 977 && this->opacity < 99)
	{
		this->opacity++;
		Character_DrawBlend(character, &this->tex, &char_henrym_frame[this->frame], this->opacity);

		//FntPrint("opacity is %d", this->opacity);
	}

	else if (stage.song_step >= 982)
	{
		Character_Draw(character, &this->tex, &char_henrym_frame[this->frame]);
		this->character.health_i = 7;
	}
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
	
	this->character.health_i = 9;

	//health bar color
	this->character.health_bar = 0xFF62515B;
	
	this->character.focus_x = FIXED_DEC(101,1);
	this->character.focus_y = FIXED_DEC(41,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	this->opacity = 0;
	
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
