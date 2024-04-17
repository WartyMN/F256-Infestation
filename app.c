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
#include "comm_buffer.h"
#include "general.h"
#include "keyboard.h"
#include "level.h"
#include "memory.h"
//#include "overlay_em.h"
#include "overlay_startup.h"
#include "player.h"
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

static uint8_t	joy2playerdir[11] = {0xff, 0, 4, 0xff, 6, 7, 5, 0xff, 2, 1, 3};	// takes a joystick input (minus any buttons), and returns the player direction associated with it.

static bool					game_is_over = false;

/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

char*						global_string_buff1 = (char*)STORAGE_STRING_BUFFER_1;
// global_string_buff2char*					global_string_buff2 = (char*)STORAGE_STRING_BUFFER_2;

extern uint8_t				zp_joy;	// tracks what's going on with j0/j1 (for this game, both do same thing)

extern uint8_t				zp_bank_num;
extern uint16_t				zp_px;
extern uint16_t				zp_py;
extern uint8_t				zp_player_dir;
extern uint8_t				zp_player_dir_prev;
extern uint16_t				zp_ticktock;
extern int8_t				zp_hp;
extern int8_t				zp_lives;

#pragma zpsym ("zp_bank_num");
#pragma zpsym ("zp_px");
#pragma zpsym ("zp_py");
#pragma zpsym ("zp_joy");
#pragma zpsym ("zp_player_dir");
#pragma zpsym ("zp_player_dir_prev");
#pragma zpsym ("zp_ticktock");
#pragma zpsym ("zp_hp");
#pragma zpsym ("zp_lives");


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

// initialize various objects - once
void App_InitializeApp(void);

// initialize game - once per game
void App_InitializeGame(void);

// handles user input
uint8_t App_MainMenuLoop(void);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/


// initialize various objects - once
void App_InitializeApp(void)
{
	kernel_init();

	App_LoadOverlay(OVERLAY_STARTUP);
	
	if (Sys_InitSystem() == false)
	{
		App_Exit(0);
	}
	
	Sys_SetBorderSize(0, 0); // want all 80 cols and 60 rows!
	
	Startup_SetUpSprites();
	
	// initialize the random number generator embedded in the Vicky
	Startup_InitializeRandomNumGen();
	
	// initialize the ptps for the comms buff
	Startup_InitializeCommsBuffer();
	
	// copy LUT into VICKY memory
	
	// teach VICKY where the sprites are and configure each one
	Startup_InitializeSprites();
	
	// set up tilemap
	Startup_SetUpTileMap();
	
	// Do first draw of UI
	App_LoadOverlay(OVERLAY_SCREEN);
	Screen_Render();
	Screen_ShowAppAboutInfo();
}


// initialize various objects - once per game
void App_InitializeGame(void)
{
	App_LoadOverlay(OVERLAY_SCREEN);
	Screen_ShowAppAboutInfo();
	
	// set up player
	App_LoadOverlay(OVERLAY_STARTUP);
	Startup_InitializePlayer();
	
	// initialize level (draw tiles, place humans, etc.) - can move to startup if run short of memory
	Level_Initialize();

// 	// tell VICKY where the sprite it, what size sprite it is, what color to use, and to enabled it
// 	Sys_SwapIOPage(VICKY_IO_PAGE_REGISTERS);
// 	R16(SPRITE0_X_LO) = zp_px;		
// 	R16(SPRITE0_Y_LO) = zp_py;		
// 	R8(SPRITE0_CTRL) = 0x41; //Size=16x16, Layer=0, LUT=0, Enabled	
// 	//R8(SPRITE0_CTRL) = 0x01; //Size=32x32, Layer=0, LUT=0, Enabled	
// 
// 	// tell VICKY about the first human sprite and turn it on
// 	R16(SPRITE0_X_LO + SPRITE_REG_LEN) = zp_px+40;		
// 	R16(SPRITE0_Y_LO + SPRITE_REG_LEN) = zp_py+40;		
// 	R8(SPRITE0_CTRL + SPRITE_REG_LEN) = 0x41; //Size=16x16, Layer=0, LUT=0, Enabled	
// 
// 	Sys_DisableIOBank();
// 
// 	
// 	//DEBUG_OUT(("%s %d: zp_px=%x + %x", __func__, __LINE__, zp_px & 0xff, zp_px >> 8));

	
	// do initial stats draw
	Buffer_RefreshStatDisplay(true);
	
	// initial message
	Buffer_NewMessage("Infestation detected! Stop the humans!");
	
	// reset the game over flag
	game_is_over = false;
	
	// reset joystick condition map so it doesn't hang on from last game (or from junk on startup)
	*(uint8_t*)ZP_JOY = 0;
}


// handles user input
uint8_t App_MainMenuLoop(void)
{
	uint8_t				user_input;
	uint8_t				joy_minus_buttons;
	bool				player_wants_to_fire;
	uint16_t			tank_sprite_loc;
	uint16_t			base_tank_sprite_loc;

	
	tank_sprite_loc = base_tank_sprite_loc = SPRITE_ROBOT_16F_LOMED_ADDR;	// starting med/lo addr of tank sprite
	zp_player_dir_prev = zp_player_dir;
	
	// main loop
	while (! game_is_over)
	{
		// turn off cursor - seems to turn itself off when kernal detects cursor position has changed. 
		//Sys_EnableTextModeCursor(false);
		
		//DEBUG: track software stack pointer
		//sprintf(global_string_buffer, "sp: %x%x", *(char*)0x52, *(char*)0x51);
		//Buffer_NewMessage(global_string_buffer);
		
		// increment the frame counter we use to check when sprite animations should change from cell0 to cell1 to cell0, etc. 
		++zp_ticktock;
		player_wants_to_fire = false;
		
						
		// ask Screen to establish which menu items should be available (this just keeps this code out of MAIN to maximize heap space)
		//App_LoadOverlay(OVERLAY_SCREEN);
		
		// ask Screen to get user input and vet it against the menu items that are currently enabled
		// only inputs for active menu items will cause an input to be returned here
		user_input = Keyboard_GetKeyIfPressed();
		// Get user input and vet it against the menu items that are currently enabled
		// returns ACTION_INVALID_INPUT if the key pressed was for a disabled menu item
		// returns the key pressed if it matched an enabled menu item, or if wasn't a known (to Screen) input. This lets App still allow for cursor keys, etc, which aren't represented by menu items

		//DEBUG_OUT(("%s %d: user_input=%u", __func__ , __LINE__, user_input));
		
		// first switch: normalize any alt keyboard input so we can merge with Joy input afterwards
		switch (user_input)
		{
			case MOVE_UP:
				zp_py -= 2;
				zp_player_dir = PLAYER_DIR_NORTH;
				user_input = ACTION_INVALID_INPUT;
				break;
				
			case MOVE_RIGHT:
				zp_px += 2;
				zp_player_dir = PLAYER_DIR_EAST;
				user_input = ACTION_INVALID_INPUT;
				break;
				
			case MOVE_DOWN:
				zp_py += 2;
				zp_player_dir = PLAYER_DIR_SOUTH;
				user_input = ACTION_INVALID_INPUT;
				break;
				
			case MOVE_LEFT:
				zp_px -= 2;
				zp_player_dir = PLAYER_DIR_WEST;
				user_input = ACTION_INVALID_INPUT;
				break;
				
			case MOVE_UP_RIGHT:
				zp_px += 2;
				zp_py -= 2;
				zp_player_dir = PLAYER_DIR_NORTHEAST;
				user_input = ACTION_INVALID_INPUT;
				break;
				
			case MOVE_DOWN_RIGHT:
				zp_px += 2;
				zp_py += 2;
				zp_player_dir = PLAYER_DIR_SOUTHEAST;
				user_input = ACTION_INVALID_INPUT;
				break;
				
			case MOVE_DOWN_LEFT:
				zp_px -= 2;
				zp_py += 2;
				zp_player_dir = PLAYER_DIR_SOUTHWEST;
				user_input = ACTION_INVALID_INPUT;
				break;
				
			case MOVE_UP_LEFT:
				zp_px -= 2;
				zp_py -= 2;
				zp_player_dir = PLAYER_DIR_NORTHWEST;
				user_input = ACTION_INVALID_INPUT;
				break;
				
			case ACTION_FIRE:
				player_wants_to_fire = true;
				break;
				
			case ACTION_WARP:
				user_input = ACTION_INVALID_INPUT;
				break;
				
			case ACTION_BOMB:
				user_input = ACTION_INVALID_INPUT;
				break;
				
			case ACTION_CYCLE_WEAPON:
				Player_SetNextWeapon();
				break;
								
			case 0:
				user_input = ACTION_INVALID_INPUT;
				break;
				
			default:
				//sprintf(global_string_buff1, "didn't know key %u", user_input);
				//Buffer_NewMessage(global_string_buff1);
				//DEBUG_OUT(("%s %d: didn't know key %u", __func__, __LINE__, user_input));
				//fatal(user_input);
				//sprintf(global_string_buff1, "%d %x '%c'", user_input, user_input, user_input);
				//Text_DrawStringAtXY(0, 0, global_string_buff1, COLOR_BRIGHT_WHITE, COLOR_BLUE);
				//Text_SetCharAtXY(++zp_px, zp_py, user_input);
				user_input = ACTION_INVALID_INPUT;
				break;
		}
		
		// handle joystick actions if no keyboard input was received
		if (user_input == ACTION_INVALID_INPUT)
		{
			if (*(uint8_t*)ZP_JOY & JOY_UP_BIT)
			{
				zp_py -= 2;
			}

			if (*(uint8_t*)ZP_JOY & JOY_DOWN_BIT)
			{
				zp_py += 2;
				zp_player_dir += 2;
			}

			if (*(uint8_t*)ZP_JOY & JOY_LEFT_BIT)
			{
				zp_px -= 2;
			}

			if (*(uint8_t*)ZP_JOY & JOY_RIGHT_BIT)
			{
				zp_px += 2;
				zp_player_dir += 1;
			}

			if (*(uint8_t*)ZP_JOY & JOY_FIRE1_BIT)
			{
				player_wants_to_fire = true;
			}

			if (*(uint8_t*)ZP_JOY & JOY_FIRE2_BIT)
			{
				Player_SetNextWeapon();
			}

			// got joy buttons, clear them and set current player dir based on joy directions pushed
			zp_joy &= 0b00001111;
			
			if (zp_joy)	// any joy not fire button
			{
				// dir changed
				zp_player_dir = joy2playerdir[*(uint8_t*)ZP_JOY];
			}
		}
				
		Player_ValidateLocation();

		// point to the appropriate tank shape
		// LOGIC:
		//   each sprite shape is 16x16=256 bytes. there are 16 total shapes, so exactly 4k of data.
		//   sprites are arranged in clockwise order, from 12:00. each position has 2 shapes, frame1/frame2, for per-frame anim.
		//   to get the right shape:
		//     1. add 0 or FF for the frame offset
		//     2. multiply zp_player_dir by 512 (PLAYER_DIR_NORTH = 0... PLAYER_DIR_NORTHWEST=7)
		//        to make things fast, we have a preset offset calculation table.


		if (zp_player_dir != zp_player_dir_prev)
		{
			tank_sprite_loc = SPRITE_ROBOT_16F_LOMED_ADDR;
			tank_sprite_loc += (uint16_t)zp_player_dir * (uint16_t)512;
			base_tank_sprite_loc = tank_sprite_loc;
			zp_player_dir_prev = zp_player_dir;

			// animate by swapping to the other frame of sprite
			//tank_sprite_loc -= 0xff * (frame_odd_even % 2);
// 				if ( (frame_odd_even % 2) == 0)
// 				{
// 					tank_sprite_loc -= 0xff;
// 				}
		}
		else
		{
			// animate by swapping to the other frame of sprite

			//tank_sprite_loc += 0xff * (frame_odd_even % 2);
			if ( (zp_ticktock % PLAYER_SPRITE_TICKTOCK_DIVISOR) == 0)
			{
				tank_sprite_loc += (uint16_t)0x0100;
			}
			else
			{
				tank_sprite_loc = base_tank_sprite_loc;
			}
		}

		//General_DelayTicks(800);
		
		Sys_SwapIOPage(VICKY_IO_PAGE_REGISTERS);
		R8(SPRITE0_ADDR_LO) = tank_sprite_loc & 0xFF;
		R8(SPRITE0_ADDR_MED) = (tank_sprite_loc) >> 8;

		//R8(SPRITE0_ADDR_HI) = 0x03;	// we are placing robot sprites starting at 03 6000 in EM
		Sys_DisableIOBank();
		
		// update sprite pos
		Sys_SwapIOPage(VICKY_IO_PAGE_REGISTERS);
		R16(SPRITE0_X_LO) = zp_px;		
		R16(SPRITE0_Y_LO) = zp_py;		
		Sys_DisableIOBank();
		
		if (player_wants_to_fire == true)
		{
			Level_PlayerAttemptShoot();
		}
		
		// update and move humans around, check for deaths, etc. 
		Level_UpdateSprites();
		Level_RenderSprites();

		// check if player died, etc. 
		if (zp_hp < 1)
		{
			Player_LoseLife();
		}
		
		Buffer_RefreshStatDisplay(true);
		
		//DEBUG_OUT(("%s %d: X/Y=%u,%u; player_wants_to_fire=%u", __func__, __LINE__, zp_px, zp_py, player_wants_to_fire));

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


// handle game over scenario: say you are dead, stop game, show hi scores, etc, etc.
void App_GameOver(void)
{
	//Buffer_NewMessage("Your last chassis has been destroyed. You being re-assigned to: street sweeper");
	Buffer_NewMessage("You blew it. Hoomans get this planet.");
				
	// set the game over flag so that main menu knows to stop doing what it's doing
	game_is_over = true;
	
	App_LoadOverlay(OVERLAY_SCREEN);
	Screen_ShowGameOver();
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


// get random number between 1 and the_range + 1
// must have seeded the number generator first with call to _randomize() --> cc65 function
// if passed 0, returns 0.
uint16_t App_GetRandom(uint16_t the_range)
{
	uint16_t the_num = 0;
	uint16_t the_random;

	// reason we have this check is that we'll get unexpected results if you do %0
	if (the_range == 0)
	{
		return 0;
	}

	// need to have vicky registers available
	Sys_SwapIOPage(VICKY_IO_PAGE_REGISTERS);

	the_random = R16(RANDOM_NUM_GEN_LOW);
	the_num = the_random % the_range + 1;

	Sys_RestoreIOPage();

	return the_num;
}


int main(void)
{
	App_InitializeApp();
	
	while (1)
	{
		App_InitializeGame();
		
		App_MainMenuLoop();
	}
	
	// restore screen, etc.
	App_Exit(ERROR_NO_ERROR);
	
	return 0;
}