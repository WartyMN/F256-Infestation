/*
 * keyboard.c
 *
 *  Created on: Aug 23, 2022
 *      Author: micahbly
 *
 */

// adapted for (Lich King) Foenix F256 Jr starting November 30, 2022
// adapted for f/manager Foenix F256 starting March 10, 2024




/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "keyboard.h"
#include "kernel.h"
#include "f256.h"
// #include "comm_buffer.h"	// just need for debugging
#include "general.h"
#include "memory.h"

// C includes
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// F256 Kernel includes
#include "api.h"


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/

#define MINUTE_TIMER_COOKIE		127		// hard-coded. just don't want it to start with 0, as that's what the keyboard cookie will start with

#define KEYBOARD_QUEUE_SIZE		8

#define VECTOR(member) (size_t) (&((struct call*) 0xff00)->member)
#define EVENT(member)  (size_t) (&((struct events*) 0)->member)
#define CALL(fn) (unsigned char) ( \
                   asm("jsr %w", VECTOR(fn)), \
                   asm("stz %v", error), \
                   asm("ror %v", error), \
                   __A__)


/*****************************************************************************/
/*                          File-scope Variables                             */
/*****************************************************************************/

static uint8_t			keyboard_queue_entries;
static uint8_t			keyboard_queue[KEYBOARD_QUEUE_SIZE];
static KeyRepeater		keyboard_repeater;

/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

// extern char* 			global_string_buffer;	// just need for debugging

//uint8_t			global_joy_state;
extern uint8_t				zp_joy;

extern struct call_args args; // in gadget's version of f256 lib, this is allocated and initialized with &args in crt0. 
extern struct event_t event; // in gadget's version of f256 lib, this is allocated and initialized with &event in crt0. 
extern char error;


#pragma zpsym ("event");
#pragma zpsym ("error");
#pragma zpsym ("zp_joy");
// #pragma zpsym ("global_string_buffer");


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

// Pop head of keyboard queue
uint8_t Keyboard_PopQueue(void);

// Calls kernel.nextEvent but also updates keyboard state events.
uint8_t Keyboard_GetNextEvent(void);

// Process a joystick event (ZX KM L)
// if called, we already know it is EVENT(JOYSTICK)
// data is in event.joystick.joy0 or event.joystick.joy1
uint8_t Keyboard_ProcessJoyEvent(void);

// Process a key PRESSED/RELEASED, updating key status bit array
uint8_t Keyboard_ProcessKeyEvent(void);

// passed a keycode, it starts a timer tracking if that key is held down
void Keyboard_StartTimerForKey(uint8_t the_key);

// schedule a repeat event
// pass the frame count of requested next event
void Keyboard_ScheduleRepeatEvent(uint8_t next_frame_count);

// add a character to the key buffer, if space is available
void Keyboard_AddToQueue(uint8_t the_char);

// // schedule a repeat event for the minute clock
// void Keyboard_ScheduleMinuteHandRepeatEvent(void);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

// Pop head of keyboard queue
uint8_t Keyboard_PopQueue(void)
{
	uint8_t		this_key;
	uint8_t		i;

	if (keyboard_queue_entries == 0)
	{
		return 0;
	}
	
	this_key = keyboard_queue[0];
	
	for (i = 0; i < (KEYBOARD_QUEUE_SIZE-1); i++)
	{
		keyboard_queue[i] = keyboard_queue[i+1];
	}
	
	--keyboard_queue_entries;
	
	return this_key;
}


// Calls kernel.nextEvent but also updates keyboard state events.
uint8_t Keyboard_GetNextEvent(void)
{
	CALL(NextEvent);

	if (error)
	{
		asm("jsr %w", VECTOR(Yield));
		return 0;
	}

	if (event.type == EVENT(timer.EXPIRED))
	{
		return 255;
	}
	else if (event.type != EVENT(key.PRESSED) && event.type != EVENT(key.RELEASED) && event.type != EVENT(JOYSTICK))
	{
		return 0;
	}

	// We have a keyboard event. (which includes possibility of joystick event, on F256)
	//Keyboard_ProcessKeyEvent();
	//this_status = Keyboard_UpdateKeyboardJoystick();
	
	return 1;
}



// Process a joystick event
// if called, we already know it is EVENT(JOYSTICK)
// data is in event.joystick.joy0 or event.joystick.joy1
uint8_t Keyboard_ProcessJoyEvent(void)
{
	uint8_t				this_value = 0;

	if (event.joystick.joy0)
	{
		// First joystick
		this_value = event.joystick.joy0;
	}
	
	if (event.joystick.joy1)
	{
		// Second joystick
		this_value = event.joystick.joy1;
	}

	if (this_value != *(uint8_t*)ZP_JOY)
	{
		// something changed. user pressed fire, released fire, pushed left, unpushed left, etc. 
		*(uint8_t*)ZP_JOY = this_value;
		Keyboard_AddToQueue(JOYSTICK_EVENT_OCCURRED);
	
		return this_value;		
	}
	
	return 0;
}


// Process a key PRESSED/RELEASED, updating key status bit array
uint8_t Keyboard_ProcessKeyEvent(void)
{
	bool		add_char_to_queue = true;
	uint8_t		this_char;

	if (event.type == EVENT(key.PRESSED))
	{
			if (event.key.flags)
			{
				// if a function key, return raw code.
				if (event.key.raw >= CH_F1 && event.key.raw <= CH_F12)
				{
					this_char = event.key.raw;
				}
				else
				{
					// if it wasn't an F key, it was a meta key, we just ignore it.
					add_char_to_queue = false;
				}
			}
			else
			{
				this_char = event.key.ascii;
			}
			
			if (add_char_to_queue)
			{
				// Schedule repeats for keys from CBM/K keyboards
				if (event.key.keyboard == 0)
				{
					// for f/manager, we only want repeat on cursor keys, delete, backspace, etc.
					if (this_char == CH_CURS_UP || this_char == CH_CURS_DOWN || this_char == CH_CURS_LEFT || this_char == CH_CURS_RIGHT || this_char == CH_DEL || this_char == CH_RUNSTOP)
					Keyboard_StartTimerForKey(this_char);
				}

				// for any keyboard type, add this char to the key buffer
				Keyboard_AddToQueue(this_char);				
			}
	}
	else
	{
		// jmp     StopRepeat WHICH IS "inc     repeat.cookie -> rts"
		keyboard_repeater.cookie++;

		// prevent collision with the permanent minute hand cookie		
		if (keyboard_repeater.cookie == MINUTE_TIMER_COOKIE)
		{
			keyboard_repeater.cookie++;
		}
	}

	return this_char;
}


// passed a keycode, it starts a timer tracking if that key is held down
void Keyboard_StartTimerForKey(uint8_t the_key)
{
	uint8_t		current_timer_value;
	
	keyboard_repeater.key = the_key;
	keyboard_repeater.cookie++;			// set a new ID
		
	// prevent collision with the permanent minute hand cookie
	if (keyboard_repeater.cookie == MINUTE_TIMER_COOKIE)
	{
		keyboard_repeater.cookie++;
	}
	
	// Get the current frame counter
	// including query makes the SetTimer call return the value of the current timer (in A)
	args.timer.units = (TIMER_FRAMES | TIMER_QUERY);
	current_timer_value = CALL(Clock.SetTimer);
// 	asm("sta $02");	// this should be available
// 	current_timer_value = R8(0x02);

	// Schedule a timer approx 0.75s in the future (repeat delay).
	Keyboard_ScheduleRepeatEvent(current_timer_value + 15);
}


// schedule a repeat event
// pass the frame count of requested next event
void Keyboard_ScheduleRepeatEvent(uint8_t next_frame_count)
{
	args.timer.absolute = next_frame_count;	
	args.timer.units = TIMER_FRAMES;
	args.timer.cookie = keyboard_repeater.cookie;
	
	CALL(Clock.SetTimer);
}


// // schedule a repeat event for the minute clock
// void Keyboard_ScheduleMinuteHandRepeatEvent(void)
// {
// 	args.timer.absolute = 60;	
// 	args.timer.units = TIMER_SECONDS;
// 	args.timer.cookie = MINUTE_TIMER_COOKIE;
// 	
// 	CALL(Clock.SetTimer);
// }


// // initiate the minute hand timer
// void Keyboard_InitiateMinuteHand(void)
// {
// 	Keyboard_ScheduleMinuteHandRepeatEvent();
// }


// returns 0 if it determined there was no repeat (yet). returns a key code if event resulted in a repeat.
uint8_t Keyboard_HandleRepeatTimerEvent(void)
{
// 	// before checking for keyboard repeats, check if this is our minute-timer cookie
// 	if (event.timer.cookie == MINUTE_TIMER_COOKIE)
// 	{
// 		App_DisplayTime();
// 		Keyboard_ScheduleMinuteHandRepeatEvent();	// schedule the next one
// 		return 0;
// 	}

	// ignore retired timers
	if (event.timer.cookie != keyboard_repeater.cookie)
	{
		return 0;
	}
	
	// Schedule the next repeat for ~0.05s from now.
	Keyboard_ScheduleRepeatEvent(event.timer.value + 3);
	
	// Return the key being repeated.
	return keyboard_repeater.key;
}


// add a character to the key buffer, if space is available
void Keyboard_AddToQueue(uint8_t the_char)
{
	// check there is space in the keyboard buffer to put this char into
	if (keyboard_queue_entries == KEYBOARD_QUEUE_SIZE)
	{
		return;
	}
	
	// add to queue
	keyboard_queue[keyboard_queue_entries] = the_char;
	keyboard_queue_entries++;
}


// main event processor
void Keyboard_ProcessEvents(void)
{
	uint8_t		repeated_char;
// 	bool		add_char_to_queue = true;
	
	while(1)
	{
		if (Keyboard_GetNextEvent() == 0)
		{
			return;
		}
		
		if (event.type == EVENT(timer.EXPIRED))
		{
			if ((repeated_char = Keyboard_HandleRepeatTimerEvent()) != 0)
			{
				Keyboard_AddToQueue(repeated_char);
			}		
		}
		else if (event.type == EVENT(key.RELEASED) || event.type == EVENT(key.PRESSED))
		{
			Keyboard_ProcessKeyEvent();
		}
		else if (event.type == EVENT(JOYSTICK))
		{
			Keyboard_ProcessJoyEvent();
		}
	}	
}


// // Update the keyboard-joystick byte (ZX KM L)
// uint8_t Keyboard_UpdateKeyboardJoystick(void)
// {
// 	static uint8_t		kj_key[5] = {'L','X','Z','M','K'};
// 	uint8_t				i;
// 	uint8_t				this_value;
// 	uint8_t				this_status;
// 	
// 	keyboard_joystick = 0;
// 	
// 	for (i = 0; i < 5; i++)
// 	{
// 		this_value = kj_key[i];
// 		this_value &= 0x1f;
// 		this_status = keyboard_status[this_value];
// 		this_status &= 0x10; // letters always bit 4 (actually ASCII of L/C)
// 		this_status += 0xff;	//I don't understand what point of this is. in asm, this modified A, maybe calling funciton then returned that?
// 		keyboard_joystick = keyboard_joystick << 1; // this is not actually a ROL, this is ASL. so MSB not getting copied to LSB. not sure how to fix. but since I don't know what this fucntion does, or care about joysticks right now, I don't intend to do anything.
// 	}
// 	
// 	return this_status;
// }

// UpdateKeyboardJoystick:
// 		stz 	KeyJoystick
// 		ldx 	#0
// _UKJLoop:
// 		lda 	_UKJKeys,x 					; which key
// 		and 	#$1F
// 		tay
// 		lda 	KeyStatus,y 				; get status
// 		and 	#$10 						; letters always bit 4 (actually ASCII of L/C)
// 		clc  								; set C if bit set
// 		adc 	#$FF
// 		rol 	KeyJoystick 				; shift into place
// 		inx
// 		cpx 	#5 							; do all 5
// 		bne 	_UKJLoop
// 		rts
// ;	
// ;		This mapping may change if raw changes ?
// ;
// _UKJKeys:
// 		.byte	'L','X','Z','M','K'		



/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

// **** USER INPUT UTILITIES *****




// Wait for one character from the keyboard and return it
char Keyboard_GetChar(void)
{
	uint8_t		the_char = 0;
	
	//DEBUG_OUT(("%s %d: entered", __func__, __LINE__));

	Keyboard_ProcessEvents();

	do
	{
		the_char = Keyboard_GetKeyIfPressed();
	} while (the_char == 0);

	//DEBUG_OUT(("%s %d: key input=%x", __func__, __LINE__, the_char));
	
	return the_char;
}


// Check to see if keystroke events pending - does not wait for a key
uint8_t Keyboard_GetKeyIfPressed(void)
{
	// if there is anything in the queue, pop it and return it.
	if (keyboard_queue_entries > 0)
	{
		return Keyboard_PopQueue();
	}
		
	// process any outstanding events
	Keyboard_ProcessEvents();
	
	return 0;
}
