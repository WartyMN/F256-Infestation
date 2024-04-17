/*
 * app.h
 *
 *  Created on: Jan 10, 2023
 *      Author: micahbly
 *
 *  A pseudo commander-style 2-column file manager
 *
 */
 
#ifndef FILE_MANAGER_H_
#define FILE_MANAGER_H_

/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/


// project includes

// C includes
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
//#include <string.h>



/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/

#define PARAM_COPY_TO_EM			true	// App_EMDataCopy() parameter
#define PARAM_COPY_FROM_EM			false	// App_EMDataCopy() parameter

// hide __fastcall_ from everything but CC65 (to squash some warnings in LSP/BBEdit)
#ifndef __CC65__
	#define __fastcall__ 
#endif

#ifndef NULL
#  define NULL 0
#endif

#define MAJOR_VERSION	0
#define MINOR_VERSION	1
#define UPDATE_VERSION	1

#define VERSION_NUM_X	0
#define VERSION_NUM_Y	24



/*****************************************************************************/
/*                             MEMORY LOCATIONS                              */
/*****************************************************************************/

// temp storage for data outside of normal cc65 visibility - extra memory!
#define CODE_START							0x799
#define STORAGE_GETSTRING_BUFFER			0x0400	// interbank buffer to temporarily store string data into; used by debug and possibly other code. DO NOT REMOVE.
#define STORAGE_GETSTRING_BUFFER_LEN		256	// 1-page buffer. see cc65 memory config file. this is outside cc65 space.
#define STORAGE_PLAYER						(STORAGE_GETSTRING_BUFFER + STORAGE_GETSTRING_BUFFER_LEN)
#define STORAGE_PLAYER_LEN            		26
#define STORAGE_MISSILES					(STORAGE_PLAYER + STORAGE_PLAYER_LEN)
#define STORAGE_MISSILES_LEN            	(6*10)
#define STORAGE_MONSTERS					(STORAGE_MISSILES + STORAGE_MISSILES_LEN)
#define STORAGE_MONSTERS_LEN            	(10*20)
#define STORAGE_CHIPS						(STORAGE_MONSTERS + STORAGE_MONSTERS_LEN)
#define STORAGE_CHIPS_LEN            		(5*6)

#define STORAGE_STRING_BUFFER_1				(CODE_START - STORAGE_STRING_BUFFER_1_LEN)	// temp string merge/etc buff
#define STORAGE_STRING_BUFFER_1_LEN			204	// 204b buffer. see cc65 memory config file. this is outside cc65 space.
#define MAX_ALLOWED_STRING_SIZE				(STORAGE_STRING_BUFFER_1_LEN - 1)	// arbitrary

#define SPRITE_ROBOT_16F_PHYS_ADDR			0x24000	// player graphic. 16x16, 8 primary shapes, 8 alt shapes for ticktocking
#define SPRITE_ROBOT_16F_LOMED_ADDR			0x4000	// for use when setting sprite registers, assuming we set the 02 part already.
#define SPRITE_ROBOT_16F_LO_ADDR			0x00	// for use when setting sprite registers byte by byte
#define SPRITE_ROBOT_16F_MED_ADDR			0x40	// for use when setting sprite registers byte by byte
#define SPRITE_ROBOT_16F_HI_ADDR			0x02	// for use when setting sprite registers byte by byte

#define SPRITE_HUMAN_1_8F_PHYS_ADDR			0x25000	// Human graphic style 1. 16x16, 4 primary shapes, 4 alt shapes for ticktocking
#define SPRITE_HUMAN_1_8F_LOMED_ADDR		0x5000	// for use when setting sprite registers, assuming we set the 02 part already.
#define SPRITE_HUMAN_1_8F_LO_ADDR			0x00	// for use when setting sprite registers byte by byte
#define SPRITE_HUMAN_1_8F_MED_ADDR			0x50	// for use when setting sprite registers byte by byte
#define SPRITE_HUMAN_1_8F_HI_ADDR			0x02	// for use when setting sprite registers byte by byte

#define SPRITE_BULLET_S_PHYS_ADDR			0x25800	// the smaller bullet graphic. 8x8, 8 shapes, no alt shapes
#define SPRITE_BULLET_S_LOMED_ADDR			0x5800	// for use when setting sprite registers, assuming we set the 02 part already.
#define SPRITE_BULLET_S_LO_ADDR				0x00	// for use when setting sprite registers byte by byte
#define SPRITE_BULLET_S_MED_ADDR			0x58	// for use when setting sprite registers byte by byte
#define SPRITE_BULLET_S_HI_ADDR				0x02	// for use when setting sprite registers byte by byte

#define SPRITE_BULLET_L_PHYS_ADDR			0x25A00	// the larger bullet graphic. 8x8, 8 shapes, no alt shapes
#define SPRITE_BULLET_L_LOMED_ADDR			0x5A00	// for use when setting sprite registers, assuming we set the 02 part already.
#define SPRITE_BULLET_L_LO_ADDR				0x00	// for use when setting sprite registers byte by byte
#define SPRITE_BULLET_L_MED_ADDR			0x5A	// for use when setting sprite registers byte by byte
#define SPRITE_BULLET_L_HI_ADDR				0x02	// for use when setting sprite registers byte by byte

#define TILEMAP_PHYS_ADDR					0x25DA8
#define TILEMAP_LEN							0x600	// 20 tiles across, 15 tiles call @16x16 each = 320x240, store each index as 2 bytes = 600 bytes
#define TILEMAP_LO_ADDR						0xA8	// for use when setting tilemap registers byte by byte
#define TILEMAP_MED_ADDR					0x5D	// for use when setting tilemap registers byte by byte
#define TILEMAP_HI_ADDR						0x02	// for use when setting tilemap registers byte by byte
#define TILEMAP_SLOT						0x05	// CPU slot to map it into temporarily when need to adjust
#define TILEMAP_VALUE						0x12	// EM slot it lives in
#define TILEMAP_ADDR_IN_CPU_SPACE			(TILEMAP_PHYS_ADDR - 0x24000 + 0xA000)	// when mapped into CPU space, the local ADDR

#define TILESET_PHYS_ADDR					0x26000
#define TILESET_LEN							4864
#define TILESET_LO_ADDR						0x00	// for use when setting tileset registers byte by byte
#define TILESET_MED_ADDR					0x60	// for use when setting tileset registers byte by byte
#define TILESET_HI_ADDR						0x02	// for use when setting tileset registers byte by byte

#define PLAYER_SPRITE_WIDTH					16		// used for collision detection, etc. 
#define PLAYER_SPRITE_HEIGHT				16		// used for collision detection, etc. 
#define PLAYER_SPRITE_TICKTOCK_DIVISOR		20000		// used for control timing of flipping between animation cells for a given sprite (tank treads, etc.)
#define PLAYER_BYTES_PER_SHAPE				(16*16*2)	// 512 bytes between each primary shape. (1 alt shape per primary)
#define PLAYER_L_SHIFT_PER_SHAPE			9		// 9 left shifts multiplies by 512

#define HUMAN_SPRITE_WIDTH					16		// used for collision detection, etc. 
#define HUMAN_SPRITE_HEIGHT					16		// used for collision detection, etc. 
#define HUMAN_SPRITE_TICKTOCK_DIVISOR		300		// used for control timing of flipping between animation cells for a given sprite (left foot, right foot, etc.)
#define HUMAN_BYTES_PER_SHAPE				(16*16*2)	// 512 bytes between each primary shape. (1 alt shape per primary)
#define HUMAN_L_SHIFT_PER_SHAPE				9		// 9 left shifts multiplies by 512

#define MISSILE_SPRITE_WIDTH				8		// used for collision detection, etc. 
#define MISSILE_SPRITE_HEIGHT				8		// used for collision detection, etc. 
#define MISSILE_BYTES_PER_SHAPE				(8*8)	// 64 bytes between each primary shape. (no alts for missiles)
#define MISSILE_L_SHIFT_PER_SHAPE			6		// 6 left shifts multiplies by 64

#define POINTS_PER_HUMAN					100		// points scored for each human invader eliminated


/*****************************************************************************/
/*                           App-wide color choices                          */
/*****************************************************************************/

#define APP_FOREGROUND_COLOR		COLOR_BRIGHT_BLUE
#define APP_BACKGROUND_COLOR		COLOR_BLACK
#define APP_ACCENT_COLOR			COLOR_BLUE



/*****************************************************************************/
/*                                   Command Keys                            */
/*****************************************************************************/


// key codes for user input
#define ACTION_INVALID_INPUT		255	// this will represent illegal keyboard command by user

#define ACTION_CANCEL				CH_ESC
#define ACTION_CANCEL_ALT			CH_RUNSTOP
#define ACTION_CONFIRM				CH_ENTER

// navigation keys
#define MOVE_UP						'w'
#define MOVE_RIGHT					'd'
#define MOVE_DOWN					'x'
#define MOVE_LEFT					'a'
#define MOVE_UP_RIGHT				'e'
#define MOVE_DOWN_RIGHT				'c'
#define MOVE_DOWN_LEFT				'z'
#define MOVE_UP_LEFT				'q'
// joystick 0
#define JOY_UP_BIT					0b00000001	// 1
#define JOY_DOWN_BIT				0b00000010  // 2
#define JOY_LEFT_BIT				0b00000100  // 4
#define JOY_RIGHT_BIT				0b00001000  // 8
#define JOY_FIRE1_BIT				0b00010000  // 16
#define JOY_FIRE2_BIT				0b00100000  // 32

#define ACTION_FIRE					CH_SPACE
#define ACTION_WARP					'/'
#define ACTION_BOMB					'b'

#define ACTION_CYCLE_WEAPON			']'
				

/*****************************************************************************/
/*                                 Error Codes                               */
/*****************************************************************************/

#define ERROR_NO_ERROR													0
#define ERROR_NO_FILES_IN_FILE_LIST										101
#define ERROR_PANEL_WAS_NULL											102
#define ERROR_PANEL_ROOT_FOLDER_WAS_NULL								103
#define ERROR_PANEL_TARGET_FOLDER_WAS_NULL								104
#define ERROR_FOLDER_WAS_NULL											105
#define ERROR_FILE_WAS_NULL												106
#define ERROR_COULD_NOT_OPEN_DIR										107
#define ERROR_COULD_NOT_CREATE_NEW_FILE_OBJECT							108
#define ERROR_FOLDER_FILE_WAS_NULL										109
#define ERROR_NO_CONNECTED_DRIVES_FOUND									110
#define ERROR_FILE_TO_DESTROY_WAS_NULL									111
#define ERROR_DESTROY_ALL_FOLDER_WAS_NULL								112
#define ERROR_FILE_DUPLICATE_FAILED										113
#define ERROR_FOLDER_TO_DESTROY_WAS_NULL								114
#define ERROR_SET_CURR_ROW_FOLDER_WAS_NULL								115
#define ERROR_GET_CURR_ROW_FOLDER_WAS_NULL								116
#define ERROR_SET_FILE_SEL_BY_ROW_PANEL_WAS_NULL						117
#define ERROR_FILE_MARK_SELECTED_FILE_WAS_NULL							118
#define ERROR_FILE_MARK_UNSELECTED_FILE_WAS_NULL						119
#define ERROR_PANEL_INIT_FOLDER_WAS_NULL								120
#define ERROR_COPY_FILE_SOURCE_FOLDER_WAS_NULL							121
#define ERROR_COPY_FILE_TARGET_FOLDER_WAS_NULL							122
#define ERROR_POPULATE_FILES_FOLDER_WAS_NULL							123
#define ERROR_COULD_NOT_CREATE_ROOT_FOLDER_FILE							124
#define ERROR_COULD_NOT_CREATE_ROOT_FOLDER_OBJ							125
#define ERROR_MEMSYS_GET_CURR_ROW_FOLDER_WAS_NULL						126
#define ERROR_BANK_MARK_SELECTED_BANK_WAS_NULL							127
#define ERROR_BANK_MARK_UNSELECTED_BANK_WAS_NULL						128
#define ERROR_BANK_TO_DESTROY_WAS_NULL									129
#define ERROR_PANEL_INIT_MEMSYS_WAS_NULL								130
#define ERROR_DESTROY_ALL_MEMSYS_WAS_NULL								131
#define ERROR_COULD_NOT_CREATE_OR_RESET_MEMSYS_OBJ						132

#define ERROR_DEFINE_ME													255



/*****************************************************************************/
/*                                  Overlays                                 */
/*****************************************************************************/

#define OVERLAY_CPU_BANK					0x05	// overlays are always loaded into bank 5
#define OVERLAY_START_ADDR					0xA000	// in CPU memory space, the start of overlay memory

// overlays defs are just the physical bank num the overlay code is stored in
#define OVERLAY_SCREEN			0x08
#define OVERLAY_STARTUP			0x09
//#define OVERLAY_3				0x0A
//#define OVERLAY_4			0x0B
//#define OVERLAY_5		0x0C
//#define OVERLAY_6					0x0D
//#define OVERLAY_7					0x0E
//#define OVERLAY_8					0x0F
//#define OVERLAY_9					0x10
//#define OVERLAY_10					0x11

#define CUSTOM_FONT_PHYS_ADDR              0x3A000	// temporary buffer for loading in a font?
#define CUSTOM_FONT_SLOT                   0x05
#define CUSTOM_FONT_VALUE                  0x1D

/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// get random number between 1 and the_range + 1
// must have seeded the number generator first with call to _randomize() --> cc65 function
// if passed 0, returns 0.
uint16_t App_GetRandom(uint16_t the_range);

// handle game over scenario: say you are dead, stop game, show hi scores, etc, etc.
void App_GameOver(void);

// display error message, wait for user to confirm, and exit
void App_Exit(uint8_t the_error_number);

// Brings the requested overlay into memory
void App_LoadOverlay(uint8_t the_overlay_em_bank_number);


#endif /* FILE_MANAGER_H_ */
