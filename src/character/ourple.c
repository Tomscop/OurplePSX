/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ourple.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Ourple Guy player types
enum
{
	Ourple_ArcMain_Idle1,
	Ourple_ArcMain_Idle2,
	Ourple_ArcMain_Idle3,
	Ourple_ArcMain_Idle4,
	Ourple_ArcMain_Idle5,
	Ourple_ArcMain_Idle6,
	Ourple_ArcMain_Idle7,
	Ourple_ArcMain_Idle8,
	Ourple_ArcMain_Left1,
	Ourple_ArcMain_Left2,
	Ourple_ArcMain_LeftMiss1,
	Ourple_ArcMain_LeftMiss2,
	Ourple_ArcMain_LeftAlt1,
	Ourple_ArcMain_LeftAlt2,
	Ourple_ArcMain_Down1,
	Ourple_ArcMain_Down2,
	Ourple_ArcMain_DownMiss1,
	Ourple_ArcMain_DownMiss2,
	Ourple_ArcMain_DownAlt1,
	Ourple_ArcMain_DownAlt2,
	Ourple_ArcMain_Up1,
	Ourple_ArcMain_Up2,
	Ourple_ArcMain_UpMiss1,
	Ourple_ArcMain_UpMiss2,
	Ourple_ArcMain_UpAlt1,
	Ourple_ArcMain_UpAlt2,
	Ourple_ArcMain_Right1,
	Ourple_ArcMain_Right2,
	Ourple_ArcMain_RightMiss1,
	Ourple_ArcMain_RightMiss2,
	Ourple_ArcMain_RightAlt1,
	Ourple_ArcMain_RightAlt2,
	
	Ourple_ArcMain_Max,
};

#define Ourple_Arc_Max Ourple_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Ourple_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Ourple;

//Ourple Guy player definitions
static const CharFrame char_ourple_frame[] = 
{
	{Ourple_ArcMain_Idle1, {  0,  0,129,162}, { 53, 92}}, //0 idle 1
	{Ourple_ArcMain_Idle2, {  0,  0,129,162}, { 53, 92}}, //1 idle 2
	{Ourple_ArcMain_Idle3, {  0,  0,129,162}, { 53, 92}}, //2 idle 3
	{Ourple_ArcMain_Idle4, {  0,  0,129,162}, { 53, 92}}, //3 idle 4
	{Ourple_ArcMain_Idle5, {  0,  0,129,162}, { 53, 92}}, //4 idle 5
	{Ourple_ArcMain_Idle6, {  0,  0,129,162}, { 53, 92}}, //5 idle 6
	{Ourple_ArcMain_Idle7, {  0,  0,129,162}, { 53, 92}}, //6 idle 7
	{Ourple_ArcMain_Idle8, {  0,  0,129,162}, { 53, 92}}, //7 idle 8
	
	{Ourple_ArcMain_Left1, {  0,  0,129,162}, { 53, 92}}, //8 left 1
	{Ourple_ArcMain_Left2, {  0,  0,129,162}, { 53, 92}}, //9 left 2
	
	{Ourple_ArcMain_LeftMiss1, {  0,  0,129,162}, { 53, 92}}, //10 left miss 1
	{Ourple_ArcMain_LeftMiss2, {  0,  0,129,162}, { 53, 92}}, //11 left miss 2
	
	{Ourple_ArcMain_LeftAlt1, {  0,  0,129,162}, { 53, 92}}, //12 left alt 1
	{Ourple_ArcMain_LeftAlt2, {  0,  0,129,162}, { 53, 92}}, //13 left alt 2
	
	{Ourple_ArcMain_Down1, {  0,  0,129,162}, { 53, 92}}, //14 down 1
	{Ourple_ArcMain_Down2, {  0,  0,129,162}, { 53, 92}}, //15 down 2
	
	{Ourple_ArcMain_DownMiss1, {  0,  0,129,162}, { 53, 92}}, //16 down miss 1
	{Ourple_ArcMain_DownMiss2, {  0,  0,129,162}, { 53, 92}}, //17 down miss 2
	
	{Ourple_ArcMain_DownAlt1, {  0,  0,129,162}, { 53, 92}}, //18 down alt 1
	{Ourple_ArcMain_DownAlt2, {  0,  0,129,162}, { 53, 92}}, //19 down alt 2
	
	{Ourple_ArcMain_Up1, {  0,  0,129,162}, { 53, 92}}, //20 up 1
	{Ourple_ArcMain_Up2, {  0,  0,129,162}, { 53, 92}}, //21 up 2
	
	{Ourple_ArcMain_UpMiss1, {  0,  0,129,162}, { 53, 92}}, //22 up miss 1
	{Ourple_ArcMain_UpMiss2, {  0,  0,129,162}, { 53, 92}}, //23 up miss 2
	
	{Ourple_ArcMain_UpAlt1, {  0,  0,129,162}, { 53, 92}}, //24 up alt 1
	{Ourple_ArcMain_UpAlt2, {  0,  0,129,162}, { 53, 92}}, //25 up alt 2
	
	{Ourple_ArcMain_Right1, {  0,  0,129,162}, { 53, 92}}, //26 right 1
	{Ourple_ArcMain_Right2, {  0,  0,129,162}, { 53, 92}}, //27 right 2
	
	{Ourple_ArcMain_RightMiss1, {  0,  0,129,162}, { 53, 92}}, //28 right miss 1
	{Ourple_ArcMain_RightMiss2, {  0,  0,129,162}, { 53, 92}}, //29 right miss 2
	
	{Ourple_ArcMain_RightAlt1, {  0,  0,129,162}, { 53, 92}}, //30 right alt 1
	{Ourple_ArcMain_RightAlt2, {  0,  0,129,162}, { 53, 92}}, //31 right alt 2
};
static const CharFrame char_ourple_frame2[] = 
{
	{Ourple_ArcMain_Idle1, {  0,  0,129,162}, { 53, 92}}, //0 idle 1
	{Ourple_ArcMain_Idle2, {  0,  0,129,162}, { 53, 92}}, //1 idle 2
	{Ourple_ArcMain_Idle3, {  0,  0,129,162}, { 53, 92}}, //2 idle 3
	{Ourple_ArcMain_Idle4, {  0,  0,129,162}, { 53, 92}}, //3 idle 4
	{Ourple_ArcMain_Idle5, {  0,  0,129,162}, { 53, 92}}, //4 idle 5
	{Ourple_ArcMain_Idle6, {  0,  0,129,162}, { 53, 92}}, //5 idle 6
	{Ourple_ArcMain_Idle7, {  0,  0,129,162}, { 53, 92}}, //6 idle 7
	{Ourple_ArcMain_Idle8, {  0,  0,129,162}, { 53, 92}}, //7 idle 8
	
	{Ourple_ArcMain_LeftAlt1, {  0,  0,129,162}, { 53, 92}}, //8 left alt 1
	{Ourple_ArcMain_LeftAlt2, {  0,  0,129,162}, { 53, 92}}, //9 left alt 2
	
	{Ourple_ArcMain_LeftMiss1, {  0,  0,129,162}, { 53, 92}}, //10 left miss 1
	{Ourple_ArcMain_LeftMiss2, {  0,  0,129,162}, { 53, 92}}, //11 left miss 2
	
	{Ourple_ArcMain_LeftAlt1, {  0,  0,129,162}, { 53, 92}}, //12 left alt 1
	{Ourple_ArcMain_LeftAlt2, {  0,  0,129,162}, { 53, 92}}, //13 left alt 2
	
	{Ourple_ArcMain_DownAlt1, {  0,  0,129,162}, { 53, 92}}, //14 down alt 1
	{Ourple_ArcMain_DownAlt2, {  0,  0,129,162}, { 53, 92}}, //15 down alt 2
	
	{Ourple_ArcMain_DownMiss1, {  0,  0,129,162}, { 53, 92}}, //16 down miss 1
	{Ourple_ArcMain_DownMiss2, {  0,  0,129,162}, { 53, 92}}, //17 down miss 2
	
	{Ourple_ArcMain_DownAlt1, {  0,  0,129,162}, { 53, 92}}, //18 down alt 1
	{Ourple_ArcMain_DownAlt2, {  0,  0,129,162}, { 53, 92}}, //19 down alt 2
	
	{Ourple_ArcMain_UpAlt1, {  0,  0,129,162}, { 53, 92}}, //20 up alt 1
	{Ourple_ArcMain_UpAlt2, {  0,  0,129,162}, { 53, 92}}, //21 up alt 2
	
	{Ourple_ArcMain_UpMiss1, {  0,  0,129,162}, { 53, 92}}, //22 up miss 1
	{Ourple_ArcMain_UpMiss2, {  0,  0,129,162}, { 53, 92}}, //23 up miss 2
	
	{Ourple_ArcMain_UpAlt1, {  0,  0,129,162}, { 53, 92}}, //24 up alt 1
	{Ourple_ArcMain_UpAlt2, {  0,  0,129,162}, { 53, 92}}, //25 up alt 2
	
	{Ourple_ArcMain_RightAlt1, {  0,  0,129,162}, { 53, 92}}, //26 right alt 1
	{Ourple_ArcMain_RightAlt2, {  0,  0,129,162}, { 53, 92}}, //27 right alt 2
	
	{Ourple_ArcMain_RightMiss1, {  0,  0,129,162}, { 53, 92}}, //28 right miss 1
	{Ourple_ArcMain_RightMiss2, {  0,  0,129,162}, { 53, 92}}, //29 right miss 2
	
	{Ourple_ArcMain_RightAlt1, {  0,  0,129,162}, { 53, 92}}, //30 right alt 1
	{Ourple_ArcMain_RightAlt2, {  0,  0,129,162}, { 53, 92}}, //31 right alt 2
};
static const Animation char_ourple_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{2, (const u8[]){26,27, ASCR_BACK, 1}},             //CharAnim_Left
	{2, (const u8[]){30,31, ASCR_BACK, 1}},       //CharAnim_LeftAlt
	{2, (const u8[]){14,15, ASCR_BACK, 1}},             //CharAnim_Down
	{2, (const u8[]){18,19, ASCR_BACK, 1}},       //CharAnim_DownAlt
	{2, (const u8[]){20,21, ASCR_BACK, 1}},             //CharAnim_Up
	{2, (const u8[]){24,25, ASCR_BACK, 1}},       //CharAnim_UpAlt
	{2, (const u8[]){ 8, 9, ASCR_BACK, 1}},             //CharAnim_Right
	{2, (const u8[]){12,13, ASCR_BACK, 1}},       //CharAnim_RightAlt
	
	{1, (const u8[]){28,29, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){16,17, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){22,23, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){10,11, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

static const Animation char_ourple_anim2[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{2, (const u8[]){ 8, 9, ASCR_BACK, 1}},             //CharAnim_Left
	{2, (const u8[]){12,13, ASCR_BACK, 1}},       //CharAnim_LeftAlt
	{2, (const u8[]){14,15, ASCR_BACK, 1}},             //CharAnim_Down
	{2, (const u8[]){18,19, ASCR_BACK, 1}},       //CharAnim_DownAlt
	{2, (const u8[]){20,21, ASCR_BACK, 1}},             //CharAnim_Up
	{2, (const u8[]){24,25, ASCR_BACK, 1}},       //CharAnim_UpAlt
	{2, (const u8[]){26,27, ASCR_BACK, 1}},             //CharAnim_Right
	{2, (const u8[]){30,31, ASCR_BACK, 1}},       //CharAnim_RightAlt
	
	{1, (const u8[]){ 10,11, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 16,17, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 22,23, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 28,29, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Ourple Guy player functions
void Char_Ourple_SetFrame(void *user, u8 frame)
{
	Char_Ourple *this = (Char_Ourple*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		CharFrame *cframe;

 			if (stage.stage_id == StageId_1_1 && stage.song_step >= 568 && stage.song_step <= 576)
				cframe = &char_ourple_frame2[this->frame = frame];
			else
				cframe = &char_ourple_frame[this->frame = frame];
			
			
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Ourple_Tick(Character *character)
{
	Char_Ourple *this = (Char_Ourple*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_1_1)
	{
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 0)
		{
			this->character.focus_zoom = FIXED_DEC(1577,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1)
		{
			this->character.focus_zoom = FIXED_DEC(261,256);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 6)
		{
			this->character.focus_zoom = FIXED_DEC(1577,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 7)
		{
			this->character.focus_zoom = FIXED_DEC(261,256);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 32)
		{
			this->character.focus_zoom = FIXED_DEC(1577,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 33)
		{
			this->character.focus_zoom = FIXED_DEC(261,256);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 38)
		{
			this->character.focus_zoom = FIXED_DEC(1577,512);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 39)
		{
			this->character.focus_zoom = FIXED_DEC(261,256);
		}
	}
	if (stage.stage_id == StageId_2_1)
	{
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1784)
		{
			this->character.focus_x = FIXED_DEC(-94,1);
			this->character.focus_y = FIXED_DEC(-59,1);
			this->character.health_bar = 0xFFCF323A;
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1924)
		{
			this->character.focus_x = FIXED_DEC(-65,1);
			this->character.focus_y = FIXED_DEC(-37,1);
			this->character.health_bar = 0xFFA357AB;
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 2046)
		{
			this->character.focus_x = FIXED_DEC(-94,1);
			this->character.focus_y = FIXED_DEC(-59,1);
			this->character.health_bar = 0xFFCF323A;
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 2181)
		{
			this->character.focus_x = FIXED_DEC(-65,1);
			this->character.focus_y = FIXED_DEC(-37,1);
			this->character.health_bar = 0xFFA357AB;
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 2372)
		{
			this->character.focus_x = FIXED_DEC(-94,1);
			this->character.focus_y = FIXED_DEC(-59,1);
			this->character.health_bar = 0xFFCF323A;
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 2431)
		{
			this->character.focus_x = FIXED_DEC(-65,1);
			this->character.focus_y = FIXED_DEC(-37,1);
			this->character.health_bar = 0xFFA357AB;
		}
	}
	if (stage.stage_id == StageId_2_3)	
	{
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1344)
		{
			this->character.focus_x = FIXED_DEC(79,1);
			this->character.focus_y = FIXED_DEC(-27,1);
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1408)
		{
			this->character.focus_x = FIXED_DEC(25,1);
			this->character.focus_y = FIXED_DEC(-5,1);
		}
		
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1343)
		{
			this->character.health_bar = 0xFFAF842C;
		}
		if ((stage.flag & STAGE_FLAG_JUST_STEP) && stage.song_step == 1407)
		{
			this->character.health_bar = 0xFFA357AB;
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
	}

	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_Ourple_SetFrame);

	if (stage.stage_id == StageId_2_1)
		Character_DrawFlipped(character, &this->tex, &char_ourple_frame[this->frame]);
	else
		Character_Draw(character, &this->tex, &char_ourple_frame[this->frame]);
}

void Char_Ourple_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Ourple_Free(Character *character)
{
	Char_Ourple *this = (Char_Ourple*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Ourple_New(fixed_t x, fixed_t y)
{
	//Allocate ourple object
	Char_Ourple *this = Mem_Alloc(sizeof(Char_Ourple));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Ourple_New] Failed to allocate ourple object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Ourple_Tick;
	this->character.set_anim = Char_Ourple_SetAnim;
	this->character.free = Char_Ourple_Free;
	if (stage.stage_id == StageId_2_1)
		Animatable_Init(&this->character.animatable, char_ourple_anim);
	else
		Animatable_Init(&this->character.animatable, char_ourple_anim2);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	if (stage.stage_id == StageId_2_1)
	this->character.health_i = 3;
	else
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFFA357AB;
	
	if (stage.stage_id == StageId_1_1)
	{
		this->character.focus_x = FIXED_DEC(39,1);
		this->character.focus_y = FIXED_DEC(-15,1);
		this->character.focus_zoom = FIXED_DEC(261,256);
	}
	if (stage.stage_id == StageId_2_1)
	{	
		this->character.focus_x = FIXED_DEC(-65,1);
		this->character.focus_y = FIXED_DEC(-37,1);
		this->character.focus_zoom = FIXED_DEC(1,1);
	}
	if (stage.stage_id == StageId_2_3)
	{
		this->character.focus_x = FIXED_DEC(25,1);
		this->character.focus_y = FIXED_DEC(-5,1);
		this->character.focus_zoom = FIXED_DEC(597,512);
	}
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\OURPLE.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle1.tim",   //Ourple_ArcMain_Idle1
		"idle2.tim",   //Ourple_ArcMain_Idle2
		"idle3.tim",   //Ourple_ArcMain_Idle3
		"idle4.tim",   //Ourple_ArcMain_Idle4
		"idle5.tim",   //Ourple_ArcMain_Idle5
		"idle6.tim",   //Ourple_ArcMain_Idle6
		"idle7.tim",   //Ourple_ArcMain_Idle7
		"idle8.tim",   //Ourple_ArcMain_Idle8
		"left1.tim",   //Ourple_ArcMain_Left1
		"left2.tim",   //Ourple_ArcMain_Left2
		"leftm1.tim",   //Ourple_ArcMain_LeftMiss1
		"leftm2.tim",   //Ourple_ArcMain_LeftMiss2
		"lefta1.tim",   //Ourple_ArcMain_LeftAlt1
		"lefta2.tim",   //Ourple_ArcMain_LeftAlt2
		"down1.tim",   //Ourple_ArcMain_Down1
		"down2.tim",   //Ourple_ArcMain_Down2
		"downm1.tim",   //Ourple_ArcMain_DownMiss1
		"downm2.tim",   //Ourple_ArcMain_DownMiss2
		"downa1.tim",   //Ourple_ArcMain_DownAlt1
		"downa2.tim",   //Ourple_ArcMain_DownAlt2
		"up1.tim",   //Ourple_ArcMain_Up1
		"up2.tim",   //Ourple_ArcMain_Up2
		"upm1.tim",   //Ourple_ArcMain_UpMiss1
		"upm2.tim",   //Ourple_ArcMain_UpMiss2
		"upa1.tim",   //Ourple_ArcMain_UpAlt1
		"upa2.tim",   //Ourple_ArcMain_UpAlt2
		"right1.tim",   //Ourple_ArcMain_Right1
		"right2.tim",   //Ourple_ArcMain_Right2
		"rightm1.tim",   //Ourple_ArcMain_RightMiss1
		"rightm2.tim",   //Ourple_ArcMain_RightMiss2
		"righta1.tim",   //Ourple_ArcMain_RightAlt1
		"righta2.tim",   //Ourple_ArcMain_RightAlt2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}