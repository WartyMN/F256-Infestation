/*
 * screen.c
 *
 *  Created on: Jan 11, 2023
 *      Author: micahbly
 *
 *  Routines for drawing and updating the UI elements
 *
 */



/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "screen.h"
#include "app.h"
#include "general.h"
#include "kernel.h"
#include "keyboard.h"
#include "memory.h"
#include "sys.h"
#include "text.h"
#include "strings.h"

// C includes
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// F256 includes
#include "f256.h"




/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/



/*****************************************************************************/
/*                           File-scope Variables                            */
/*****************************************************************************/

#pragma data-name ("OVERLAY_SCREEN")


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

extern char*				global_string[NUM_STRINGS];
extern char*				global_string_buff1;
// extern char*				global_string_buff2;


extern uint8_t				zp_bank_num;
extern uint8_t				io_bank_value_kernel;	// stores value for the physical bank pointing to C000-DFFF whenever we change it, so we can restore it.

#pragma zpsym ("zp_bank_num");


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

void Screen_DrawUI(void);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

// clear screen and draw main UI
void Screen_DrawUI(void)
{
	
	Text_ClearScreen(APP_FOREGROUND_COLOR, APP_BACKGROUND_COLOR);
	
	//Text_DrawStringAtXY(10, 10, "This is Infestation!", COLOR_BRIGHT_BLUE, COLOR_BRIGHT_WHITE);
}



/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/




// show game over screen
void Screen_ShowGameOver(void)
{
	Text_ClearScreen(APP_FOREGROUND_COLOR, APP_BACKGROUND_COLOR);
	
	Text_DrawStringAtXY(15, 2, "GAME OVER!", COLOR_BRIGHT_WHITE, COLOR_BRIGHT_WHITE);
	Text_DrawStringAtXY(1, 4, "The Humans have colonized your planet", COLOR_BRIGHT_WHITE, COLOR_BRIGHT_WHITE);
	Text_DrawStringAtXY(1, 5, "Everything that is not poo is now the", COLOR_BRIGHT_WHITE, COLOR_BRIGHT_WHITE);
	Text_DrawStringAtXY(1, 6, "property of the M.U.S.K. corporation,", COLOR_BRIGHT_WHITE, COLOR_BRIGHT_WHITE);
	Text_DrawStringAtXY(1, 7, "including all robots and robot parts.", COLOR_BRIGHT_WHITE, COLOR_BRIGHT_WHITE);
	Text_DrawStringAtXY(1, 9, "Your fellow robots are disappointed in", COLOR_BRIGHT_WHITE, COLOR_BRIGHT_WHITE);
	Text_DrawStringAtXY(1, 10, "your lack of productivity. You are", COLOR_BRIGHT_WHITE, COLOR_BRIGHT_WHITE);
	Text_DrawStringAtXY(1, 11, "being reassigned to street sweeper.", COLOR_BRIGHT_WHITE, COLOR_BRIGHT_WHITE);
	Text_DrawStringAtXY(5, 14, "= Press any key to continue =", COLOR_BRIGHT_WHITE, COLOR_BRIGHT_WHITE);
	
	Keyboard_GetChar();

	Text_ClearScreen(APP_FOREGROUND_COLOR, APP_BACKGROUND_COLOR);
}


// set up screen variables and draw screen for first time
void Screen_Render(void)
{
	Screen_DrawUI();
}


// display information about game
void Screen_ShowAppAboutInfo(void)
{
	// show app name, version, and credit
	Text_DrawStringAtXY(0, 59, "This game made during April 5-7, 2024 F256 Game Jam", COLOR_BRIGHT_BLUE, COLOR_BRIGHT_WHITE);

}
