/*
 * general.c
 *
 *  Created on: Feb 19, 2022
 *      Author: micahbly
 *
 *  - adapted from my Amiga WB2K project's general.c
 */

// This is a cut-down, semi-API-compatible version of the OS/f general.c file from Lich King (Foenix)
// adapted for Foenix F256 Jr starting November 29, 2022


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "strings.h"
#include "app.h"
#include "memory.h"
#include "sys.h"
#include "text.h"

// C includes
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// F256 includes
#include "f256.h"


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/



/*****************************************************************************/
/*                          File-scoped Variables                            */
/*****************************************************************************/


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

char*					global_string[NUM_STRINGS] = 
{
	"Hello",
	"goodbye"
};




/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/



/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/






/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

// retrieves a string from extended memory and stashes in the first page of STORAGE_DOS_BOOT_BUFFER for the calling program to retrieve
// returns a pointer to the string (pointer will always be to STORAGE_DOS_BOOT_BUFFER)
char* General_GetString(uint8_t the_string_id)
{
	return global_string[the_string_id];
}
