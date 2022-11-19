/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bf.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend player types
enum
{
	BF_ArcMain_BF0,
	BF_ArcMain_BF1,
	BF_ArcMain_BF2,
	BF_ArcMain_BF3,
	BF_ArcMain_BF4,
	BF_ArcMain_BF5,
	BF_ArcMain_BF6,
	
	BF_ArcMain_Max,
};

#define BF_Arc_Max BF_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BF_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_BF;

//Boyfriend player definitions
static const CharFrame char_bf_frame[] = 
{
	{BF_ArcMain_BF0, {  0,   0, 102,  99}, { 53,  92}}, //0 idle 1
	{BF_ArcMain_BF0, {103,   0, 102,  99}, { 53,  92}}, //1 idle 2
	{BF_ArcMain_BF0, {  0, 100, 102, 101}, { 53,  94}}, //2 idle 3
	{BF_ArcMain_BF0, {103, 100, 103, 104}, { 53,  97}}, //3 idle 4
	{BF_ArcMain_BF1, {  0,   0, 103, 104}, { 53,  97}}, //4 idle 5
	
	{BF_ArcMain_BF1, {104,   0,  96, 102}, { 56,  95}}, //5 left 1
	{BF_ArcMain_BF1, {  0, 105,  94, 102}, { 54,  95}}, //6 left 2
	
	{BF_ArcMain_BF1, { 95, 103,  94,  89}, { 52,  82}}, //7 down 1
	{BF_ArcMain_BF2, {  0,   0,  94,  90}, { 52,  83}}, //8 down 2
	
	{BF_ArcMain_BF2, { 95,   0,  93, 112}, { 41, 104}}, //9 up 1
	{BF_ArcMain_BF2, {  0,  91,  94, 111}, { 42, 103}}, //10 up 2
		
	{BF_ArcMain_BF2, { 95, 113, 102, 102}, { 41,  95}}, //11 right 1
	{BF_ArcMain_BF3, {  0,   0, 102, 102}, { 41,  95}}, //12 right 2
		
	{BF_ArcMain_BF3, {103,   0,  99, 105}, { 54,  98}}, //13 peace 1
	{BF_ArcMain_BF3, {  0, 103, 104, 103}, { 54,  96}}, //14 peace 2
	{BF_ArcMain_BF3, {105, 106, 104, 104}, { 54,  97}}, //15 peace 3
		
	{BF_ArcMain_BF4, {  0,   0, 128, 128}, { 53,  92}}, //16 sweat 1
	{BF_ArcMain_BF4, {128,   0, 128, 128}, { 53,  93}}, //17 sweat 2
	{BF_ArcMain_BF4, {  0, 128, 128, 128}, { 53,  98}}, //18 sweat 3
	{BF_ArcMain_BF4, {128, 128, 128, 128}, { 53,  98}}, //19 sweat 4
		
	{BF_ArcMain_BF5, {  0,   0,  93, 108}, { 52, 101}}, //20 left miss 1
	{BF_ArcMain_BF5, { 94,   0,  93, 108}, { 52, 101}}, //21 left miss 2
	
	{BF_ArcMain_BF5, {  0, 109,  95,  98}, { 50,  90}}, //22 down miss 1
	{BF_ArcMain_BF5, { 96, 109,  95,  97}, { 50,  89}}, //23 down miss 2
		
	{BF_ArcMain_BF6, {  0,   0,  90, 107}, { 44,  99}}, //24 up miss 1
	{BF_ArcMain_BF6, { 91,   0,  89, 108}, { 44, 100}}, //25 up miss 2
	
	{BF_ArcMain_BF6, {  0, 108,  99, 108}, { 42, 101}}, //26 right miss 1
	{BF_ArcMain_BF6, {100, 109, 101, 108}, { 43, 101}}, //27 right miss 2
};

static const CharFrame char_bf_frame2[] = 
{
	{BF_ArcMain_BF0, {  0,  0, 82, 94}, { 53, 92}}, //0 idle 1
	{BF_ArcMain_BF0, { 83,  0, 82, 94}, { 53, 92}}, //1 idle 2
	{BF_ArcMain_BF0, {166,  0, 82, 94}, { 53, 92}}, //2 idle 3
	{BF_ArcMain_BF0, {  0, 95, 82, 94}, { 53, 92}}, //3 idle 4
	{BF_ArcMain_BF0, { 83, 95, 82, 94}, { 53, 92}}, //4 idle 5
		
	{BF_ArcMain_BF0, {166, 95, 82, 94}, { 53, 92}}, //5 left 1
	{BF_ArcMain_BF1, {  0,  0, 82, 94}, { 53, 92}}, //6 left 2
		
	{BF_ArcMain_BF1, { 83,  0, 82, 94}, { 53, 92}}, //7 down 1
	{BF_ArcMain_BF1, {166,  0, 82, 94}, { 53, 92}}, //8 down 2
		
	{BF_ArcMain_BF1, {  0, 95, 82, 94}, { 53, 92}}, //9 up 1
	{BF_ArcMain_BF1, { 83, 95, 82, 94}, { 53, 92}}, //10 up 2
		
	{BF_ArcMain_BF1, {166, 95, 82, 94}, { 53, 92}}, //11 right 1
	{BF_ArcMain_BF2, {  0,  0, 82, 94}, { 53, 92}}, //12 right 2
			
	{BF_ArcMain_BF0, {  0,  0,  0,  0}, {  0,  0}}, //13 peace 1
	{BF_ArcMain_BF0, {  0,  0,  0,  0}, {  0,  0}}, //14 peace 2
	{BF_ArcMain_BF0, {  0,  0,  0,  0}, {  0,  0}}, //15 peace 3
		
	{BF_ArcMain_BF0, {  0,  0,  0,  0}, {  0,  0}}, //16 sweat 1
	{BF_ArcMain_BF0, {  0,  0,  0,  0}, {  0,  0}}, //17 sweat 2
	{BF_ArcMain_BF0, {  0,  0,  0,  0}, {  0,  0}}, //18 sweat 3
	{BF_ArcMain_BF0, {  0,  0,  0,  0}, {  0,  0}}, //19 sweat 4
		
	{BF_ArcMain_BF2, { 83,  0, 82, 94}, { 53, 92}}, //20 left miss 1
	{BF_ArcMain_BF2, {166,  0, 82, 94}, { 53, 92}}, //21 left miss 2
		
	{BF_ArcMain_BF2, {  0, 95, 82, 94}, { 53, 92}}, //22 down miss 1
	{BF_ArcMain_BF2, { 83, 95, 82, 94}, { 53, 92}}, //23 down miss 2
		
	{BF_ArcMain_BF2, {166, 95, 82, 94}, { 53, 92}}, //24 up miss 1
	{BF_ArcMain_BF3, {  0,  0, 82, 94}, { 53, 92}}, //25 up miss 2
	
	{BF_ArcMain_BF3, { 83,  0, 82, 94}, { 53, 92}}, //26 right miss 1
	{BF_ArcMain_BF3, {166,  0, 82, 94}, { 53, 92}}, //27 right miss 2
};
static const Animation char_bf_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){11, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{1, (const u8[]){ 20, 20, 21, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 22, 22, 23, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 24, 24, 25, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 26, 26, 27, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
	
	{2, (const u8[]){13, 14, 15, ASCR_BACK, 1}},         //PlayerAnim_Peace
	{2, (const u8[]){16, 17, 18, 19, ASCR_REPEAT}},      //PlayerAnim_Sweat
};

//Boyfriend player functions
void Char_BF_SetFrame(void *user, u8 frame)
{
	Char_BF *this = (Char_BF*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		CharFrame *cframe;

		if (stage.stage_id == StageId_2_2 || stage.stage_id == StageId_3_3 || stage.stage_id == StageId_4_4)
 			cframe = &char_bf_frame[this->frame = frame];
 		else 
			cframe = &char_bf_frame2[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BF_Tick(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_1_3)
	{
		if (stage.song_step == 257)
		{
			this->character.focus_x = FIXED_DEC(-185,1);
			this->character.focus_y = FIXED_DEC(-91,1);	
			this->character.health_bar = 0xFF7CD6F5;
		}
		if (stage.song_step == 911)
		{
			this->character.focus_x = FIXED_DEC(-132,1);
			this->character.focus_y = FIXED_DEC(-455,1);
			this->character.focus_zoom = FIXED_DEC(2295,512);
		}
		if (stage.song_step == 912)
		{
			this->character.focus_y = FIXED_DEC(-105,1);
			this->character.focus_zoom = FIXED_DEC(767,512);
		}
		if (stage.song_step == 1167)
		{
			this->character.focus_x = FIXED_DEC(-509,1);
			this->character.focus_y = FIXED_DEC(-91,1);	
			this->character.focus_zoom = FIXED_DEC(1,1024);
		}
		if (stage.song_step == 1168)
		{
			this->character.focus_x = FIXED_DEC(-185,1);
			this->character.focus_zoom = FIXED_DEC(1,1);
		}
		if (stage.song_step == 1423)
		{
			this->character.focus_x = FIXED_DEC(-132,1);
			this->character.focus_y = FIXED_DEC(-455,1);
			this->character.focus_zoom = FIXED_DEC(2295,512);
		}
		if (stage.song_step == 1424)
		{
			this->character.focus_y = FIXED_DEC(-105,1);
			this->character.focus_zoom = FIXED_DEC(767,512);
		}
		if (stage.song_step == 1680)
		{
			this->character.focus_x = FIXED_DEC(-73,1);
			this->character.focus_y = FIXED_DEC(-77,1);
			this->character.focus_zoom = FIXED_DEC(1,1024);
			this->character.health_bar = 0xFF00A2E8;
		}
		if (stage.song_step == 1681)
		{
			this->character.focus_zoom = FIXED_DEC(1,1);
		}
		if (stage.song_step == 2200)
		{
			this->character.focus_x = FIXED_DEC(-190,1);
			this->character.focus_y = FIXED_DEC(-97,1);
		}
	}
	if (stage.stage_id == StageId_2_3)	
	{
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 831)
		{
			this->character.focus_x = FIXED_DEC(-91,1);
			this->character.focus_y = FIXED_DEC(-84,1);	
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 889)
		{
			this->character.focus_x = FIXED_DEC(-48,1);
			this->character.focus_y = FIXED_DEC(-64,1);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 2111)
		{
			this->character.focus_x = FIXED_DEC(-91,1);
			this->character.focus_y = FIXED_DEC(-84,1);	
		}
		if ((stage.flag & 	STAGE_FLAG_JUST_STEP) && stage.song_step ==2172)
		{
			this->character.focus_x = FIXED_DEC(-48,1);
			this->character.focus_y = FIXED_DEC(-64,1);
		}
		
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 824)
		{
			this->character.health_bar = 0xFFCF323A;
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 895)
		{
			this->character.health_bar = 0xFF00A2E8;
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 2104)
		{
			this->character.health_bar = 0xFFCF323A;
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 2175)
		{
			this->character.health_bar = 0xFF00A2E8;
		}
		
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 320)
		{
			this->character.focus_zoom = FIXED_DEC(787,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 384)
		{
			this->character.focus_zoom = FIXED_DEC(857,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 388)
		{
			this->character.focus_zoom = FIXED_DEC(787,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 448)
		{
			this->character.focus_zoom = FIXED_DEC(957,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 460)
		{
			this->character.focus_zoom = FIXED_DEC(597,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 824)
		{
			this->character.focus_zoom = FIXED_DEC(857,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 832)
		{
			this->character.focus_zoom = FIXED_DEC(597,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 970)
		{
			this->character.focus_zoom = FIXED_DEC(957,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 974)
		{
			this->character.focus_zoom = FIXED_DEC(597,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1024)
		{
			this->character.focus_zoom = FIXED_DEC(957,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1034)
		{
			this->character.focus_zoom = FIXED_DEC(597,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1088)
		{
			this->character.focus_zoom = FIXED_DEC(857,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1216)
		{
			this->character.focus_zoom = FIXED_DEC(597,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1536)
		{
			this->character.focus_zoom = FIXED_DEC(957,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1544)
		{
			this->character.focus_zoom = FIXED_DEC(597,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1664)
		{
			this->character.focus_zoom = FIXED_DEC(957,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1672)
		{
			this->character.focus_zoom = FIXED_DEC(597,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1920)
		{
			this->character.focus_zoom = FIXED_DEC(857,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1967)
		{
			this->character.focus_zoom = FIXED_DEC(597,512);
		}
	}

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
			
		if (character->idle2 == 1)
		{
			if (Animatable_Ended(&character->animatable) &&
			(character->animatable.anim != CharAnim_Left &&
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
			(stage.song_step & 0x7) == 3)
			character->set_anim(character, CharAnim_LeftAlt);
		}
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BF_SetFrame);

	if (stage.stage_id == StageId_2_2 || stage.stage_id == StageId_3_3 || stage.stage_id == StageId_4_4)
		Character_Draw(character, &this->tex, &char_bf_frame[this->frame]);
 	else 
	{
		if (stage.stage_id == StageId_1_3 && stage.song_step <= 270 || stage.stage_id == StageId_1_3 && stage.song_step >= 1680 && stage.song_step <= 2200)
			Character_Draw(character, &this->tex, &char_bf_frame2[this->frame]);
		
		else if (stage.stage_id != StageId_1_3)
			Character_Draw(character, &this->tex, &char_bf_frame2[this->frame]);
	}
}

void Char_BF_SetAnim(Character *character, u8 anim)
{
	Char_BF *this = (Char_BF*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BF_Free(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BF_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend object
	Char_BF *this = Mem_Alloc(sizeof(Char_BF));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BF_New] Failed to allocate boyfriend object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BF_Tick;
	this->character.set_anim = Char_BF_SetAnim;
	this->character.free = Char_BF_Free;
	
	Animatable_Init(&this->character.animatable, char_bf_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	this->character.idle2 = 0;
	
	if (stage.stage_id == StageId_2_2 || stage.stage_id == StageId_3_3 || stage.stage_id == StageId_4_4)
		this->character.health_i = 1;
	else
		this->character.health_i = 0;

	//health bar color
	if (stage.stage_id == StageId_2_2 || stage.stage_id == StageId_3_3 || stage.stage_id == StageId_4_4)
	{
		this->character.health_bar = 0xFF2BA9C9;
	}
	else 
	{
		this->character.health_bar = 0xFF00A2E8;
	}
	
	if (stage.stage_id == StageId_1_1)
	{	
		this->character.focus_x = FIXED_DEC(-38,1);
		this->character.focus_y = FIXED_DEC(-70,1);
		this->character.focus_zoom = FIXED_DEC(261,256);
	}
	else if (stage.stage_id == StageId_1_2)
	{	
		this->character.focus_x = FIXED_DEC(-87,1);
		this->character.focus_y = FIXED_DEC(-54,1);
		this->character.focus_zoom = FIXED_DEC(1,1);
	}
	else if (stage.stage_id == StageId_1_3)
	{	
		this->character.focus_x = FIXED_DEC(-73,1);
		this->character.focus_y = FIXED_DEC(-77,1);
		this->character.focus_zoom = FIXED_DEC(1,1);
	}
	else if (stage.stage_id == StageId_1_4)
	{	
		this->character.focus_x = FIXED_DEC(-99,1);
		this->character.focus_y = FIXED_DEC(-76,1);
		this->character.focus_zoom = FIXED_DEC(477,512);
	}
	else if (stage.stage_id == StageId_2_2)
	{	
		this->character.focus_x = FIXED_DEC(-63,1);
		this->character.focus_y = FIXED_DEC(-65,1);
		this->character.focus_zoom = FIXED_DEC(1,1);
	}
	else if (stage.stage_id == StageId_2_3)
	{	
		this->character.focus_x = FIXED_DEC(-48,1);
		this->character.focus_y = FIXED_DEC(-64,1);
		this->character.focus_zoom = FIXED_DEC(597,512);
	}
	else if (stage.stage_id == StageId_3_3)
	{	
		this->character.focus_x = FIXED_DEC(-44,1);
		this->character.focus_y = FIXED_DEC(-97,1);
		this->character.focus_zoom = FIXED_DEC(597,512);
	}
	else if (stage.stage_id == StageId_4_4)
	{	
		this->character.focus_x = FIXED_DEC(-69,1);
		this->character.focus_y = FIXED_DEC(-82,1);
		this->character.focus_zoom = FIXED_DEC(1,1);
	}
	else if (stage.stage_id == StageId_5_1)
	{	
		this->character.focus_x = FIXED_DEC(-65,1);
		this->character.focus_y = FIXED_DEC(-68,1);
		this->character.focus_zoom = FIXED_DEC(1004,1024);
	}
	else
	{	
		this->character.focus_x = FIXED_DEC(-50,1);
		this->character.focus_y = FIXED_DEC(-65,1);
		this->character.focus_zoom = FIXED_DEC(1,1);
	}
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	if (stage.stage_id == StageId_2_2 || stage.stage_id == StageId_3_3 || stage.stage_id == StageId_4_4)
	{
		this->arc_main = IO_Read("\\CHAR\\BF.ARC;1");
		
		const char **pathp = (const char *[]){
			"bf0.tim",   //BF_ArcMain_BF0
			"bf1.tim",   //BF_ArcMain_BF1
			"bf2.tim",   //BF_ArcMain_BF2
			"bf3.tim",   //BF_ArcMain_BF3
			"bf4.tim",   //BF_ArcMain_BF4
			"bf5.tim",   //BF_ArcMain_BF5
			"bf6.tim",   //BF_ArcMain_BF6
			NULL
		};
		IO_Data *arc_ptr = this->arc_ptr;
		for (; *pathp != NULL; pathp++)
			*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	}
	else
	{
		this->arc_main = IO_Read("\\CHAR\\BFO.ARC;1");
		
		const char **pathp = (const char *[]){
			"bfo0.tim",   //BF_ArcMain_BF0
			"bfo1.tim",   //BF_ArcMain_BF1
			"bfo2.tim",   //BF_ArcMain_BF2
			"bfo3.tim",   //BF_ArcMain_BF3
			NULL
		};
		IO_Data *arc_ptr = this->arc_ptr;
		for (; *pathp != NULL; pathp++)
			*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	}
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
