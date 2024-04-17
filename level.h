/*
 * level.h
 *
 *  Created on: April 6, 2024
 *      Author: micahbly
 */
 
#ifndef LEVEL_H_
#define LEVEL_H_



/* about this class: Level
 *
 * This represents the game playfield, and all the non-player objects on the field
 *
 *** things this class needs to be able to do
 *
 * initialize the level (populate monsters, etc.)
 * manage one turn/round of play
 * update positions of player, monsters, bullets, loot, etc.
 * manage combat
 * manage combat results
 *
 *
 */


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/


// project includes
// #include "object.h"
// #include "monster.h"
#include "general.h"

// C includes
#include <stdbool.h>
#include <stdint.h>

// cc65 includes


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

#define LEVEL_MIN_X		32			// sprites partially off screen at values closer to edge than this
#define LEVEL_MIN_Y		32			// sprites partially off screen at values closer to edge than this
#define LEVEL_MAX_X		(320+16)	// sprites partially off screen at values closer to edge than this
#define LEVEL_MAX_Y		(240+16)	// sprites partially off screen at values closer to edge than this

// 64 max sprites. player uses 1. 
#define LEVEL_MAX_MISSILES				33 // max number of missiles that can appear in the game at once. need to balance player and monster firing accordingly.
#define LEVEL_MAX_HUMANS				10 // max number of monsters that can appear in the game at once.

#define LEVEL_MAX_CHIPS					4 // max number of computer chips that can appear in the game at once.
#define LEVEL_MAX_CLIPS					8 // max number of ammo clips that can appear in the game at once.
#define LEVEL_MAX_POO					8 // max number of human droppings that can appear in the game at once.

#define LEVEL_MAX_SPRITES				63 // max number of non-player sprites of any kind that can appear in the game at once. (humans, missiles, chips, clips, poo)

/*****************************************************************************/
/*                        Player decisions/actions/events                    */
/*****************************************************************************/



/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

// typedef struct Level Level;					// defined in level.h
// 
// struct Level
// {
// 	uint8_t		num_monsters_;					// count of active monsters (monsters_)
// 	uint8_t		num_objects_;					// count of active objects (objects_)
// };

// typedef struct Missile 
// {
// 	uint16_t	x_;				// current location in pixels
// 	uint16_t	y_;
// 	int8_t		x_speed_;		// velocity in x dir. # of pixels per frame. negative or positive.
// 	int8_t		y_speed_;
// 	uint8_t		damage_;		// damage it would do if it hits monster or player.
// 	uint8_t		sprite_id_;		// 1-63, the ID of the sprite associated with this object.
// } Missile;
// 
// typedef struct Monster 
// {
// 	uint16_t	x_;				// current location in pixels
// 	uint16_t	y_;
// 	int8_t		x_speed_;		// velocity in x dir. # of pixels per frame. negative or positive.
// 	int8_t		y_speed_;
// 	uint8_t		hp_;			// current HP
// 	uint8_t		value_;			// value if killed, in points
// 	uint8_t		poo_countdown_;	// # of frames until it poos
// 	uint8_t		sprite_id_;		// 1-63, the ID of the sprite associated with this object.
// } Monster;


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// **** CONSTRUCTOR AND DESTRUCTOR *****

// configure level for a new game - place humans, set up tiles, re-initialize sprites, etc. 
void Level_Initialize(void);


// **** SETTERS *****



// **** GETTERS *****





// **** PLAYFIELD CREATION RELATED *****




// **** OTHER FUNCTIONS *****

// // randomly picks a room, then picks a spot within that room, makes a coordinate. ensures that target spot is empty.
// void Level_PickRandomLocation(Coordinate* the_coordinate);

// // Zeros out the passed object, and removes from the ptp array for the Level. Resorts that array. Still exists in the global_level->objects_ pointer array.
// void Level_RemoveObject(Object* the_object);
// 
// // Zeros out the passed monster, and removes from the ptp array for the Level. Resorts that array. Still exists in the global_level->monsters_ pointer array.
// void Level_RemoveMonster(Object* the_monster);
// 
// // takes a player/monster/other object and adds it to the level, including positioning the player. Keep in level.c, because same code is used for teleport.
// void Level_RandomlyPlaceObject(Object* the_object);








// ***** SCENE MANAGEMENT FUNCTIONS ****


// update graphic state of all non-player sprites
// changes to west facing if going left, right leg if it was left leg, etc.
// checks current velocity and resets if human has hit edge of screen
void Level_UpdateSprites(void);

// render all active non-player sprites that need render update
// this is the only function that actually touches the sprite registers
void Level_RenderSprites(void);






// **** COMBAT MODE FUNCTIONS *****

// player wants to shoot: see if a missile is available, if player's gun is full, if refire time is up, etc. 
bool Level_PlayerAttemptShoot(void);


// // iterates through the complete level monster array looking for the ACTIVE monster that matches the coordinate and char passed
// Object* Level_FindActiveMonsterByLoc(Coordinate* the_location);
// 
// // finds the nearest monster to the passed location. If no monster closer than IN_MONSTER_RANGE, returns NULL
// Object* Level_FindNearestMonster(Coordinate* the_location);
// 
// // finds every active monster within range of the passed location and applies the passed function and animation
// // if except_at_epicenter is true, a monster at the exact location passed will not be affected. This allows Bobs to blow up other monsters but not them.
// // returns false if no monsters were affected
// bool Level_AffectAllInRangeMonsters(Coordinate* the_location, uint8_t the_range, bool except_at_epicenter, uint8_t the_animation, bool (*action_function)(Object*, unsigned char), unsigned char the_arg);
// 
// // finds every active object within range of the passed location and destroys it
// // this removes the object from the level's list of objects
// // returns true if any objects were affected
// bool Level_DestroyAllInRangeObjects(Coordinate* the_location, uint8_t the_range);
// 
// // calculate the end point of an object thrown/shot for distance x, from spot y, in direction z
// // puts the location into the coordinate pointer passed
// // draws the projectile moving through space
// // returns true if the path of object was stopped by a monster or a wall
// bool Level_ManageProjectilePath(Coordinate* start_coordinate, Coordinate* end_coordinate, uint8_t the_range, uint8_t direction_code);
// 
// // blow up the contents of the map, for the radius and starting point passed
// // turn wall and door into floor
// // retile map in the affected area
// // leave monsters and loot as is
// void Level_BlowUpArea(Coordinate* start_coordinate, uint8_t radius);


// **** Debug functions *****

// void Level_Print(void);




#endif /* LEVEL_H_ */
