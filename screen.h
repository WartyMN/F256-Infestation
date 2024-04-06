/*
 * screen.h
 *
 *  Created on: Jan 11, 2023
 *      Author: micahbly
 */

#ifndef SCREEN_H_
#define SCREEN_H_

/* about this class
 *
 *** things a screen needs to be able to do
 *
 * draw the file manager screen
 * draw individual buttons
 * update visual state of individual buttons
 *
 *** things a screen has
 *
 *
 */

/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

//#include <string.h>

#include "app.h"
#include "text.h"
#include <stdint.h>


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/


#define CH_INVERSE_SPACE				7		// useful for progress bars as a slot fully used up
#define CH_CHECKERBOARD					199		// useful for progress bars as a slot fully used up
#define CH_UNDERSCORE					148		// this is one line up from a pure underscore, but works if text right under it. 0x5f	// '_'
#define CH_OVERSCORE					0x0e	// opposite of '_'


/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/

/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/



/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// set up screen variables and draw screen for first time
void Screen_Render(void);

// display information about f/manager
void Screen_ShowAppAboutInfo(void);


#endif /* SCREEN_H_ */
