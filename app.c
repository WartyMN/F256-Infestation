/*
 * app.c
 *
 *  Created on: April 5, 2024
 *      Author: micahbly
 *
 *  A top-down shoot'em up
 *
 */
 


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/


// project includes
#include "app.h"
#include "general.h"
#include "keyboard.h"
#include "memory.h"
//#include "overlay_em.h"
#include "overlay_startup.h"
#include "text.h"
#include "screen.h"
#include "sys.h"

// C includes
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// cc65 includes
#include <device.h>
//#include <unistd.h>
#include <cc65.h>
#include "f256.h"
#include "api.h"
#include "kernel.h"



/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/

#define CH_PROGRESS_BAR_FULL	CH_CHECKERBOARD


/*****************************************************************************/
/*                          File-scoped Variables                            */
/*****************************************************************************/


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

char*					global_string_buff1 = (char*)STORAGE_STRING_BUFFER_1;
char*					global_string_buff2 = (char*)STORAGE_STRING_BUFFER_2;

extern uint8_t			global_joy_state;	// tracks what's going on with j0/j1 (for this game, both do same thing)

extern uint8_t			zp_bank_num;
extern uint8_t			zp_px;
extern uint8_t			zp_py;
#pragma zpsym ("zp_bank_num");
#pragma zpsym ("zp_px");
#pragma zpsym ("zp_py");


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

// initialize various objects - once per run
void App_Initialize(void);

// handles user input
uint8_t App_MainMenuLoop(void);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/


// initialize various objects - once per run
void App_Initialize(void)
{
	// show info about the host F256 and environment, as well as copyright, version of f/manager
	App_LoadOverlay(OVERLAY_SCREEN);
	Screen_ShowAppAboutInfo();
}


// handles user input
uint8_t App_MainMenuLoop(void)
{
	uint8_t				user_input;
	bool				exit_main_loop = false;
	bool				success;
	
	// main loop
	while (! exit_main_loop)
	{
		// turn off cursor - seems to turn itself off when kernal detects cursor position has changed. 
		Sys_EnableTextModeCursor(false);
		
		//DEBUG: track software stack pointer
		//sprintf(global_string_buffer, "sp: %x%x", *(char*)0x52, *(char*)0x51);
		//Buffer_NewMessage(global_string_buffer);

		do
		{
						
			// ask Screen to establish which menu items should be available (this just keeps this code out of MAIN to maximize heap space)
			App_LoadOverlay(OVERLAY_SCREEN);
			
			// ask Screen to get user input and vet it against the menu items that are currently enabled
			// only inputs for active menu items will cause an input to be returned here
			user_input = Keyboard_GetKeyIfPressed();
			// Get user input and vet it against the menu items that are currently enabled
			// returns ACTION_INVALID_INPUT if the key pressed was for a disabled menu item
			// returns the key pressed if it matched an enabled menu item, or if wasn't a known (to Screen) input. This lets App still allow for cursor keys, etc, which aren't represented by menu items

			//DEBUG_OUT(("%s %d: user_input=%u", __func__ , __LINE__, user_input));
			
			// first switch: for file menu only, and skip if file menu is inactive
			//   slightly inefficient in that it has to go through them all twice, but this is not a performance bottleneck
			//   note: we also put the sort commands here because it doesn't make sense to sort if no files
			switch (user_input)
			{
				case MOVE_LEFT:
				case MOVE_LEFT_ALT:
					Text_DrawStringAtXY(0, 0, "left        ", COLOR_BRIGHT_WHITE, COLOR_GREEN);
					--zp_px;
					break;

				case MOVE_RIGHT:
				case MOVE_RIGHT_ALT:
					Text_DrawStringAtXY(0, 0, "right       ", COLOR_BRIGHT_WHITE, COLOR_GREEN);
					++zp_px;
					break;

				case MOVE_UP:
				case MOVE_UP_ALT:
					Text_DrawStringAtXY(0, 0, "up          ", COLOR_BRIGHT_WHITE, COLOR_GREEN);
					--zp_px;
					break;

				case MOVE_DOWN:
				case MOVE_DOWN_ALT:
					Text_DrawStringAtXY(0, 0, "down        ", COLOR_BRIGHT_WHITE, COLOR_GREEN);
					++zp_px;
					break;

				case ACTION_FIRE:
					//
					Text_DrawStringAtXY(0, 0, "fire        ", COLOR_BRIGHT_WHITE, COLOR_GREEN);
					break;
					
				case ACTION_WARP:
					//
					Text_DrawStringAtXY(0, 0, "warp        ", COLOR_BRIGHT_WHITE, COLOR_GREEN);
					break;
					
				case ACTION_BOMB:
					//
					Text_DrawStringAtXY(0, 0, "grenade     ", COLOR_BRIGHT_WHITE, COLOR_GREEN);
					break;
				
				case JOYSTICK_EVENT_OCCURRED:
					// some change happened with one of the joysticks. 
					// can check global_joy_state to see what's going on
					sprintf(global_string_buff1, "%d %x '%c'", global_joy_state, global_joy_state, global_joy_state);
					Text_DrawStringAtXY(0, 1, global_string_buff1, COLOR_BRIGHT_WHITE, COLOR_RED);
					Text_DrawStringAtXY(0, 2, "                                       ", COLOR_BRIGHT_WHITE, COLOR_RED);
					
					if (global_joy_state & JOY_UP_BIT)
					{
						Text_DrawStringAtXY(0, 2, "up", COLOR_BRIGHT_WHITE, COLOR_RED);
					}

					if (global_joy_state & JOY_DOWN_BIT)
					{
						Text_DrawStringAtXY(4, 2, "down", COLOR_BRIGHT_WHITE, COLOR_RED);
					}

					if (global_joy_state & JOY_LEFT_BIT)
					{
						Text_DrawStringAtXY(10, 2, "left", COLOR_BRIGHT_WHITE, COLOR_RED);
					}

					if (global_joy_state & JOY_RIGHT_BIT)
					{
						Text_DrawStringAtXY(16, 2, "right", COLOR_BRIGHT_WHITE, COLOR_RED);
					}

					if (global_joy_state & JOY_FIRE1_BIT)
					{
						Text_DrawStringAtXY(23, 2, "fire1", COLOR_BRIGHT_WHITE, COLOR_RED);
					}

					if (global_joy_state & JOY_FIRE2_BIT)
					{
						Text_DrawStringAtXY(30, 2, "fire2", COLOR_BRIGHT_WHITE, COLOR_RED);
					}
				
					break;
				
				case 0:
					break;
					
				default:
					//sprintf(global_string_buff1, "didn't know key %u", user_input);
					//Buffer_NewMessage(global_string_buff1);
					//DEBUG_OUT(("%s %d: didn't know key %u", __func__, __LINE__, user_input));
					//fatal(user_input);
					sprintf(global_string_buff1, "%d %x '%c'", user_input, user_input, user_input);
					Text_DrawStringAtXY(0, 0, global_string_buff1, COLOR_BRIGHT_WHITE, COLOR_BLUE);
					//Text_SetCharAtXY(++zp_px, zp_py, user_input);
					user_input = ACTION_INVALID_INPUT;
					break;
			}
		} while (user_input == ACTION_INVALID_INPUT);				
	} // while for exit loop
	
	// normal returns all handled above. this is a catch-all
	return 0;
}



/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/







// Brings the requested overlay into memory
void App_LoadOverlay(uint8_t the_overlay_em_bank_number)
{
	zp_bank_num = the_overlay_em_bank_number;
	Memory_SwapInNewBank(OVERLAY_CPU_BANK);
}


// if ending on error: display error message, wait for user to confirm, and exit
// if no error, just exit
void App_Exit(uint8_t the_error_number)
{	
	R8(0xD6A2) = 0xDE;
	R8(0xD6A3) = 0xAD;
	R8(0xD6A0) = 0xF0;
	R8(0xD6A0) = 0x00;
	asm("JMP ($FFFC)");
}




int main(void)
{
	kernel_init();

	App_LoadOverlay(OVERLAY_STARTUP);
	
	if (Sys_InitSystem() == false)
	{
		App_Exit(0);
	}
	
	Sys_SetBorderSize(0, 0); // want all 80 cols and 60 rows!
	
	// initialize the random number generator embedded in the Vicky
	Startup_InitializeRandomNumGen();
	
	// clear screen and draw logo
	//Startup_ShowLogo();
	
	App_LoadOverlay(OVERLAY_SCREEN);
	
	// Do first draw of UI
	Screen_Render();

	App_Initialize();
	
	App_MainMenuLoop();
	
	// restore screen, etc.
	App_Exit(ERROR_NO_ERROR);
	
	return 0;
}