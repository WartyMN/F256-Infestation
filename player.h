/*
 * player.h
 *
 *  Created on: Apr 6, 2024
 *      Author: micahbly
 */

#ifndef PLAYER_H_
#define PLAYER_H_

/* about this class
 *
 *  Routines for managing the player
 *
 *  Needed functionality:
 *  - ensure player can't leave bounds of field
 *  - ensure player can't move through walls and other obstacles
 *  - check for player death conditions
 *  - 
 */

/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

#include "app.h"
#include "text.h"
#include <stdint.h>


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

#define PARAM_DROP_OBJECT_SILENTLY			true	// boolean parameter for Player_DropObject()
#define PARAM_DROP_OBJECT_WITH_MESSAGE		false	// boolean parameter for Player_DropObject()

#define PARAM_SET_ITEM_SILENTLY				true	// boolean parameter for Player_SetWeapon, etc. Will prevent comms buffer message.
#define PARAM_SET_ITEM_WITH_MESSAGE			false	// boolean parameter for Player_SetWeapon, etc. Will send comms buffer message.

#define PLAYER_STARTING_LIVES				3
#define PLAYER_PER_LEVEL_HP_BASE			25
#define PLAYER_MAX_HP						255
#define PLAYER_MAX_WEAPONS					5		// the types of weapons player is pre-supplied with
#define PLAYER_STARTING_WARP_COUNT			3
#define PLAYER_MAX_WARP_COUNT				6
#define PLAYER_POINTS_FOR_EXTRA_LIFE		10000

#define PLAYER_DIR_NORTH					0		// directions player is facing: used to pick sprite model and to know which way to fire bullets
#define PLAYER_DIR_NORTHEAST				1
#define PLAYER_DIR_EAST						2
#define PLAYER_DIR_SOUTHEAST				3
#define PLAYER_DIR_SOUTH					4
#define PLAYER_DIR_SOUTHWEST				5
#define PLAYER_DIR_WEST						6
#define PLAYER_DIR_NORTHWEST				7

#define PLAYER_SLOW_MOVES_PER_HALF_TURN				1
#define PLAYER_NORMAL_MOVES_PER_HALF_TURN			2
#define PLAYER_SPEEDY_MOVES_PER_HALF_TURN			4
#define PLAYER_SUPER_SPEEDY_MOVES_PER_HALF_TURN		8
#define PLAYER_TEMP_SPEED_LENGTH					1000

#define PLAYER_WEAPON_PISTOL				0
#define PLAYER_WEAPON_SUB_MG				1
#define PLAYER_WEAPON_HEAVY_MG				2
#define PLAYER_WEAPON_20MM_CANNON			3
#define PLAYER_WEAPON_FLAMETHROWER			4

#define PLAYER_WEAPON_PISTOL_DMG			5	// damage done for any hit with 1 bullet of this weapon
#define PLAYER_WEAPON_SUB_MG_DMG			3
#define PLAYER_WEAPON_HEAVY_MG_DMG			5
#define PLAYER_WEAPON_20MM_CANNON_DMG		20
#define PLAYER_WEAPON_FLAMETHROWER_DMG		15

#define PLAYER_WEAPON_PISTOL_CLIP			9	// number of bullets that fit into a single clip
#define PLAYER_WEAPON_SUB_MG_CLIP			30
#define PLAYER_WEAPON_HEAVY_MG_CLIP			100
#define PLAYER_WEAPON_20MM_CANNON_CLIP		50
#define PLAYER_WEAPON_FLAMETHROWER_CLIP		150

#define PLAYER_WEAPON_PISTOL_SPEED			5	// time between firing events
#define PLAYER_WEAPON_SUB_MG_SPEED			1
#define PLAYER_WEAPON_HEAVY_MG_SPEED		1
#define PLAYER_WEAPON_20MM_CANNON_SPEED		3
#define PLAYER_WEAPON_FLAMETHROWER_SPEED	1

#define PLAYER_WEAPON_PISTOL_RELOAD			5	// time it takes to reload if it runs out of ammo
#define PLAYER_WEAPON_SUB_MG_RELOAD			5
#define PLAYER_WEAPON_HEAVY_MG_RELOAD		5
#define PLAYER_WEAPON_20MM_CANNON_RELOAD	15
#define PLAYER_WEAPON_FLAMETHROWER_RELOAD	30

#define PLAYER_WEAPON_PISTOL_MAX_CLIPS			20	// max number of clips of this weapon type that the player can carry
#define PLAYER_WEAPON_SUB_MG_MAX_CLIPS			18
#define PLAYER_WEAPON_HEAVY_MG_MAX_CLIPS		10
#define PLAYER_WEAPON_20MM_CANNON_MAX_CLIPS		5
#define PLAYER_WEAPON_FLAMETHROWER_MAX_CLIPS	4

#define PLAYER_WEAPON_PISTOL_STARTING_CLIPS			20	// number of clips of this weapon type that the player starts the game with
#define PLAYER_WEAPON_SUB_MG_STARTING_CLIPS			18
#define PLAYER_WEAPON_HEAVY_MG_STARTING_CLIPS		10
#define PLAYER_WEAPON_20MM_CANNON_STARTING_CLIPS	5
#define PLAYER_WEAPON_FLAMETHROWER_STARTING_CLIPS	4

#define PLAYER_CRUSH_DAMAGE					10	// damage player does when running into/over enemies
#define PLAYER_DAMAGE_FROM_SLIMING			15	// damage player absorbs when running over a human

#define IS_SLOW			0x20 // gets 1 move every 2 turns
#define IS_SPEEDY		0x40 // gets 2 moves per turn
#define IS_SUPER_SPEEDY	0x80 // gets 4 moves per turn


/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/

/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

typedef struct Weapon {
	uint8_t		damage_;
	uint8_t		clip_size_;
	uint8_t		firing_speed_;
	uint8_t		reload_speed_;
	uint8_t		max_clips_;
	char		name_[17];
} Weapon;


typedef struct Player {
	int16_t			healthy_hp_;				// HP when healthy
	uint8_t			clips_[PLAYER_MAX_WEAPONS];	// for each weapon, the count of clips remaining, including the current clip
	uint8_t			bullets_in_clip_[PLAYER_MAX_WEAPONS];	// for each weapon, the count of bullets remaining in the current clip
	uint8_t			current_weapon_id_;			// index to which weapon player is currently using. should maybe be ZP
	uint16_t		temp_speed_countdown_;		// when a speed change is encountered, this is set, and decremented every frame until 0 then speed is reset.
	uint8_t			special_conditions_;		// 2x speed, slow, paralyzed, etc.
// 	uint8_t			badge_points_[BADGES_MAX_TYPES];
	// location covered in zp global zp_px, zp_py
	// speed covered in zp global zp_speed
	// HP covered in zp global zp_hp
	// points covered in zp global zp_points
} Player;

/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// **** SETTERS *****

// player will move at half-speed. gets 1 move every 2 game turns.
void Player_SetSlowSpeed(void);

// player will move at normal speed. 1 move every 1 game turn.
void Player_SetNormalSpeed(void);

// player will move at "fast" speed. 2 moves every 1 game turn.
void Player_SetFastSpeed(void);

// player will move at "super fast" speed. 4 moves every 1 game turn.
void Player_SetSuperFastSpeed(void);

// void Player_AddXP(const uint16_t the_xp);
// // adds the specified XP to the player. if it passes the next-level threshold, will bump player level.
// 
// // removes the specified XP to the player. will NOT reduce player level, even if it puts them under the threshold of the level they are on.
// void Player_RemoveXP(const uint16_t the_xp);

// Sets the player's current weapon.
// if -1 is passed, will understand to mean "no weapon"
void Player_SetWeapon(int8_t weapon_id);

// Sets the player's current weapon to the next  item available. If none available, sets to NULL. if only 1 available, makes no change.
void Player_SetNextWeapon(void);

// // gives player random amount of new HP, and increment strength
// void Player_LevelUp(void);
// 
// // opposite of LevelUp: player loses random amount of HP, and strength gets decremented
// // returns true if player died as a result of levelDown action
// bool Player_LevelDown(void);

// removes a life from player, converting to new HP. if no more lives available, ends game
void Player_LoseLife(void);

// adds a random amount to the healthy and current HP of the player. 
void Player_IncreaseHP(void);

// removes a random amount from the healthy and current HP of the player
void Player_DecreaseHP(void);

// sets the flag for the specified special condition to OFF
void Player_ClearSpecialCondition(uint8_t the_flag);

// sets the flag for the specified special condition to ON
void Player_SetSpecialCondition(uint8_t the_flag);

                    


// **** GETTERS *****

uint8_t Player_GetHealthyHP(void);

uint8_t Player_GetWeaponID(void);


// ***** ITEM Management FUNCTIONS ****


// player has picked up an ammo clip for the specified type of weapon. add the clip to inventory.
// returns false if player already had max # of clips for that weapon type
bool Player_PickUpClip(uint8_t weapon_id);

// player has picked up the specified type of computer chip. modify speed accordingly
bool Player_PickUpChip(uint8_t chip_id);


// ***** COMBAT FUNCTIONS ****

// removes the passed amount of current HP
void Player_TakeDamage(int16_t the_damage);



// **** OTHER FUNCTIONS *****

// // return the rank the player has for the passed badge type
// // returns -1 if player has not earned any badges of this type
// int8_t Player_GetBadgeRank(uint8_t the_badge_type);
// 
// // increment the badge count for the passed type, and inform player if they earned a new level
// void Player_IncrementBadgePoints(uint8_t the_badge_type);

// moves player from current loc to new loc; updates visuals. move must have been validated beforehand!
//  set global_first_map_appearance to true beforehand, if you do NOT want the old location redrawn.
void Player_MoveToLocation(Coordinate* new_location);

// check px and py to make sure they are inside playfield; adjusts if necessary
void Player_ValidateLocation(void);



#endif /* PLAYER_H_ */
