/*
 * comm_buffer.c
 *
 *  Created on: Oct 21, 2022
 *      Author: micahbly
 */

// copied and adapted for Infestation! starting April 8, 2024


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "comm_buffer.h"
#include "app.h"
#include "general.h"
#include "text.h"
#include "keyboard.h"
#include "memory.h"
#include "player.h"

// C includes
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// cc65 includes



/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

uint8_t				global_curr_buff_row = 0;
uint8_t				global_curr_weapon_char;		// for status display, the character code for the weapon currently in use by the player

char 				global_comm_buff[COMM_BUFF_SIZE];

char				comm_buff_temp[MAX_ALLOWED_STRING_SIZE];	// temp storage for strings to be displayed in buffer; allows wrapping chars to be imebedded without modifying the source string
char*				global_comm_buffer[COMM_BUFFER_NUM_ROWS];
char**				global_comm_buffer_line[COMM_BUFFER_NUM_ROWS];	// ptp for the comms buffer rows, to allow scrolling


extern char*			global_string_buff1;
// extern char*			global_string_buff2;

//extern bool			global_buffer_vis;
extern Weapon		global_weapon[PLAYER_MAX_WEAPONS];

extern Player*		global_player;
//#pragma zpsym ("global_player");

extern uint16_t				zp_px;
extern uint16_t				zp_py;
extern uint8_t				zp_num_bullets;
extern uint8_t				zp_num_clips;
extern uint8_t				zp_num_warps;
extern uint8_t				zp_speed;
extern uint16_t				zp_points;
extern uint8_t				zp_hp;
extern uint8_t				zp_bullet_dmg;
extern uint8_t				zp_player_dir;
extern uint8_t				zp_lives;

#pragma zpsym ("zp_px");
#pragma zpsym ("zp_py");
#pragma zpsym ("zp_num_bullets");
#pragma zpsym ("zp_num_clips");
#pragma zpsym ("zp_num_warps");
#pragma zpsym ("zp_speed");
#pragma zpsym ("zp_points");
#pragma zpsym ("zp_hp");
#pragma zpsym ("zp_bullet_dmg");
#pragma zpsym ("zp_player_dir");
#pragma zpsym ("zp_lives");

//#pragma zpsym ("global_buffer_vis");
//#pragma zpsym ("global_string_buffer");



/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

// pushes lines "up" by 1, so that line 3 becomes line 2, line 1 becomes line 3
void Buffer_ScrollUp(void);



/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/


// pushes lines "up" by 1, so that line 3 becomes line 2, line 1 becomes line 3
void Buffer_ScrollUp(void)
{
	char*		temp;
	uint8_t		i;
	int8_t		last;
	int8_t		next_to_last;

	last = COMM_BUFFER_NUM_ROWS - 1;
	next_to_last = last - 1;
	temp = *global_comm_buffer_line[last];
	*global_comm_buffer_line[last] = *global_comm_buffer_line[0];
//printf("last=%i, next_to_last=%i \n", last, next_to_last);	
	for (i = 0; i < next_to_last; i++)
	{
		*global_comm_buffer_line[i] = *global_comm_buffer_line[i + 1];
	}
	
	*global_comm_buffer_line[next_to_last] = temp;

	Buffer_RefreshDisplay();
}



/*****************************************************************************/
/*                       Public Function Definitions                         */
/*****************************************************************************/



// Draw the status and message area framework
void Buffer_DrawCommunicationArea(void)
{
	// first clear all lines. 
	// LOGIC: 
	//   The comms buffer is drawn with transparent background over the actual game screen
	//   between games, want to make sure we clear it out
	
	Text_FillBox(		
		COMM_BUFFER_FIRST_COL, COMM_BUFFER_FIRST_ROW, 
		COMM_BUFFER_LAST_COL, COMM_BUFFER_LAST_ROW, 
		CH_SPACE,
		COLOR_BRIGHT_MAGENTA, 
		COLOR_BLACK
	);

	// redraw the only solid background part, which is the single row at the bottom of the screen used for stats
	Text_FillBox(		
		COMM_BUFFER_FIRST_COL, COMM_BUFFER_FIRST_ROW, 
		COMM_BUFFER_LAST_COL, COMM_BUFFER_LAST_ROW, 
		CH_SPACE,
		COLOR_BRIGHT_WHITE, 
		COLOR_BRIGHT_MAGENTA
	);
}


// shows stats from the passed player object in the status area of the screen
// if refresh_background=true, will redraw the background in black and redraw the icons
// COMM_BUFFER_DO_REFRESH and COMM_BUFFER_NO_REFRESH are available
void Buffer_RefreshStatDisplay(bool refresh_background)
{
	//if (!global_buffer_vis) return;	// do nothing if the comms buffer is not supposed to be visible right now
	
	if (refresh_background)
	{
		// redraw the only solid background part, which is the single row at the bottom of the screen used for stats
		Text_FillBox(		
			COMM_BUFFER_FIRST_COL, COMM_BUFFER_FIRST_ROW, 
			COMM_BUFFER_LAST_COL, COMM_BUFFER_LAST_ROW, 
			CH_SPACE,
			COLOR_BRIGHT_WHITE, 
			COLOR_BRIGHT_MAGENTA
		);

		// draw icons individually -- don't need to set color, as we already set it when drawing the background line
		Text_SetCharAtXY(STAT_COL_CLIPS_ICON,	STAT_FIRST_ROW,	CH_CLIP_ICON);
		Text_SetCharAtXY(STAT_COL_BULLETS_ICON,	STAT_FIRST_ROW,	CH_BULLETS_ICON);
		Text_SetCharAtXY(STAT_COL_LIVES_ICON,	STAT_FIRST_ROW,	CH_LIVES_ICON);
		Text_SetCharAtXY(STAT_COL_HP_ICON,		STAT_FIRST_ROW, CH_HP_ICON);
		Text_SetCharAtXY(STAT_COL_WARPS_ICON,	STAT_FIRST_ROW, CH_WARPS_ICON);
	}

	// draw individual stats each with its own string
	Text_DrawStringAtXY(
		STAT_COL_WEAPON_TEXT, STAT_FIRST_ROW, 
		global_weapon[global_player->current_weapon_id_].name_, 
		COLOR_BRIGHT_WHITE, 
		COLOR_BRIGHT_MAGENTA
	);

	sprintf(global_string_buff1, "%02X", zp_num_clips);
	Text_DrawStringAtXY(
		STAT_COL_CLIPS_TEXT, STAT_FIRST_ROW, 
		global_string_buff1, 
		COLOR_BRIGHT_WHITE, 
		COLOR_BRIGHT_MAGENTA
	);

	sprintf(global_string_buff1, "%02X", zp_num_bullets);
	Text_DrawStringAtXY(
		STAT_COL_BULLETS_TEXT, STAT_FIRST_ROW, 
		global_string_buff1, 
		COLOR_BRIGHT_WHITE, 
		COLOR_BRIGHT_MAGENTA
	);

	sprintf(global_string_buff1, "%01X", zp_lives);
	Text_DrawStringAtXY(
		STAT_COL_LIVES_TEXT, STAT_FIRST_ROW, 
		global_string_buff1, 
		COLOR_BRIGHT_WHITE, 
		COLOR_BRIGHT_MAGENTA
	);

	sprintf(global_string_buff1, "%02X", zp_hp);
	Text_DrawStringAtXY(
		STAT_COL_HP_TEXT, STAT_FIRST_ROW, 
		global_string_buff1, 
		COLOR_BRIGHT_WHITE, 
		COLOR_BRIGHT_MAGENTA
	);

	sprintf(global_string_buff1, "%01X", zp_num_warps);
	Text_DrawStringAtXY(
		STAT_COL_WARPS_TEXT, STAT_FIRST_ROW, 
		global_string_buff1, 
		COLOR_BRIGHT_WHITE, 
		COLOR_BRIGHT_MAGENTA
	);

	sprintf(global_string_buff1, "%05u", zp_points);
	Text_DrawStringAtXY(
		STAT_COL_SCORE_TEXT, STAT_FIRST_ROW, 
		global_string_buff1, 
		COLOR_BRIGHT_WHITE, 
		COLOR_BRIGHT_MAGENTA
	);
}


// resets the buffer memory area to spaces, and pushes that change to the screen display
void Buffer_Clear(void)
{
	uint8_t i;

	for (i = 0; i < COMM_BUFFER_NUM_ROWS; i++)
	{
		*(global_comm_buffer[i]) = 0;
	}

	//if (!global_buffer_vis) return;	// do not update screen if the comms buffer is not supposed to be visible right now
	
	Text_FillBox(		
		COMM_BUFFER_FIRST_COL, COMM_BUFFER_FIRST_ROW, 
		COMM_BUFFER_LAST_COL, COMM_BUFFER_LAST_ROW, 
		CH_SPACE, 
		COLOR_BRIGHT_MAGENTA, 
		COLOR_BLACK
	);
	
	// reset the buffer lines-since-you-did-something-yourself count.
	global_curr_buff_row = 0;
}


// transfers all buffer lines to the screen display
void Buffer_RefreshDisplay(void)
{
	uint8_t		i;

	//if (!global_buffer_vis) return;	// do not update screen if the comms buffer is not supposed to be visible right now
	
	Buffer_DrawCommunicationArea();
	
	for (i = 0; i < COMM_BUFFER_NUM_ROWS; i++)
	{
		int8_t		this_len = strlen(*global_comm_buffer_line[i]);
//printf("'%s' \n", *global_comm_buffer_line[i]);	
		
		if ( this_len > COMM_BUFFER_NUM_COLS)
		{
			//DEBUG_OUT(("%s %d: string was too long (%i vs %i): '%s'", __func__, __LINE__, COMM_BUFFER_NUM_COLS, this_len, *global_comm_buffer_line[i]));
			*(global_comm_buffer_line[i][COMM_BUFFER_NUM_COLS]) = 0;
		}
		Text_DrawStringAtXY(
			COMM_BUFFER_FIRST_COL, COMM_BUFFER_FIRST_ROW + i, 
			*global_comm_buffer_line[i],
			COLOR_BRIGHT_MAGENTA, 
			COLOR_BLACK
		);
	}
}


// accepts a message as the bottom most row and displays it, scrolling other lines up
void Buffer_NewMessage(char* the_message)
{
	uint8_t		the_len;
	char*		right_margin;
	char*		start_of_string = comm_buff_temp;

	//DEBUG_OUT(("%s %d: msg='%s'", __func__, __LINE__, the_message));
	
// 	// check that we haven't already displayed 3 lines worth of buffer since user last hit a key
// 	// if we have, give user the blinky and wait for them to hit a key before displaying any more rows
// 	// skip this though if we are in stealth mode
// 	if (global_curr_buff_row >= COMM_BUFFER_NUM_ROWS && global_buffer_vis == true)
// 	{
// 		Buffer_GetUserToHitKey();
// 	}

	// check if this is longer than we can display on one line
	the_len = strlen(the_message);

	// copy the string into the temp buffer so we can add line terminators without modifying source string
	strcpy(start_of_string, the_message);

	while(*start_of_string)
	{
		++global_curr_buff_row;

		if (the_len < COMM_BUFFER_NUM_COLS)
		{
			sprintf(*global_comm_buffer_line[0], "%-*s", COMM_BUFFER_NUM_COLS, start_of_string);
			//DEBUG_OUT(("%s %d: '%s'", __func__, __LINE__, *global_comm_buffer_line[0]));
			//printf("'%s'", *global_comm_buffer_line[0]);
			Buffer_ScrollUp();
			return;
		}

		right_margin = start_of_string + COMM_BUFFER_NUM_COLS;

		while (*right_margin != ' ')
		{
			--right_margin;
		}

		*right_margin = '\0';
		sprintf(*global_comm_buffer_line[0], "%-*s", COMM_BUFFER_NUM_COLS, start_of_string);
		Buffer_ScrollUp();

		the_len -= right_margin - start_of_string + 1; // +1 is for the space
		start_of_string = right_margin + 1;
	}
}


