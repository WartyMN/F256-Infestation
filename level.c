/*
 * player.c
 *
 *  Created on: Apr 6, 2024
 *      Author: micahbly
 *
 *  Routines for managing the player
 *
 */



/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "level.h"
#include "app.h"
#include "comm_buffer.h"
#include "general.h"
#include "kernel.h"
#include "keyboard.h"
#include "memory.h"
#include "object.h"
#include "player.h"
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



/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

Sprite						global_missiles[LEVEL_MAX_MISSILES];
Sprite						global_humans[LEVEL_MAX_HUMANS];
Sprite						global_chips[LEVEL_MAX_CHIPS];
Sprite						global_clips[LEVEL_MAX_CLIPS];
Sprite						global_poo[LEVEL_MAX_POO];

extern Player*				global_player;

extern char*				global_string[NUM_STRINGS];
extern char*				global_string_buff1;
// extern char*				global_string_buff2;

extern uint8_t				zp_bank_num;
#pragma zpsym ("zp_bank_num");
extern uint8_t				io_bank_value_kernel;	// stores value for the physical bank pointing to C000-DFFF whenever we change it, so we can restore it.

extern uint16_t				zp_px;
extern uint16_t				zp_py;
extern uint8_t				zp_num_bullets;
extern uint8_t				zp_num_clips;
extern uint8_t				zp_num_warps;
extern uint8_t				zp_speed;
extern uint16_t				zp_points;
extern int8_t				zp_hp;
extern uint8_t				zp_bullet_dmg;
extern uint8_t				zp_player_dir;
extern int8_t				zp_lives;
extern uint16_t				zp_ticktock;

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
#pragma zpsym ("zp_ticktock");

//#pragma zpsym ("global_player");


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

// sets starting location of humans randomly
void Level_PlaceHumans(void);

// activates all the humans
void Level_ActivateAllHumans(void);

// deactivates all missiles so they are ready for use by player
void Level_DeactivateAllMissiles(void);

// add the player to the level at a random spot
void Level_PlacePlayer(void);

// update the tile grid that corresponds to the passed location to the "bloody" version of the tile
// needs to be in MAIN because it temporarily maps EM data into the overlay slot
void Level_MakeTileBloody(uint16_t x, uint16_t y);

// Reset the tilemap to initial conditions: the no-gore tiles
void Level_ResetTileMap(void);

/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/


// sets starting location of humans randomly
void Level_PlaceHumans(void)
{
	uint8_t		i;
	uint16_t	new_pixel;
	
	// TODO: check not being placed on top of another human, or on top of obstacle, chip, etc.
	
	for (i=0; i < LEVEL_MAX_HUMANS; i++)
	{
		// pick a legal place within the playfield, which has 32 pix boundary on all sides for sprites.
		// make sure that the picked place is an even pixel number, not 1,3, 13 etc., because we are using even numbering for ticktock animations
		
		new_pixel = App_GetRandom(LEVEL_MAX_X - LEVEL_MIN_X) + LEVEL_MIN_X;
		
		if (new_pixel % 2 != 0)
		{
			--new_pixel;
		}
		
		global_humans[i].x1_ = new_pixel;

		new_pixel = App_GetRandom(LEVEL_MAX_Y - LEVEL_MIN_Y) + LEVEL_MIN_Y;
		
		if (new_pixel % 2 != 0)
		{
			--new_pixel;
		}
		
		global_humans[i].y1_ = new_pixel;
		
		// set a random starting direction
		Object_SetDirection(&global_humans[i], App_GetRandom(8) - 1, HUMAN_SPEED, HUMAN_L_SHIFT_PER_SHAPE);
	}

	return;
}


// activates all the humans
void Level_ActivateAllHumans(void)
{
	uint8_t		i;
	
	for (i=0; i < LEVEL_MAX_HUMANS; i++)
	{
		//DEBUG_OUT(("%s %d: marking human sprite %u as active; reg=%p", __func__, __LINE__, i, global_humans[i].sprite_reg_addr_));
		
		global_humans[i].is_active_ = 1;
		global_humans[i].render_needed_ = 1;
	}

	return;
}


// deactivates all missiles so they are ready for use by player
void Level_DeactivateAllMissiles(void)
{
	uint8_t		i;
	
	for (i=0; i < LEVEL_MAX_MISSILES; i++)
	{
		//DEBUG_OUT(("%s %d: marking missile sprite %u as inactive; reg=%p", __func__, __LINE__, i, global_missiles[i].sprite_reg_addr_));
		
		global_missiles[i].is_active_ = 0;
		global_missiles[i].render_needed_ = 0;

		global_missiles[i].state_ = 0x60;	// $60=8x8 sprite; 1 = on

		//DEBUG_OUT(("%s %d: about to copy missile %u data to vicky: reg=%p, to copy=%p", __func__, __LINE__, i, global_missiles[i].sprite_reg_addr_, &global_missiles[i]));
		
		Sys_SwapIOPage(VICKY_IO_PAGE_REGISTERS);
		memcpy(global_missiles[i].sprite_reg_addr_, &global_missiles[i], SPRITE_REG_LEN);
		Sys_DisableIOBank();

		//DEBUG_OUT(("%s %d: copied missile %u data to vicky: reg=%p, to copy=%p", __func__, __LINE__, i, global_missiles[i].sprite_reg_addr_, &global_missiles[i]));
	}

	return;
}


// add the player to the level at a random spot
void Level_PlacePlayer(void)
{
	Coordinate 	new_location;
	
	// TODO: check not being placed on top of human, or on top of obstacle, chip, etc.
	
	// pick a legal place within the playfield, which has 32 pix boundary on all sides for sprites.
	new_location.x = App_GetRandom(LEVEL_MAX_X - LEVEL_MIN_X) + LEVEL_MIN_X;
	new_location.y = App_GetRandom(LEVEL_MAX_Y - LEVEL_MIN_Y) + LEVEL_MIN_Y;
	
	// set object's location
	Player_MoveToLocation(&new_location);
	
	return;
}


// update the tile grid that corresponds to the passed location to the "bloody" version of the tile
// needs to be in MAIN because it temporarily maps EM data into the overlay slot
void Level_MakeTileBloody(uint16_t x, uint16_t y)
{
	uint16_t	tile_map_grid_addr;
	uint8_t		prev_value;
	
	// LOGIC:
	//   the tile map is at TILEMAP_PHYS_ADDR
	//   the tile map is 20x15, with each grid being 16x16 px
	//   x and y are 0-319 and 0-239 (except they are +32,+32 because of sprite offset)
	//   x/16 and y/16 should get us to the right area
	//   x/16 + y/16*20 = the tile EXCEPT that tilemap uses 2 bytes for each, so need to double that. 
	//   to make a grid bloody, we just add 1 to it, as tiles are arranged clean-dirty-clean-dirty-etc.
	//   if tile already has a non-even number in it, don't change it. 
	
	// account for 32 px offset sprites use
	x -= 32;
	y -= 32;
	
	// reduce to linear offset, multiplying by 2 to account for tilemap values being 2-byte
	tile_map_grid_addr = (x/16 + (y/16 * 20)) * 2;
	
	//DEBUG_OUT(("%s %d: tile_map_grid_addr=%x, x=%u, y=%u, col=%u, row=%u", __func__, __LINE__, tile_map_grid_addr, x, y,  x/16, y/16));
	
	tile_map_grid_addr += TILEMAP_ADDR_IN_CPU_SPACE;
	
	//DEBUG_OUT(("%s %d: tile_map_grid_addr=%x after adding TILEMAP_ADDR_IN_CPU_SPACE", __func__, __LINE__, tile_map_grid_addr));
	
	// map in the EM bank holding the tilemap
	App_LoadOverlay(TILEMAP_VALUE);
	
	prev_value = R8(tile_map_grid_addr);

	//DEBUG_OUT(("%s %d: prev-value=%u", __func__, __LINE__, prev_value));

	if (prev_value % 2 == 0)
	{
		++prev_value;
		R8(tile_map_grid_addr) = prev_value;
	}	
}


// Reset the tilemap to initial conditions: the no-gore tiles
void Level_ResetTileMap(void)
{
	uint16_t	tile_map_grid_addr;
	uint16_t	i;
	uint8_t		prev_value;
	
	// LOGIC:
	//   the tile map is at TILEMAP_PHYS_ADDR
	//   the tile map is 20x15, with each grid being 16x16 px
	//   to make a grid bloody, we added 1 to it, as tiles are arranged clean-dirty-clean-dirty-etc.
	//   if tile already doesn't have a non-even number in it, don't change it. 
	
	tile_map_grid_addr = TILEMAP_ADDR_IN_CPU_SPACE;
	
	//DEBUG_OUT(("%s %d: tile_map_grid_addr=%x after adding TILEMAP_ADDR_IN_CPU_SPACE", __func__, __LINE__, tile_map_grid_addr));
	
	// map in the EM bank holding the tilemap
	App_LoadOverlay(TILEMAP_VALUE);
	
	for (i = 0; i < TILEMAP_LEN; i++)
	{
		prev_value = R8(tile_map_grid_addr);
	
		//DEBUG_OUT(("%s %d: prev-value=%u", __func__, __LINE__, prev_value));
	
		if (prev_value % 2 != 0)
		{
			--prev_value;
			R8(tile_map_grid_addr) = prev_value;
		}
		
		++tile_map_grid_addr;
	}
	
}





/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/



// **** CONSTRUCTOR AND DESTRUCTOR *****

// configure level for a new game - place humans, set up tiles, re-initialize sprites, etc. 
void Level_Initialize(void)
{
	// turn all sprites off in case they had been on
	
	// place random obstacles?
	
	// reset tilemap in case it got bloodied by previous game
	Level_ResetTileMap();
	
	// place player
	Level_PlacePlayer();
	
	// place chips, clips, and poo
	
	// place the humans around the board
	Level_PlaceHumans();
	
	// set random starting directions/vectors for humans
	
	// turn all humans on
	Level_ActivateAllHumans();
	
	// turn all the missiles off so they can be used again
	Level_DeactivateAllMissiles();
}


// **** SETTERS *****









// **** GETTERS *****







// ***** SCENE MANAGEMENT FUNCTIONS ****


// update graphic state of all non-player sprites
// changes to west facing if going left, right leg if it was left leg, etc.
// checks current velocity and resets if human has hit edge of screen
void Level_UpdateSprites(void)
{
	uint8_t			i;
	uint8_t			j;
	uint8_t			temp_frame_saver;
	Rectangle		player_box;
	
	player_box.x1 = zp_px;
	player_box.y1 = zp_py;
	player_box.x2 = zp_px + PLAYER_SPRITE_WIDTH;
	player_box.y2 = zp_py + PLAYER_SPRITE_HEIGHT;
	
	for (i=0; i < LEVEL_MAX_HUMANS; i++)
	{
		if (global_humans[i].is_active_ == 1)
		{

			//DEBUG_OUT(("%s %d: human %u is active", __func__, __LINE__, i));

			// check if this human got run over by player
			if (Object_CollisionCheck(&global_humans[i], &player_box) == true)
			{
				// splat!
				Player_TakeDamage(PLAYER_DAMAGE_FROM_SLIMING);
				
				//Buffer_NewMessage("I got slimed!");
				
				zp_points += POINTS_PER_HUMAN;
				
				// mark object as dead
				global_humans[i].is_active_ = 0;
				global_humans[i].x_speed_ = 0;
				global_humans[i].y_speed_ = 0;
				global_humans[j].render_needed_ = 1;
				
				// hide the sprite, and mark the tile it was on as bloody
				Level_MakeTileBloody(global_humans[i].x1_, global_humans[i].y1_);
			}
			else
			{
				// this human is still alive.
				
				// attempt to move human in direction it was going. 
				Object_Move(&global_humans[i]);
				
				// check if move above caused human to be blocked by screen bounds, objects, etc.
				if (Object_MoveIsValid(&global_humans[i]) == false)
				{
					// blocked for this turn. pick a random new direction and get sprite speed and graphics updated
					//global_humans[i].x_speed_ = 4 - (App_GetRandom(3) * 2);		// end up with -2, 0, or +2 for this vector
					//global_humans[i].y_speed_ = 4 - (App_GetRandom(3) * 2);		// end up with -2, 0, or +2 for this vector
					Object_SetDirection(&global_humans[i], App_GetRandom(8) - 1, HUMAN_SPEED, HUMAN_L_SHIFT_PER_SHAPE);					
				}
				
				// check if it needs an anim change.
				//if ( (zp_ticktock % HUMAN_SPRITE_TICKTOCK_DIVISOR) == 0)
				if ( ((global_humans[i].x1_ + global_humans[i].y1_) % 8) == 0)
				{
					// toggle anim cell to the other
					temp_frame_saver = global_humans[i].addr_med_;
					global_humans[i].addr_med_ = global_humans[i].addr_med_alt_;
					global_humans[i].addr_med_alt_ = temp_frame_saver;
				}
			}

			global_humans[i].render_needed_ = 1;
		}
	}

	for (i=0; i < LEVEL_MAX_MISSILES; i++)
	{
		if (global_missiles[i].is_active_ == 1)
		{

			DEBUG_OUT(("%s %d: missile %u is active @ %u,%u", __func__, __LINE__, i, global_missiles[i].x1_, global_missiles[i].y1_));

			// check if this missile hit any humans
			for (j=0; j < LEVEL_MAX_HUMANS; j++)
			{
				if (global_humans[j].is_active_ == true)
				{
					if (Object_CollisionCheck(&global_missiles[i], (Rectangle*)&global_humans[j].x1_) == true)
					{
						// player successfully shot a human
						zp_points += POINTS_PER_HUMAN;
						
						//Buffer_NewMessage("Bite my shiny ass, fleshbag!");
	
						// mark object as dead
						global_humans[j].is_active_ = 0;
						global_humans[j].x_speed_ = 0;
						global_humans[j].y_speed_ = 0;
						global_humans[j].render_needed_ = 1;
						// hide the sprite, and mark the tile it was on as bloody
						Level_MakeTileBloody(global_humans[j].x1_, global_humans[j].y1_);
						
						// inactivate this missile so it can be used again
						global_missiles[i].is_active_ = 0;
						global_missiles[i].x_speed_ = 0;
						global_missiles[i].y_speed_ = 0;
					}
				}
			}
			
			if (global_missiles[i].is_active_ == 1)
			{
				// this missile is still tracking
				
				// attempt to move missile in direction it was going. 
				Object_Move(&global_missiles[i]);
				
				// check if move above caused missile to be blocked by screen bounds, objects, etc.
				if (Object_MoveIsValid(&global_missiles[i]) == false)
				{
					// blocked. make inactive and remove from scene
					global_missiles[i].is_active_ = 0;
				}
			}

			global_missiles[i].render_needed_ = 1;
		}
	}
}

// render all active non-player sprites that need render update
// this is the only function that actually touches the sprite registers
void Level_RenderSprites(void)
{
	uint8_t		i;

	for (i=0; i < LEVEL_MAX_HUMANS; i++)
	{
		if (global_humans[i].render_needed_ == 1)
		{
			// do we need to turn it off on or?
			if (global_humans[i].is_active_ == 1)
			{
				global_humans[i].state_ = 0x41;	// $40=16x16 sprite; 1 = on
			}
			else
			{
				global_humans[i].state_ = 0x40;	// $40=16x16 sprite; 0 = off
			}

			//DEBUG_OUT(("%s %d: about to copy human %u data to vicky: reg=%p, to copy=%p", __func__, __LINE__, i, global_humans[i].sprite_reg_addr_, &global_humans[i]));
			
			Sys_SwapIOPage(VICKY_IO_PAGE_REGISTERS);
			memcpy(global_humans[i].sprite_reg_addr_, &global_humans[i], SPRITE_REG_LEN);
			Sys_DisableIOBank();

			//DEBUG_OUT(("%s %d: copied human %u data to vicky: reg=%p, to copy=%p", __func__, __LINE__, i, global_humans[i].sprite_reg_addr_, &global_humans[i]));
			
			global_humans[i].render_needed_ = 0;
		}		
	}

	for (i=0; i < LEVEL_MAX_MISSILES; i++)
	{
		if (global_missiles[i].render_needed_ == 1)
		{
			// do we need to turn it off on or?
			if (global_missiles[i].is_active_ == 1)
			{
				global_missiles[i].state_ = 0x61;	// $60=8x8 sprite; 1 = on
			}
			else
			{
				global_missiles[i].state_ = 0x60;	// $60=8x8 sprite; 0 = off
			}

			//DEBUG_OUT(("%s %d: about to copy missile %u data to vicky: reg=%p, to copy=%p", __func__, __LINE__, i, global_missiles[i].sprite_reg_addr_, &global_missiles[i]));
			
			Sys_SwapIOPage(VICKY_IO_PAGE_REGISTERS);
			memcpy(global_missiles[i].sprite_reg_addr_, &global_missiles[i], SPRITE_REG_LEN);
			Sys_DisableIOBank();

			//DEBUG_OUT(("%s %d: copied missile %u data to vicky: reg=%p, to copy=%p", __func__, __LINE__, i, global_missiles[i].sprite_reg_addr_, &global_missiles[i]));
			
			global_missiles[i].render_needed_ = 0;
		}		
	}
}



// **** COMBAT FUNCTIONS *****

// player wants to shoot: see if a missile is available, if player's gun is full, if refire time is up, etc. 
bool Level_PlayerAttemptShoot(void)
{
	uint8_t		i;

	//DEBUG_OUT(("%s %d: wants wants to shoot", __func__, __LINE__));
	
	// any ammo left in clip?
	if (zp_num_bullets > 0)
	{
		for (i=0; i < LEVEL_MAX_MISSILES; i++)
		{
			if (global_missiles[i].is_active_ != 1)
			{
				DEBUG_OUT(("%s %d: missile %u is available for use", __func__, __LINE__, i));
	
				// TODO: check clip status, reload time, refire time, etc. 
				
				// start missing with the player, use player dir and velocity to aim and fire it
				global_missiles[i].x1_ = zp_px;
				global_missiles[i].y1_ = zp_py;
				global_missiles[i].x2_ = zp_px + MISSILE_SPRITE_WIDTH;
				global_missiles[i].y2_ = zp_py + MISSILE_SPRITE_HEIGHT;
				
				Object_SetDirection(&global_missiles[i], zp_player_dir, MISSILE_SPEED, MISSILE_L_SHIFT_PER_SHAPE);					
	
				global_missiles[i].is_active_ = 1;
				global_missiles[i].render_needed_ = 1;
				
				--zp_num_bullets;
				
				// one fired is all we need
				return true;
			}
		}
	}
	else
	{
		// clip is empty. any other clips for this weapon?
		if (Player_Reload() == true)
		{
			Buffer_NewMessage("Changing clips");
		}
		else
		{
			Buffer_NewMessage("<click>");
		}
	}
	
	return false;
}


// **** OTHER FUNCTIONS *****


