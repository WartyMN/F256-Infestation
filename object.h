/*
 * object.h
 *
 *  Created on: Apr 7, 2024
 *      Author: micahbly
 */

#ifndef OBJECT_H_
#define OBJECT_H_

/* about this class
 *
 *  Routines and structures for in-game objects such as computer chips, ammo clips, and human poo (think all-natural land mine)
 *
 *  Needed functionality:
 *  - 
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


#define CHIP_ID_Z80							0
#define CHIP_ID_8088						1
#define CHIP_ID_6502						2
#define CHIP_ID_68030						3
#define CHIP_ID_8800						4
#define MAX_CHIPS							5		// number of known types of computer chips (food/poison)

#define OBJECT_TYPE_CHIP					0		// for Object->type_id_
#define OBJECT_TYPE_CLIP					1
#define OBJECT_TYPE_POO						2
#define OBJECT_TYPE_MISSILE					3
#define OBJECT_TYPE_HUMAN					4

#define HUMAN_SPEED							2		// pixels per turn per vector a human can move. eg, 2 pixels in x or 2 pixels in y dir
#define MISSILE_SPEED						16		// pixels per turn per vector a bullet/missile can move. 16=width of a player or human

/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/

/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

// note: poo type objects don't need a struct. they have no name, there is only kind (so all can use same player-modification function), etc. 

typedef struct Chip {
	void		(*fn_)(void);
	char		name_[6];
} Chip;

typedef struct Sprite {
// -- start of VICKY sprite register mirroring
	uint8_t		state_;				// 1 for active, $40 for 16x16, etc.
	uint8_t		addr_lo_;
	uint8_t		addr_med_;
	uint8_t		addr_hi_;
	uint16_t	x1_;				// current upper/left location in pixels - for sprite register
	uint16_t	y1_;
// -- end of VICKY sprite register mirroring
	uint16_t	x2_;				// current lower/right location in pixels - for detecting collisions
	uint16_t	y2_;				// together, x1,y1,x2,y2 represent a rect object.
	uint8_t*	sprite_reg_addr_;	// 0xD900 - DAF8, the VICKY register of the sprite associated with this object.
	uint8_t		type_id_;			// chip, clip, poo, human, missile
	uint8_t		render_needed_;		// set to true when direction, sprite shape, position, etc, has changed and it needs to be re-rendered
	uint8_t		is_active_;			// dead or alive
	int8_t		x_speed_;			// horizontal speed, in pixels (negative=left)
	int8_t		y_speed_;			// horizontal speed, in pixels (negative=up)
	uint8_t		direction_;			// the direction the sprite is headed, 0-7. match to PLAYER_DIR_NORTH... PLAYER_DIR_NORTHWEST
	uint8_t		addr_med_alt_;		// this is the alternate anim frame (if current is left leg, this is right leg, e.g.)
	uint16_t	addr_base_lomed_;	// the LO+MED part of the base address of the sprite graphics
} Sprite;


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// **** SETTERS *****

// Sets the object's direction as specified, sets speed accordingly, sets sprite shape to match
// base_speed is the cardinal direction speed (N/W/E/S), which is halved to get diagonal speed
// shift_factor is number of left shifts that need to happen to get from the base address of the sprite graphic to the desired shape graphic
//   e.g., 9 to jump 512 between shapes for 16x16 sprites with 2 phases, 6 to jump 64 with 8x8 graphics that have no alternate phases.
void Object_SetDirection(Sprite* the_object, uint8_t the_direction, uint8_t base_speed, uint8_t shift_factor);


// **** GETTERS *****



// ***** ITEM Management FUNCTIONS ****




// ***** COMBAT FUNCTIONS ****

// check if the rectangle describing the object's sprite is in collision with the passed rectangle
bool Object_CollisionCheck(Sprite* the_object, Rectangle* r2);


// **** OTHER FUNCTIONS *****

// change the x/y location based on the already programmed velocity. no bounds checking. no actual sprite updating.
void Object_Move(Sprite* the_object);

// check x and y to make sure they are inside playfield and not blocked by obstacle. return false if the object is blocked.
bool Object_MoveIsValid(Sprite* the_object);


#endif /* OBJECT_H_ */
