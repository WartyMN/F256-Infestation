/*
 * comm_buffer.h
 *
 *  Created on: Oct 21, 2022
 *      Author: micahbly
 */

// copied and adapted for Infestation! starting April 8, 2024


#ifndef COMM_BUFFER_H_
#define COMM_BUFFER_H_



/* about this class: Buffer
 *
 * This incorporates the overall game world.
 *
 * differences from Lich King and f/term/etc:
 *   For a 40x30 screen, not 80x30 or 80x60
 *   buffer rolls up into the screen, and fades away
 *   single-line stat bar at bottom of screen
 *
 *** things this class needs to be able to do
 *
 * display player status in a text overlay to the game screen
 * show weapon name, # clips for that weapon, # bullets in current clip, player lives (icon?), player HP (icon?), short messages
 *
 *
 *** things objects of this class have
 *
 * 
 *
 */


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "app.h"

// C includes
#include <stdbool.h>

// cc65 includes



/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/


// comms area inner box
#define COMM_BUFFER_NUM_COLS			(SCREEN_NUM_COLS - 2) // account for box chars
#define COMM_BUFFER_NUM_ROWS			4
#define COMM_AREA_FIRST_ROW				((SCREEN_NUM_ROWS - COMM_BUFFER_NUM_ROWS) - 2)	// -2 so it starts/ends above the final row, which is the status line.
#define COMM_AREA_LAST_ROW				(COMM_AREA_FIRST_ROW + COMM_BUFFER_NUM_ROWS)
#define COMM_BUFFER_FIRST_COL			1	// allow for 1 col of box draw chars
#define COMM_BUFFER_LAST_COL			(COMM_BUFFER_NUM_COLS)	// allow for 1 col of box draw chars
#define COMM_BUFFER_FIRST_ROW			(COMM_AREA_FIRST_ROW + 1) // account for top box draw chars
#define COMM_BUFFER_LAST_ROW			((COMM_BUFFER_FIRST_ROW + COMM_BUFFER_NUM_ROWS) - 1) // account for box draw chars and row of stats
#define COMM_BUFF_SIZE					(COMM_BUFFER_NUM_ROWS * (COMM_BUFFER_NUM_COLS + 1))

// comms buffer box
#define COMM_BUFFER_BOX_NUM_COLS		(COMM_BUFFER_NUM_COLS + 2)	// with box draw chars
#define COMM_BUFFER_BOX_NUM_ROWS		(COMM_BUFFER_NUM_ROWS + 2)	// with box draw chars
#define COMM_BUFFER_BOX_FIRST_COL		(COMM_BUFFER_FIRST_COL - 1)
#define COMM_BUFFER_BOX_LAST_COL		(COMM_BUFFER_LAST_COL + 1)
#define COMM_BUFFER_BOX_FIRST_ROW		(COMM_BUFFER_FIRST_ROW - 1)
#define COMM_BUFFER_BOX_LAST_ROW		(COMM_BUFFER_LAST_ROW + 1)

// status display box (under comms area box)
#define STAT_BOX_FIRST_ROW		(COMM_BUFFER_BOX_LAST_ROW)
#define STAT_BOX_LAST_ROW		(STAT_BOX_FIRST_ROW + 2)
#define STAT_BOX_NUM_COLS		COMM_BUFFER_BOX_NUM_COLS	// with box draw chars
#define STAT_BOX_NUM_ROWS		((STAT_BOX_LAST_ROW - STAT_BOX_FIRST_ROW) + 1)
#define STAT_BOX_FIRST_COL		COMM_BUFFER_BOX_FIRST_COL
#define STAT_BOX_LAST_COL		COMM_BUFFER_BOX_LAST_COL

// status display
#define STAT_NUM_ROWS			1							// single-row stats display
#define STAT_FIRST_ROW			(COMM_AREA_LAST_ROW + 1)	// directly under last row of comms buffer
#define STAT_LAST_ROW			STAT_FIRST_ROW				// only 1 row
#define STAT_LAST_COL			(SCREEN_NUM_COLS - 1)
#define STAT_FIRST_COL			0


// // status display: rows and cols are offsets from STAT_FIRST_COL/STAT_FIRST_ROW
// #define STAT_ROW_1			(STAT_FIRST_ROW + 0)
// #define STAT_ROW_2			(STAT_FIRST_ROW + 1)
// #define STAT_ROW_3			(STAT_FIRST_ROW + 2)
// #define STAT_COL_1			(STAT_FIRST_COL + 0)
// #define STAT_COL_2			(STAT_FIRST_COL + 6)

#define STAT_COL_WEAPON_ICON	(0)
#define STAT_COL_WEAPON_TEXT	(0)

#define STAT_COL_CLIPS_ICON		(STAT_COL_WEAPON_TEXT + 17)
#define STAT_COL_CLIPS_TEXT		(STAT_COL_CLIPS_ICON + 1)

#define STAT_COL_BULLETS_ICON	(STAT_COL_CLIPS_TEXT + 3)
#define STAT_COL_BULLETS_TEXT	(STAT_COL_BULLETS_ICON + 1)

#define STAT_COL_LIVES_ICON		(STAT_COL_BULLETS_TEXT + 3)
#define STAT_COL_LIVES_TEXT		(STAT_COL_LIVES_ICON + 1)

#define STAT_COL_HP_ICON		(STAT_COL_LIVES_TEXT + 2)
#define STAT_COL_HP_TEXT		(STAT_COL_HP_ICON + 1)

#define STAT_COL_WARPS_ICON		(STAT_COL_HP_TEXT + 3)
#define STAT_COL_WARPS_TEXT		(STAT_COL_WARPS_ICON + 1)

#define STAT_COL_SCORE_ICON		(STAT_COL_WARPS_TEXT + 2)
#define STAT_COL_SCORE_TEXT		(STAT_COL_SCORE_ICON + 0)

// status icons
#define CH_HP_ICON				252	// custom char only in tech-4-infest - regular heart shifted 1 px left
#define CH_BULLETS_ICON			253	// custom char only in tech-4-infest
#define CH_CLIP_ICON			254	// custom char only in tech-4-infest
#define CH_LIVES_ICON			255	// custom char only in tech-4-infest
#define CH_WARPS_ICON			87	// just a capital 'W'
#define CH_SPACE				32

// parameters for functions
#define COMM_BUFFER_DO_REFRESH			true
#define COMM_BUFFER_NO_REFRESH			false

/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// Draw the status and message area framework
void Buffer_DrawCommunicationArea(void);

// shows stats from the passed player object in the status area of the screen
// if refresh_background=true, will redraw the background in black and redraw the icons
void Buffer_RefreshStatDisplay(bool refresh_background);

// fake allocs the buffer memory area. does not change screen display
void Buffer_Initialize(void);

// resets the buffer memory area to spaces, and pushes that change to the screen display
void Buffer_Clear(void);

// transfers all buffer lines to the screen display
void Buffer_RefreshDisplay(void);

// accepts a message as the bottom most row and displays it, scrolling other lines up
void Buffer_NewMessage(char* the_message);




#endif /* COMM_BUFFER_H_ */
