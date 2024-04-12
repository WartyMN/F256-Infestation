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

Missile						global_missiles[LEVEL_MAX_MISSILES];
Monster						global_monsters[LEVEL_MAX_MONSTERS];

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









// **** GETTERS *****









// ***** ITEM Management FUNCTIONS ****





// **** COMBAT FUNCTIONS *****




// **** OTHER FUNCTIONS *****


// 
// // add the player to the passed level at a random spot
// void Player_PlaceRandomly(void)
// {
// 	Coordinate 	new_location;
// 	
// 	// pick a legal place within the playfield, which has 32 pix boundary on all sides for sprites.
// 	new_location.x = App_GetRandom(320-32-32) + 32;
// 	new_location.y = App_GetRandom(240-32-32) + 32;
// 	
// 	//Level_PickRandomLocation(&new_location);
// 	// randomly picks a room, then picks a spot within that room and assigns that to the passed coordinate
// 
// 	// set object's location
// 	Player_MoveToLocation(&new_location);
// 	
// 	return;
// }
// 
// 
