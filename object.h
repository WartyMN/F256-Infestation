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

typedef struct Object {
	uint8_t		type_id_;		// chip, clip, poo
	uint16_t	x_;				// current location in pixels
	uint16_t	y_;
	uint8_t		sprite_id_;		// 1-63, the ID of the sprite associated with this object.
} Object;


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// **** SETTERS *****

                    


// **** GETTERS *****



// ***** ITEM Management FUNCTIONS ****




// ***** COMBAT FUNCTIONS ****




// **** OTHER FUNCTIONS *****



#endif /* OBJECT_H_ */
