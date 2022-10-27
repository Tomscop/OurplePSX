/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "menu.h"

#include "mem.h"
#include "main.h"
#include "timer.h"
#include "io.h"
#include "gfx.h"
#include "audio.h"
#include "pad.h"
#include "archive.h"
#include "mutil.h"
#include "network.h"

#include "font.h"
#include "trans.h"
#include "loadscr.h"

#include "stage.h"
#include "save.h"
#include "character/gf.h"

#include "stdlib.h"

static u32 Sounds[3];
static char scoredisp[30];

//Menu state
static struct
{
	//Menu state
	u8 page, next_page;
	boolean page_swap;
	u8 select, next_select;
	
	fixed_t scroll;
	fixed_t trans_time;
	
	//Page specific state
	union
	{
		struct
		{
			fixed_t logo_bump;
			fixed_t fade, fadespd;
		} title;
		struct
		{
			fixed_t fade, fadespd;
		} story;
		struct
		{
			fixed_t back_r, back_g, back_b;
		} freeplay;
	} page_state;
	
	union
	{
		struct
		{
			u8 id, diff;
			boolean story;
		} stage;
	} page_param;
	
	//Menu assets
	Gfx_Tex tex_back, tex_story, tex_title, tex_ourple, tex_checkers, tex_menustuff;
	FontData font_bold, font_arial;
	
	Character *gf; //Title Girlfriend
} menu;

//Internal menu functions
char menu_text_buffer[0x100];

static const char *Menu_LowerIf(const char *text, boolean lower)
{
	//Copy text
	char *dstp = menu_text_buffer;
	if (lower)
	{
		for (const char *srcp = text; *srcp != '\0'; srcp++)
		{
			if (*srcp >= 'A' && *srcp <= 'Z')
				*dstp++ = *srcp | 0x20;
			else
				*dstp++ = *srcp;
		}
	}
	else
	{
		for (const char *srcp = text; *srcp != '\0'; srcp++)
		{
			if (*srcp >= 'a' && *srcp <= 'z')
				*dstp++ = *srcp & ~0x20;
			else
				*dstp++ = *srcp;
		}
	}
	
	//Terminate text
	*dstp++ = '\0';
	return menu_text_buffer;
}

static void Menu_DrawBack(boolean flash, s32 scroll, u8 r0, u8 g0, u8 b0, u8 r1, u8 g1, u8 b1)
{
	RECT back_src = {0, 0, 255, 255};
	RECT back_dst = {0, -scroll - screen.SCREEN_WIDEADD2, screen.SCREEN_WIDTH, screen.SCREEN_WIDTH * 4 / 5};
	
	if (flash || (animf_count & 4) == 0)
		Gfx_DrawTexCol(&menu.tex_back, &back_src, &back_dst, r0, g0, b0);
	else
		Gfx_DrawTexCol(&menu.tex_back, &back_src, &back_dst, r1, g1, b1);
}

static int increase_Story(int length, int thesong)
{
	int result = 0;
	int testresult = 0;

	for (int i = 0; i < length; i++)
	{
		testresult = stage.prefs.savescore[thesong + i][menu.page_param.stage.diff];

		if (testresult == 0)
			return 0;

		result += stage.prefs.savescore[thesong + i][menu.page_param.stage.diff];
	}

	return result * 10;
}

static void Menu_DifficultySelector(s32 x, s32 y)
{
	//Draw difficulty arrows
	static const RECT arrow_src[2][2] = {
		{{224, 64, 16, 32}, {224, 96, 16, 32}}, //left
		{{240, 64, 16, 32}, {240, 96, 16, 32}}, //right
	};
	
	Gfx_BlitTex(&menu.tex_story, &arrow_src[0][(pad_state.held & PAD_LEFT) != 0], x - 40 - 16, y - 16);
	Gfx_BlitTex(&menu.tex_story, &arrow_src[1][(pad_state.held & PAD_RIGHT) != 0], x + 40, y - 16);
	
	//Draw difficulty
	static const RECT diff_srcs[] = {
		{  0, 96, 64, 18},
		{ 64, 96, 80, 18},
		{144, 96, 64, 18},
	};
	
	const RECT *diff_src = &diff_srcs[menu.page_param.stage.diff];
	Gfx_BlitTex(&menu.tex_story, diff_src, x - (diff_src->w >> 1), y - 9 + ((pad_state.press & (PAD_LEFT | PAD_RIGHT)) != 0));
}

//Menu functions
void Menu_Load(MenuPage page)
{
	stage.stage_id = StageId_1_1;
	//Load menu assets
	IO_Data menu_arc = IO_Read("\\MENU\\MENU.ARC;1");
	Gfx_LoadTex(&menu.tex_back,  Archive_Find(menu_arc, "back.tim"),  0);
	Gfx_LoadTex(&menu.tex_story, Archive_Find(menu_arc, "story.tim"), 0);
	Gfx_LoadTex(&menu.tex_title, Archive_Find(menu_arc, "title.tim"), 0);
	Gfx_LoadTex(&menu.tex_ourple, Archive_Find(menu_arc, "ourple.tim"), 0);
	Gfx_LoadTex(&menu.tex_checkers, Archive_Find(menu_arc, "checkers.tim"), 0);
	Gfx_LoadTex(&menu.tex_menustuff, Archive_Find(menu_arc, "menustuf.tim"), 0);
	Mem_Free(menu_arc);
	
	FontData_Load(&menu.font_bold, Font_Bold);
	FontData_Load(&menu.font_arial, Font_Arial);
	
	//Initialize menu state
	menu.select = menu.next_select = 0;
	
	switch (menu.page = menu.next_page = page)
	{
		case MenuPage_Opening:
			//Do this here so timing is less reliant on VSync
			break;
		default:
			break;
	}
	menu.page_swap = true;
	
	menu.trans_time = 0;
	Trans_Clear();
	
	stage.song_step = 0;

	// to load
	CdlFILE file;
    IO_FindFile(&file, "\\SOUNDS\\SCROLL.VAG;1");
    u32 *data = IO_ReadFile(&file);
    Sounds[0] = Audio_LoadVAGData(data, file.size);
    Mem_Free(data);

	IO_FindFile(&file, "\\SOUNDS\\CONFIRM.VAG;1");
    data = IO_ReadFile(&file);
    Sounds[1] = Audio_LoadVAGData(data, file.size);
    Mem_Free(data);

	IO_FindFile(&file, "\\SOUNDS\\CANCEL.VAG;1");
    data = IO_ReadFile(&file);
    Sounds[2] = Audio_LoadVAGData(data, file.size);
    Mem_Free(data);
	
	//Set background colour
	Gfx_SetClear(0, 0, 0);
}

void Menu_Unload(void)
{
	//Free title Girlfriend
	Character_Free(menu.gf);
}

void Menu_ToStage(StageId id, StageDiff diff, boolean story)
{
	menu.next_page = MenuPage_Stage;
	menu.page_param.stage.id = id;
	menu.page_param.stage.story = story;
	menu.page_param.stage.diff = diff;
	Trans_Start();
}

void Menu_Tick(void)
{
	//Clear per-frame flags
	stage.flag &= ~STAGE_FLAG_JUST_STEP;
	
	//Get song position
	u16 next_step = Audio_TellXA_Milli() / 147; //100 BPM
	if (next_step != stage.song_step)
	{
		if (next_step >= stage.song_step)
			stage.flag |= STAGE_FLAG_JUST_STEP;
		stage.song_step = next_step;
	}
	
	//Handle transition out
	if (Trans_Tick())
	{
		//Change to set next page
		menu.page_swap = true;
		menu.page = menu.next_page;
		menu.select = menu.next_select;
	}
	
	//Menu music
	//if (menu.page == MenuPage_Title)
	//	Audio_PlayXA_Track(XA_Title, 0x40, 2, 1);
	//else if ((menu.page == MenuPage_Main) || (menu.page == MenuPage_Credits) || (menu.page == MenuPage_Options))
		Audio_PlayXA_Track(XA_MainMenu, 0x40, 0, 1);
	//else if (menu.page == MenuPage_Freeplay)
	//	Audio_PlayXA_Track(XA_Freeplay, 0x40, 3, 1);
	Audio_WaitPlayXA();
	
	//Tick menu page
	MenuPage exec_page;
	switch (exec_page = menu.page)
	{
		case MenuPage_Opening:
		{
			menu.page = menu.next_page = MenuPage_Title;
			menu.page_swap = true;
		}
	//Fallthrough
		case MenuPage_Title:
		{
			//Initialize page
			if (menu.page_swap)
			{
				menu.page_state.title.logo_bump = (FIXED_DEC(7,1) / 24) - 1;
				menu.page_state.title.fade = FIXED_DEC(255,1);
				menu.page_state.title.fadespd = FIXED_DEC(90,1);
			}
			
			//Draw white fade
			if (menu.page_state.title.fade > 0)
			{
				RECT flash = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
				u8 flash_col = menu.page_state.title.fade >> FIXED_SHIFT;
				Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 1);
				menu.page_state.title.fade -= FIXED_MUL(menu.page_state.title.fadespd, timer_dt);
			}
			
			//Go to main menu when start is pressed
			if (menu.trans_time > 0 && (menu.trans_time -= timer_dt) <= 0)
				Trans_Start();
			
			if ((pad_state.press & PAD_START) && menu.next_page == menu.page && Trans_Idle())
			{
				//play confirm sound
				Audio_PlaySound(Sounds[1], 0x3fff);
				menu.trans_time = FIXED_UNIT;
				menu.page_state.title.fade = FIXED_DEC(255,1);
				menu.page_state.title.fadespd = FIXED_DEC(300,1);
				menu.next_page = MenuPage_Main;
				menu.next_select = 0;
			}
			
			//Draw Friday Night Funkin' logo
			if ((stage.flag & STAGE_FLAG_JUST_STEP) && (stage.song_step & 0x3) == 0 && menu.page_state.title.logo_bump == 0)
				menu.page_state.title.logo_bump = (FIXED_DEC(7,1) / 24) - 1;
			
			static const fixed_t logo_scales[] = {
				FIXED_DEC(1,1),
				FIXED_DEC(101,100),
				FIXED_DEC(102,100),
				FIXED_DEC(103,100),
				FIXED_DEC(105,100),
				FIXED_DEC(110,100),
				FIXED_DEC(97,100),
			};
			fixed_t logo_scale = logo_scales[(menu.page_state.title.logo_bump * 24) >> FIXED_SHIFT];
			u32 x_rad = (logo_scale * (176 >> 1)) >> FIXED_SHIFT;
			u32 y_rad = (logo_scale * (137 >> 1)) >> FIXED_SHIFT;
			
			RECT logo_src = {0, 0, 176, 137};
			RECT logo_dst = {
				100 - x_rad + (screen.SCREEN_WIDEADD2 >> 1),
				68 - y_rad,
				x_rad << 1,
				y_rad << 1
			};
			Gfx_DrawTex(&menu.tex_title, &logo_src, &logo_dst);
			
			if (menu.page_state.title.logo_bump > 0)
				if ((menu.page_state.title.logo_bump -= timer_dt) < 0)
					menu.page_state.title.logo_bump = 0;
			
			//Draw "Press Start to Begin"
			if (menu.next_page == menu.page)
			{
				RECT press_src = {0, 152, 256, 32};
				Gfx_BlitTex(&menu.tex_title, &press_src, (screen.SCREEN_WIDTH - 256) / 2, screen.SCREEN_HEIGHT - 48);
			}
			else
			{
				//Flash white
				RECT press_src = {0, (animf_count & 1) ? 184 : 152, 256, 32};
				Gfx_BlitTex(&menu.tex_title, &press_src, (screen.SCREEN_WIDTH - 256) / 2, screen.SCREEN_HEIGHT - 48);
			}
			
			//Draw Ourple
			RECT ourple_src = {  0,  0, 91,255};
			RECT ourple_dst = {
				234,
				0,
				86,
				240
			};
			Gfx_DrawTex(&menu.tex_ourple, &ourple_src, &ourple_dst);
			break;
		}
		case MenuPage_Main:
		{
			static const char *menu_options[] = {
				"STORY MODE",
				"FREEPLAY",
				"CREDITS",
				"OPTIONS",
			};
			
			//Initialize page
			if (menu.page_swap)
				menu.scroll = menu.select * FIXED_DEC(12,1);
				
			
			//Draw version identification
			menu.font_bold.draw(&menu.font_bold,
				"PSXFUNKIN BY CUCKYDEV",
				screen.SCREEN_WIDTH2,
				screen.SCREEN_HEIGHT - 32,
				FontAlign_Center
			);
			
			//Handle option and selection
			if (menu.trans_time > 0 && (menu.trans_time -= timer_dt) <= 0)
				Trans_Start();
			
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Select option if cross is pressed
				if (pad_state.press & (PAD_START | PAD_CROSS))
				{
					//play confirm sound
					Audio_PlaySound(Sounds[1], 0x3fff);
					switch (menu.select)
					{
						case 0: //Story Mode
							menu.next_page = MenuPage_Story;
							break;
						case 1: //Freeplay
							menu.next_page = MenuPage_Freeplay;
							break;
						case 2: //Credits
							menu.next_page = MenuPage_Credits;
							break;
						case 3: //Options
							menu.next_page = MenuPage_Options;
							break;
					}
					menu.next_select = 0;
					menu.trans_time = FIXED_UNIT;
				}
				
				//Return to title screen if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					//play cancel sound
					Audio_PlaySound(Sounds[2], 0x3fff);
					menu.next_page = MenuPage_Title;
					Trans_Start();
				}
			}
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(12,1);

			menu.scroll += (next_scroll - menu.scroll) >> 2;
			
			if (menu.next_page == menu.page || menu.next_page == MenuPage_Title)
			{
				//Draw all options
				for (u8 i = 0; i < COUNT_OF(menu_options); i++)
				{
					menu.font_bold.draw(&menu.font_bold,
						Menu_LowerIf(menu_options[i], menu.select != i),
						12,
						screen.SCREEN_HEIGHT2 + (i << 5) - 48 - (menu.scroll >> FIXED_SHIFT),
						FontAlign_Left
					);
				}
			}
			else if (animf_count & 2)
			{
				//Draw selected option
				menu.font_bold.draw(&menu.font_bold,
					menu_options[menu.select],
					12,
					screen.SCREEN_HEIGHT2 + (menu.select << 5) - 48 - (menu.scroll >> FIXED_SHIFT),
					FontAlign_Left
				);
			}
			
			//Draw Menu Stuff
			RECT menustuff_src = {  0,  0,252,240};
			RECT menustuff_dst = {
				0,
				0,
				screen.SCREEN_WIDTH,
				screen.SCREEN_HEIGHT
			};
			Gfx_DrawTex(&menu.tex_menustuff, &menustuff_src, &menustuff_dst);
			
			//Draw Checkers
			RECT checkers_src = {  0,  0,252,240};
			RECT checkers_dst = {
				0,
				0,
				screen.SCREEN_WIDTH,
				screen.SCREEN_HEIGHT
			};
			Gfx_BlendTex(&menu.tex_checkers, &checkers_src, &checkers_dst, 1);
			
			//Draw background
			Menu_DrawBack(
				menu.next_page == menu.page || menu.next_page == MenuPage_Title,
				menu.scroll >> (FIXED_SHIFT + 3),
				253 >> 1, 231 >> 1, 113 >> 1,
				253 >> 1, 113 >> 1, 155 >> 1
			);
			break;
		}
		case MenuPage_Story:
		{
			menu.next_page = MenuPage_Stage;
			menu.page_param.stage.id = StageId_1_1;
			menu.page_param.stage.story = true;
			menu.page_param.stage.diff = StageDiff_Normal;
			Trans_Start();
			break;
		}
		case MenuPage_Freeplay:
		{
			static const struct
			{
				StageId stage;
				u32 col;
				const char *text;
			} menu_options[] = {
				{StageId_1_1, 0xFF9271FD, "GUY"},
				{StageId_1_2, 0xFF9271FD, "MIDNIGHT"},
				{StageId_1_3, 0xFF9271FD, "TERMINATED"},
				{StageId_1_4, 0xFF9271FD, "LURKING"},
				{StageId_2_1, 0xFF223344, "LORE"},
				{StageId_2_2, 0xFF223344, "BLUBBER"},
				{StageId_2_3, 0xFF223344, "GOLDEN"},
				{StageId_3_1, 0xFF941653, "PERFORMANCE"},
				{StageId_3_2, 0xFF941653, "BITE"},
				{StageId_3_3, 0xFF941653, "TRAPPED"},
				{StageId_4_1, 0xFFFC96D7, "GO FISH"},
				{StageId_4_2, 0xFFFC96D7, "WATCHFUL"},
				{StageId_4_3, 0xFFFC96D7, "RESTLESS"},
				{StageId_4_4, 0xFFFC96D7, "BEATBOX"},
				{StageId_5_1, 0xFFA0D1FF, "SHOWTIME"},
				{StageId_5_2, 0xFFA0D1FF, "MAN"},
				{StageId_5_3, 0xFFA0D1FF, "FOLLOWED"},
				{StageId_6_1, 0xFFFF78BF, "FAZFUCK NEWS"},
				{StageId_6_2, 0xFFFF78BF, "CRIMINAL"},
				{StageId_6_3, 0xFFFF78BF, "MILLER"},
			};

			menu.font_arial.draw(&menu.font_arial,
				scoredisp,
				150,
				screen.SCREEN_HEIGHT / 2 - 75,
				FontAlign_Left
			);

			sprintf(scoredisp, "PERSONAL BEST: %d", (stage.prefs.savescore[menu_options[menu.select].stage][menu.page_param.stage.diff] > 0) ? stage.prefs.savescore[menu_options[menu.select].stage][menu.page_param.stage.diff] * 10 : 0);

			//Initialize page
			if (menu.page_swap)
			{
				menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + screen.SCREEN_HEIGHT2,1);
				menu.page_param.stage.diff = StageDiff_Normal;
				menu.page_state.freeplay.back_r = FIXED_DEC(255,1);
				menu.page_state.freeplay.back_g = FIXED_DEC(255,1);
				menu.page_state.freeplay.back_b = FIXED_DEC(255,1);
			}

			//Draw page label
			menu.font_bold.draw(&menu.font_bold,
				"FREEPLAY",
				16,
				screen.SCREEN_HEIGHT - 32,
				FontAlign_Left
			);
			
			//Draw difficulty selector
			Menu_DifficultySelector(screen.SCREEN_WIDTH - 100, screen.SCREEN_HEIGHT2 - 48);
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Select option if cross is pressed
				if (pad_state.press & (PAD_START | PAD_CROSS))
				{
					//play confirm sound
					Audio_PlaySound(Sounds[1], 0x3fff);
					menu.next_page = MenuPage_Stage;
					menu.page_param.stage.id = menu_options[menu.select].stage;
					menu.page_param.stage.story = false;
					Trans_Start();
				}
				
				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					//play cancel sound
					Audio_PlaySound(Sounds[2], 0x3fff);
					menu.next_page = MenuPage_Main;
					menu.next_select = 1; //Freeplay
					Trans_Start();
				}
			}
	
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(24,1);
			menu.scroll += (next_scroll - menu.scroll) >> 4;
			
			for (u8 i = 0; i < COUNT_OF(menu_options); i++)
			{
				//Get position on screen
				s32 y = (i * 24) - 8 - (menu.scroll >> FIXED_SHIFT);
				if (y <= -screen.SCREEN_HEIGHT2 - 8)
					continue;
				if (y >= screen.SCREEN_HEIGHT2 + 8)
					break;
				
				//Draw text
				menu.font_bold.draw(&menu.font_bold,
					Menu_LowerIf(menu_options[i].text, menu.select != i),
					48 + (y >> 2),
					screen.SCREEN_HEIGHT2 + y - 8,
					FontAlign_Left
				);
			}
			
			//Draw background
			fixed_t tgt_r = (fixed_t)((menu_options[menu.select].col >> 16) & 0xFF) << FIXED_SHIFT;
			fixed_t tgt_g = (fixed_t)((menu_options[menu.select].col >>  8) & 0xFF) << FIXED_SHIFT;
			fixed_t tgt_b = (fixed_t)((menu_options[menu.select].col >>  0) & 0xFF) << FIXED_SHIFT;
			
			menu.page_state.freeplay.back_r += (tgt_r - menu.page_state.freeplay.back_r) >> 4;
			menu.page_state.freeplay.back_g += (tgt_g - menu.page_state.freeplay.back_g) >> 4;
			menu.page_state.freeplay.back_b += (tgt_b - menu.page_state.freeplay.back_b) >> 4;
			
			Menu_DrawBack(
				true,
				8,
				menu.page_state.freeplay.back_r >> (FIXED_SHIFT + 1),
				menu.page_state.freeplay.back_g >> (FIXED_SHIFT + 1),
				menu.page_state.freeplay.back_b >> (FIXED_SHIFT + 1),
				0, 0, 0
			);
			break;
		}
		case MenuPage_Credits:
		{
			static const struct
			{
				StageId stage;
				const char *text;
				boolean difficulty;
			} menu_options[] = {
				{StageId_1_1, "PORT DEV", false},
				{StageId_1_1, "    PICHUMANTEN", false},
				{StageId_1_1, "HELPING ME AND STUFF", false},
				{StageId_1_1, "    IGORSOU", false},
				{StageId_1_1, "    UNSTOPABLE", false},
				{StageId_1_1, "PSXFUNKIN DEV", false},
				{StageId_1_1, "    CUCKYDEV", false},
				{StageId_1_1, "UNSTOPABLE FORK DEVS", false},
				{StageId_1_1, "    UNSTOPABLE", false},
				{StageId_1_1, "    IGORSOU", false},
				{StageId_1_1, "    SPICYJPEG", false},
				{StageId_1_1, "    SPARK", false},
				{StageId_1_1, "GETTING ME PERMISSION", false},
				{StageId_1_1, "    MRRUMBLEROSES", false},
				{StageId_1_1, "OTHER COOL PEOPLE", false},
				{StageId_1_1, "    LUCKY", false},
				{StageId_1_1, "    GEYT", false},
				{StageId_1_1, "    DREAMCASTNICK", false},
				{StageId_1_1, "    NINTENDOBRO", false},
				{StageId_1_1, "OURPLE GUY TEAM", false},
				{StageId_1_1, "    KIWIQUEST", false},
				{StageId_1_1, "    LOSSARQUO", false},
				{StageId_1_1, "    COLD VEE", false},
				{StageId_1_1, "    VISPLOO", false},
				{StageId_1_1, "    EATEYREE", false},
				{StageId_1_1, "    HEADDZO", false},
				{StageId_1_1, "    FINDJUNO", false},
				{StageId_1_1, "    ZEROH", false},
				{StageId_1_1, "    EMI", false},
				{StageId_1_1, "    LUWIGI", false},
				{StageId_1_1, "    MANU", false},
				{StageId_1_1, "    MARISSA", false},
				{StageId_1_1, "    GALXE", false},
				{StageId_1_1, "    JUSTISAAC", false},
				{StageId_1_1, "    JAYLIRIAH", false},
				{StageId_1_1, "    JEFF", false},
				{StageId_1_1, "    SALAMIPASTE", false},
				{StageId_1_1, "    CASE", false},
				{StageId_1_1, "    POINTYESM", false},
				{StageId_1_1, "    POLYFIELD", false},
				{StageId_1_1, "    JONSPEEDARTS", false},
				{StageId_1_1, "    ROTTY", false},
				{StageId_1_1, "    SNOOB", false},
				{StageId_1_1, "    ITSMEBLUEBERRY", false},
				{StageId_1_1, "    GIBZ", false},
				{StageId_1_1, "CREATOR OF FNAF", false},
				{StageId_1_1, "    SCOTT CAWTHON", false},
				{StageId_1_1, "GUEST APPEARANCES", false},
				{StageId_1_1, "    MATPAT", false},
				{StageId_1_1, "    MARKIPLIER", false},
				{StageId_1_1, "    FAT JONES", false},
			};
			    
			//Initialize page
			if (menu.page_swap)
			{
				menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + screen.SCREEN_HEIGHT2,1);
				menu.page_param.stage.diff = StageDiff_Normal;
			}
			
			//Draw page label
			menu.font_bold.draw(&menu.font_bold,
				"CREDITS",
				16,
				screen.SCREEN_HEIGHT - 32,
				FontAlign_Left
			);
			
			//Draw difficulty selector
			if (menu_options[menu.select].difficulty)
				Menu_DifficultySelector(screen.SCREEN_WIDTH - 100, screen.SCREEN_HEIGHT2 - 48);
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					//play cancel sound
					Audio_PlaySound(Sounds[2], 0x3fff);
					menu.next_page = MenuPage_Main;
					menu.next_select = 2; //Credits
					Trans_Start();
				}
			}
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(24,1);
			menu.scroll += (next_scroll - menu.scroll) >> 4;
			
			for (u8 i = 0; i < COUNT_OF(menu_options); i++)
			{
				//Get position on screen
				s32 y = (i * 24) - 8 - (menu.scroll >> FIXED_SHIFT);
				if (y <= -screen.SCREEN_HEIGHT2 - 8)
					continue;
				if (y >= screen.SCREEN_HEIGHT2 + 8)
					break;
				
				//Draw text
				menu.font_bold.draw(&menu.font_bold,
					Menu_LowerIf(menu_options[i].text, menu.select != i),
					48 + (y >> 2),
					screen.SCREEN_HEIGHT2 + y - 8,
					FontAlign_Left
				);
			}
			
			//Draw background
			Menu_DrawBack(
				true,
				8,
				197 >> 1, 240 >> 1, 95 >> 1,
				0, 0, 0
			);
			break;
		}
		case MenuPage_Options:
		{
			static const char *gamemode_strs[] = {"NORMAL", "SWAP", "TWO PLAYER"};
			static const struct
			{
				enum
				{
					OptType_Boolean,
					OptType_Enum,
				} type;
				const char *text;
				void *value;
				union
				{
					struct
					{
						int a;
					} spec_boolean;
					struct
					{
						s32 max;
						const char **strs;
					} spec_enum;
				} spec;
			} menu_options[] = {
				{OptType_Boolean, "GHOST TAP", &stage.prefs.ghost, {.spec_boolean = {0}}},
				{OptType_Boolean, "DOWNSCROLL", &stage.prefs.downscroll, {.spec_boolean = {0}}},
				{OptType_Boolean, "MIDDLESCROLL", &stage.prefs.middlescroll, {.spec_boolean = {0}}},
				{OptType_Boolean, "FOLLOWED EYES", &stage.prefs.eyes, {.spec_boolean = {0}}},
				{OptType_Boolean, "CRT SCANLINE", &stage.prefs.scanline, {.spec_boolean = {0}}},
				{OptType_Boolean, "SHOW SONG TIME", &stage.prefs.songtimer, {.spec_boolean = {0}}},
				{OptType_Boolean, "MISS SOUNDS", &stage.prefs.sfxmiss, {.spec_boolean = {0}}},
				{OptType_Boolean, "PAL REFRESH RATE", &stage.prefs.palmode, {.spec_boolean = {0}}},
				{OptType_Boolean, "BOTPLAY", &stage.prefs.botplay, {.spec_boolean = {0}}},
				{OptType_Boolean, "PRACTICE MODE", &stage.prefs.practice, {.spec_boolean = {0}}},
				{OptType_Boolean, "DEBUG MODE", &stage.prefs.debug, {.spec_boolean = {0}}},
				//{OptType_Enum,    "GAMEMODE", &stage.mode, {.spec_enum = {COUNT_OF(gamemode_strs), gamemode_strs}}},
			};
			if (menu.select == 7 && pad_state.press & (PAD_CROSS | PAD_LEFT | PAD_RIGHT))
				stage.pal_i = 1;

			if (stage.mode == StageMode_2P)
				stage.prefs.middlescroll = false;
			
			//Initialize page
			if (menu.page_swap)
				menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + screen.SCREEN_HEIGHT2,1);
			
			RECT save_src = {0, 121, 55, 7};
			RECT save_dst = {screen.SCREEN_WIDTH / 2 + 30 - (121 / 2), screen.SCREEN_HEIGHT - 30, 53 * 2, 7 * 2};
			Gfx_DrawTex(&menu.tex_story, &save_src, &save_dst);

			//Draw page label
			menu.font_bold.draw(&menu.font_bold,
				"OPTIONS",
				16,
				screen.SCREEN_HEIGHT - 32,
				FontAlign_Left
			);
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Handle option changing
				switch (menu_options[menu.select].type)
				{
					case OptType_Boolean:
						if (pad_state.press & (PAD_CROSS | PAD_LEFT | PAD_RIGHT))
							*((boolean*)menu_options[menu.select].value) ^= 1;
						break;
					case OptType_Enum:
						if (pad_state.press & PAD_LEFT)
							if (--*((s32*)menu_options[menu.select].value) < 0)
								*((s32*)menu_options[menu.select].value) = menu_options[menu.select].spec.spec_enum.max - 1;
						if (pad_state.press & PAD_RIGHT)
							if (++*((s32*)menu_options[menu.select].value) >= menu_options[menu.select].spec.spec_enum.max)
								*((s32*)menu_options[menu.select].value) = 0;
						break;
				}
				
				if (pad_state.press & PAD_SELECT)
					writeSaveFile();

				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					//play cancel sound
					Audio_PlaySound(Sounds[2], 0x3fff);
					menu.next_page = MenuPage_Main;
					menu.next_select = 3; //Options
					Trans_Start();
				}
			}
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(24,1);
			menu.scroll += (next_scroll - menu.scroll) >> 4;
			
			for (u8 i = 0; i < COUNT_OF(menu_options); i++)
			{
				//Get position on screen
				s32 y = (i * 24) - 8 - (menu.scroll >> FIXED_SHIFT);
				if (y <= -screen.SCREEN_HEIGHT2 - 8)
					continue;
				if (y >= screen.SCREEN_HEIGHT2 + 8)
					break;
				
				//Draw text
				char text[0x80];
				switch (menu_options[i].type)
				{
					case OptType_Boolean:
						sprintf(text, "%s %s", menu_options[i].text, *((boolean*)menu_options[i].value) ? "ON" : "OFF");
						break;
					case OptType_Enum:
						sprintf(text, "%s %s", menu_options[i].text, menu_options[i].spec.spec_enum.strs[*((s32*)menu_options[i].value)]);
						break;
				}
				menu.font_bold.draw(&menu.font_bold,
					Menu_LowerIf(text, menu.select != i),
					48 + (y >> 2),
					screen.SCREEN_HEIGHT2 + y - 8,
					FontAlign_Left
				);
			}
			
			//Draw background
			Menu_DrawBack(
				true,
				8,
				253 >> 1, 113 >> 1, 155 >> 1,
				0, 0, 0
			);
			break;
		}
		case MenuPage_Stage:
		{
			//Unload menu state
			Menu_Unload();
			
			//Load new stage
			LoadScr_Start();
			Stage_Load(menu.page_param.stage.id, menu.page_param.stage.diff, menu.page_param.stage.story);
			gameloop = GameLoop_Stage;
			LoadScr_End();
			break;
		}
		default:
			break;
	}
	
	//Clear page swap flag
	menu.page_swap = menu.page != exec_page;
}
