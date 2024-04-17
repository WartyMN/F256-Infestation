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
#include "player.h"
#include "app.h"
#include "general.h"
#include "kernel.h"
#include "keyboard.h"
#include "level.h"
#include "memory.h"
#include "object.h"
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

Weapon		global_weapon[PLAYER_MAX_WEAPONS] = 
{
	{PLAYER_WEAPON_PISTOL_DMG, 		PLAYER_WEAPON_PISTOL_CLIP,		PLAYER_WEAPON_PISTOL_SPEED,			PLAYER_WEAPON_PISTOL_RELOAD,		PLAYER_WEAPON_PISTOL_MAX_CLIPS,			"8mm peacemaker  "},
	{PLAYER_WEAPON_SUB_MG_DMG,		PLAYER_WEAPON_SUB_MG_CLIP,		PLAYER_WEAPON_SUB_MG_SPEED,			PLAYER_WEAPON_SUB_MG_RELOAD,		PLAYER_WEAPON_SUB_MG_MAX_CLIPS,			"8mm pieces maker"},
	{PLAYER_WEAPON_HEAVY_MG_DMG,	PLAYER_WEAPON_HEAVY_MG_CLIP,	PLAYER_WEAPON_HEAVY_MG_SPEED,		PLAYER_WEAPON_HEAVY_MG_RELOAD,		PLAYER_WEAPON_HEAVY_MG_MAX_CLIPS,		"13mm auto gun   "},
	{PLAYER_WEAPON_20MM_CANNON_DMG,	PLAYER_WEAPON_20MM_CANNON_CLIP,	PLAYER_WEAPON_20MM_CANNON_SPEED,	PLAYER_WEAPON_20MM_CANNON_RELOAD,	PLAYER_WEAPON_20MM_CANNON_MAX_CLIPS,	"20mm auto cannon"},
	{PLAYER_WEAPON_FLAMETHROWER_DMG,PLAYER_WEAPON_FLAMETHROWER_CLIP,PLAYER_WEAPON_FLAMETHROWER_SPEED,	PLAYER_WEAPON_FLAMETHROWER_RELOAD,	PLAYER_WEAPON_FLAMETHROWER_MAX_CLIPS,	"MkI sterilizer  "},
};

Chip		global_computer_chip[MAX_CHIPS] = 
{
	{&Player_SetSlowSpeed,		"z80"},
	{&Player_DecreaseHP,		"8088"},
	{&Player_IncreaseHP,		"6502"},
	{&Player_SetFastSpeed,		"68030"},
	{&Player_SetSuperFastSpeed,	"8800"},
};


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

Player*				global_player;

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




// player will move at half-speed. gets 1 move every 2 game turns.
void Player_SetSlowSpeed(void)
{
	Player_SetSpecialCondition(IS_SLOW);
	Player_ClearSpecialCondition(IS_SPEEDY);
	Player_ClearSpecialCondition(IS_SUPER_SPEEDY);
	zp_speed = PLAYER_SLOW_MOVES_PER_HALF_TURN;	
	global_player->temp_speed_countdown_ = App_GetRandom(PLAYER_TEMP_SPEED_LENGTH);

// 	// set a timer to revert to normal speed
// 	Action_New(Player_SetNormalSpeed, global_player, OBJECT_TYPE_PLAYER, num_turns, ACTION_TYPE_FUTURE_BEFORE);
}

// player will move at normal speed. 1 move every 1 game turn.
void Player_SetNormalSpeed(void)
{
	Player_ClearSpecialCondition(IS_SLOW);
	Player_ClearSpecialCondition(IS_SPEEDY);
	Player_ClearSpecialCondition(IS_SUPER_SPEEDY);
	zp_speed = PLAYER_NORMAL_MOVES_PER_HALF_TURN;
	global_player->temp_speed_countdown_ = 0;
}


// player will move at "fast" speed. 2 moves every 1 game turn.
void Player_SetFastSpeed(void)
{
	Player_ClearSpecialCondition(IS_SLOW);
	Player_SetSpecialCondition(IS_SPEEDY);
	Player_ClearSpecialCondition(IS_SUPER_SPEEDY);
	zp_speed = PLAYER_SPEEDY_MOVES_PER_HALF_TURN;
	global_player->temp_speed_countdown_ = App_GetRandom(PLAYER_TEMP_SPEED_LENGTH);

// 	// set a timer to revert to normal speed
// 	Action_New(Player_SetNormalSpeed, global_player, OBJECT_TYPE_PLAYER, num_turns, ACTION_TYPE_FUTURE_BEFORE);
}


// player will move at "super fast" speed. 4 moves every 1 game turn.
void Player_SetSuperFastSpeed(void)
{
	Player_ClearSpecialCondition(IS_SLOW);
	Player_ClearSpecialCondition(IS_SPEEDY);
	Player_SetSpecialCondition(IS_SUPER_SPEEDY);
	zp_speed = PLAYER_SUPER_SPEEDY_MOVES_PER_HALF_TURN;
	global_player->temp_speed_countdown_ = App_GetRandom(PLAYER_TEMP_SPEED_LENGTH);

// 	// set a timer to revert to normal speed
// 	Action_New(Player_SetNormalSpeed, global_player, OBJECT_TYPE_PLAYER, num_turns, ACTION_TYPE_FUTURE_BEFORE);
}


// removes a life from player, converting to new HP. if no more lives available, ends game
void Player_LoseLife(void)
{
	if (zp_lives < 1)
	{
		App_GameOver();
	}
	else
	{
		--zp_lives;
		*(uint8_t*)ZP_HP = 0;
		Player_IncreaseHP();
		
		// TODO: play gong or something when life used up?
	}
}


// adds a random amount to the healthy and current HP of the player.
void Player_IncreaseHP(void)
{
	int16_t		the_random_number;
	
	the_random_number = App_GetRandom(PLAYER_PER_LEVEL_HP_BASE);
	zp_hp += the_random_number;
	global_player->healthy_hp_ += the_random_number;
}


// removes a random amount from the healthy and current HP of the player
void Player_DecreaseHP(void)
{
	int16_t		the_random_number;
	
	the_random_number = App_GetRandom(PLAYER_PER_LEVEL_HP_BASE);
	
	if (zp_hp < the_random_number)
	{
		zp_hp = 0;
	}
	else
	{
		zp_hp -= the_random_number;
	
	}
	
	global_player->healthy_hp_ -= the_random_number;
}


// Sets the player's current weapon.
// if -1 is passed, will understand to mean "no weapon"
void Player_SetWeapon(int8_t weapon_id)
{
	// did we get passed 'no weapon'?
	if (weapon_id < 0 || weapon_id >= PLAYER_MAX_WEAPONS || weapon_id == global_player->current_weapon_id_)
	{
		return;
	}

	// now set the current weapon to the new item
	global_player->current_weapon_id_ = weapon_id;
	
	// set the fast access globals for the current weapon
	zp_num_clips = global_player->clips_[weapon_id];
	zp_num_bullets = global_player->bullets_in_clip_[weapon_id];	
	zp_bullet_dmg = global_weapon[weapon_id].damage_;
	
	return;
}


// Sets the player's current weapon to the next  item available. If none available, sets to NULL. if only 1 available, makes no change.
void Player_SetNextWeapon(void)
{
	uint8_t		next_weapon_id;
	
	// update count of bullets left in clip of current weapon
	global_player->bullets_in_clip_[global_player->current_weapon_id_] = zp_num_bullets;
	
	next_weapon_id = global_player->current_weapon_id_;
	++next_weapon_id;
	
	if (next_weapon_id >= PLAYER_MAX_WEAPONS)
	{
		next_weapon_id = 0;
	}
	
	Player_SetWeapon(next_weapon_id);
}


// sets the flag for the specified special condition to ON
void Player_SetSpecialCondition(uint8_t the_flag)
{
	global_player->special_conditions_ |= (the_flag);
}


// sets the flag for the specified special condition to OFF
void Player_ClearSpecialCondition(uint8_t the_flag)
{
	global_player->special_conditions_ &= ~(the_flag);
}







// **** GETTERS *****


// // gets the HP a player would have if healthy.
// uint16_t Player_GetHealthyHP(void)
// {
// 	return global_player->healthy_hp_;
// }


uint8_t Player_GetWeaponID(void)
{
	return global_player->current_weapon_id_;
}






// ***** ITEM Management FUNCTIONS ****



// player has picked up an ammo clip for the specified type of weapon. add the clip to inventory.
// returns false if player already had max # of clips for that weapon type
bool Player_PickUpClip(uint8_t weapon_id)
{
	uint8_t		num_clips_owned;
	
	num_clips_owned = global_player->clips_[weapon_id];
	
	if (num_clips_owned >= global_weapon[weapon_id].max_clips_)
	{
		// oops, already have too many. 
		return false;
	}
	
	++global_player->clips_[weapon_id];
	
	return true;
}


// player has picked up the specified type of computer chip. modify speed accordingly
bool Player_PickUpChip(uint8_t chip_id)
{
	//TODO : show message
	
	// run whatever function is associated with this chip
	(global_computer_chip[chip_id].fn_)();
	
	return true;
}




// **** COMBAT FUNCTIONS *****


// removes the passed amount of current HP
void Player_TakeDamage(int16_t the_damage)
{
	if (zp_hp < the_damage)
	{
		zp_hp = 0;
	}
	else
	{
		zp_hp -= the_damage;	
	}
}


// try to reload the current weapon
// returns true if player was absle to start reload process (i.e., had another clip of right kind)
bool Player_Reload(void)
{
	if (zp_num_clips > 0)
	{
		--zp_num_clips;
		global_player->clips_[global_player->current_weapon_id_] = zp_num_clips;
		zp_num_bullets = global_weapon[global_player->current_weapon_id_].clip_size_;
		
		return true;
	}
	else
	{
		return false;
	}
}



// **** OTHER FUNCTIONS *****



// moves player from current loc to new loc; updates visuals. move must have been validated beforehand!
//  set global_first_map_appearance to true beforehand, if you do NOT want the old location redrawn.
void Player_MoveToLocation(Coordinate* new_location)
{
// 	// record the transfer
// 	Coordinate_Transfer(new_location, &global_player->location_);
// 
// 	// draw object at the new location
// 	Screen_SetScreenCharAtCoord(SCREEN_MASTER_MAP, &global_player->location_, &global_player->my_char_);
// 	
// 	// set the room-you-are-in property
// 	global_player->room_num_ = Level_GetRoomNumberFromLocation(&global_player->location_);

// 	R16(SPRITE0_X_LO) = zp_px;		
// 	R16(SPRITE0_Y_LO) = zp_py;

	zp_px = new_location->x;
	zp_py = new_location->y;
	
	Player_ValidateLocation();
}


// check px and py to make sure they are inside playfield; adjusts if necessary
void Player_ValidateLocation(void)
{
	if (zp_px < LEVEL_MIN_X)
	{
		zp_px = LEVEL_MIN_X;
	}
	
	if (zp_py < LEVEL_MIN_Y)
	{
		zp_py = LEVEL_MIN_Y;
	}
	
	if (zp_px > LEVEL_MAX_X)
	{
		zp_px = LEVEL_MAX_X;
	}
	
	if (zp_py > LEVEL_MAX_Y)
	{
		zp_py = LEVEL_MAX_Y;
	}
}


// // return the rank the player has for the passed badge type
// // returns -1 if player has not earned any badges of this type
// int8_t Player_GetBadgeRank(uint8_t the_badge_type)
// {
// 	int8_t	this_rank;
// 	uint8_t	this_count;
// 	uint8_t	j;
// 	uint8_t	needed;
// 
// 	this_count = global_player->badge_points_[the_badge_type];
// 	this_rank = -1;
// 
// 	//DEBUG_OUT(("%s %d: i=%i, player badge points=%i", __func__, __LINE__, i, this_count));
// 	
// 	if (this_count > 0)
// 	{
// 		for (j = 0; j < BADGES_NUM_RANKS; j++)
// 		{
// 			needed = badge_data[the_badge_type * BADGES_NUM_RANKS + j];
// 
// 			if (this_count >= needed)
// 			{
// 				this_rank = j;
// 			}
// 			//DEBUG_OUT(("%s %d: needed=%i, this_rank=%i", __func__, __LINE__, needed, this_rank));
// 		}
// 	}
// 
// 	return this_rank;
// }
// 
// 
// // increment the badge count for the passed type, and inform player if they earned a new level
// void Player_IncrementBadgePoints(uint8_t the_badge_type)
// {
// 	int8_t	old_rank;
// 	int8_t	new_rank;
// 	
// 	old_rank = Player_GetBadgeRank(the_badge_type);	
// 
// 	++global_player->badge_points_[the_badge_type];
// 	
// 	new_rank = Player_GetBadgeRank(the_badge_type);	
// 	
// 	if (new_rank > old_rank)
// 	{
// 		strcpy(global_string_buffer2, General_GetString(ID_STR_BADGE_RANK_0 + new_rank));
// 		strcpy(global_string_buffer, General_GetString(ID_STR_MSG_BADGE_EARNED));
// 		sprintf(global_string_buffer3, global_string_buffer, global_string_buffer2, General_GetString(ID_STR_BADGE_SCROLL_USE + the_badge_type));
// 
// 		Buffer_NewMessage(global_string_buffer3);
// 	}	
// }
