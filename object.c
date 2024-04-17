/*
 * object.c
 *
 *  Created on: Apr 13, 2024
 *      Author: micahbly
 *
 *  Routines for managing individual objects (non-player sprites)
 *
 */



/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "object.h"
#include "api.h"
#include "app.h"
#include "general.h"
#include "kernel.h"
#include "keyboard.h"
#include "level.h"
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

//#pragma zpsym ("global_player");


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/






/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/



// **** CONSTRUCTOR AND DESTRUCTOR *****



// **** SETTERS *****

// Sets the object's direction as specified, sets speed accordingly, sets sprite shape to match
// base_speed is the cardinal direction speed (N/W/E/S), which is halved to get diagonal speed
// shift_factor is number of left shifts that need to happen to get from the base address of the sprite graphic to the desired shape graphic
//   e.g., 9 to jump 512 between shapes for 16x16 sprites with 2 phases, 6 to jump 64 with 8x8 graphics that have no alternate phases.
void Object_SetDirection(Sprite* the_object, uint8_t the_direction, uint8_t base_speed, uint8_t shift_factor)
{
	uint16_t		new_sprite_loc;
	uint8_t			diagonal_speed = base_speed / 2;
	
	the_object->direction_ = the_direction;
	
	switch (the_direction)
	{
		case PLAYER_DIR_NORTH:
			the_object->x_speed_ = 0;
			the_object->y_speed_ = 0 - base_speed;
			break;
			
		case PLAYER_DIR_NORTHEAST:
			the_object->x_speed_ = diagonal_speed;
			the_object->y_speed_ = 0 - diagonal_speed;
			break;
			
		case PLAYER_DIR_EAST:
			the_object->x_speed_ = base_speed;
			the_object->y_speed_ = 0;
			break;
			
		case PLAYER_DIR_SOUTHEAST:
			the_object->x_speed_ = diagonal_speed;
			the_object->y_speed_ = diagonal_speed;
			break;
			
		case PLAYER_DIR_SOUTH:
			the_object->x_speed_ = 0;
			the_object->y_speed_ = base_speed;
			break;
			
		case PLAYER_DIR_SOUTHWEST:
			the_object->x_speed_ =  0 - diagonal_speed;
			the_object->y_speed_ = diagonal_speed;
			break;
			
		case PLAYER_DIR_WEST:
			the_object->x_speed_ = 0 - base_speed;
			the_object->y_speed_ = 0;
			break;
			
		case PLAYER_DIR_NORTHWEST:
			the_object->x_speed_ = 0 - diagonal_speed;
			the_object->y_speed_ = 0 - diagonal_speed;
			break;
		
		default:
			// shouldn't get hit, but let's have some fun if it does...
			the_object->x_speed_ = base_speed * 4;
			the_object->y_speed_ = base_speed * 2;			
	}
	
	//new_sprite_loc = the_object->addr_base_lomed_ + (uint16_t)(the_object->direction_/2) * (uint16_t)512;
	// temp debug hack: humans have only 4 shapes, missiles have 8. hard-coded the human expectation here. 
	//new_sprite_loc = the_object->addr_base_lomed_ + ((uint16_t)(the_object->direction_/2) << shift_factor);
	if (shift_factor == MISSILE_L_SHIFT_PER_SHAPE)
	{
		new_sprite_loc = the_object->addr_base_lomed_ + ((uint16_t)(the_object->direction_) << shift_factor);
	}
	else
	{
		new_sprite_loc = the_object->addr_base_lomed_ + ((uint16_t)(the_object->direction_/2) << shift_factor);
	}
	
	the_object->addr_lo_ = new_sprite_loc & 0xFF;
	the_object->addr_med_ = new_sprite_loc >> 8;
	the_object->addr_med_alt_ =the_object->addr_med_ + 0x01;	// alt frames are 256 bytes away from main frame
}








// **** GETTERS *****








// **** COMBAT FUNCTIONS *****


// check if the rectangle describing the object's sprite is in collision with the passed rectangle
bool Object_CollisionCheck(Sprite* the_object, Rectangle* r2)
{
	Rectangle* r1 = (Rectangle*)&the_object->x1_;	// coerce the x1,y1,x2,y2 properties into a Rect object.
	
	//DEBUG_OUT(("%s %d: checking sprite %p for collision; %u, %u - %u, %u vs %u, %u - %u, %u", __func__, __LINE__, the_object, the_object->x1_, the_object->y1_, the_object->x2_, the_object->y2_, r2->x1, r2->y1, r2->x2, r2->y2));
	
	return General_RectIntersect(r1, r2);
}



// **** OTHER FUNCTIONS *****


// change the x/y location based on the already programmed velocity. no bounds checking. no actual sprite updating.
void Object_Move(Sprite* the_object)
{
	the_object->x1_ += the_object->x_speed_;
	the_object->y1_ += the_object->y_speed_;
	the_object->x2_ = the_object->x1_ + HUMAN_SPRITE_WIDTH;
	the_object->y2_ = the_object->y1_ + HUMAN_SPRITE_HEIGHT;
}


// check x and y to make sure they are inside playfield and not blocked by obstacle. return false if the object is blocked.
bool Object_MoveIsValid(Sprite* the_object)
{
	bool	valid = true;
	
	if (the_object->x1_ <= LEVEL_MIN_X)
	{
		the_object->x1_ = LEVEL_MIN_X + 2;
		valid = false;
	}
	else if (the_object->x2_ >= LEVEL_MAX_X)
	{
		the_object->x2_ = LEVEL_MAX_X - 2;
		valid = false;
	}

	if (the_object->y1_ <= LEVEL_MIN_Y)
	{
		the_object->y1_ = LEVEL_MIN_Y + 2;
		valid = false;
	}
	else if (the_object->y2_ >= LEVEL_MAX_Y)
	{
		the_object->y2_ = LEVEL_MAX_Y - 2;
		valid = false;
	}
	
	return valid;
}

