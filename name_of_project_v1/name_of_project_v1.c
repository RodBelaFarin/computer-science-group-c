#include "easybutterfly.h" // ACUASeasy library for Butterfly

void WAIT_FOR_RELEASE_JOYSTICK_ALL() {

  WAIT_FOR_RELEASE_JOYSTICK_RIGHT
  WAIT_FOR_RELEASE_JOYSTICK_LEFT
  WAIT_FOR_RELEASE_JOYSTICK_UP
  WAIT_FOR_RELEASE_JOYSTICK_DOWN
  WAIT_FOR_RELEASE_JOYSTICK_CENTER
  return;

}


PROGRAM_INIT
	ACTIVATE_LCD // initialize the LCD. The output begins at the first position of the display.
	CLEAR_LCD // Clears the LCD screen.

	enum states { display_time, state_time, 		//enumeration of all states
				  display_dev1, state_dev1,
				  display_dev2,	state_dev2,
				  display_set_time, state_set_time,
				  state_time_center_pressed,
				  state_d1_center_pressed,
				  state_d2_center_pressed,
				  state_start_timer1,
				  state_start_timer2,
				  display_leave_menu, state_leave_menu,
				  display_set_timer1, state_set_timer1,
				  display_set_timer2, state_set_timer2,
				  display_set_sound, state_set_sound,
				  display_set_time_input, state_set_time_input,
				  get_sound_on_off, state_sound_on, state_sound_off,
				  display_set_timer1_on_off, state_set_timer1_on_off,
				  display_set_timer2_on_off, state_set_timer2_on_off,
				  get_timer1_on_off, state_timer1_on, state_timer1_off,
				  get_timer2_on_off, state_timer2_on, state_timer2_off,
				  display_set_timer1_countdown, state_set_timer1_countdown,
				  display_set_timer2_countdown, state_set_timer2_countdown,
				  display_timer1_input, state_timer1_input,
				  display_timer2_input, state_timer2_input
				};
	enum states c = display_time;

PROGRAM_START

	switch (c)
	{

	// DISPLAY TIME AND WAIT FOR EVENT
	case display_time:
	    //save actual_time
		 //if (sound_is_on) play_sound;
		LCD_TEXT("  0000")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_time;
		break;
	case state_time:
		ON_JOYSTICK_LEFT   c = display_dev1;
		ON_JOYSTICK_RIGHT  c = display_dev2;
		ON_JOYSTICK_CENTER c = state_time_center_pressed;
		//if (now - actual_time > 1min) c = display_time;
		break;

    // DISPLAY DEVICE1 TIMER AND WAIT FOR EVENT
	case display_dev1:
		 //if (sound_is_on) play_sound;
		LCD_TEXT("1 0010")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_dev1;
		break;
	case state_dev1:
		ON_JOYSTICK_LEFT   c = display_dev2;
		ON_JOYSTICK_RIGHT  c = display_time;
		ON_JOYSTICK_UP     {} //function turning on device 1
		ON_JOYSTICK_DOWN   {} //function turning off device 1
		ON_JOYSTICK_CENTER {} //start timer
		//if (now - start_timer > 1min) c = display_dev1;
		break;

    // DISPLAY DEVICE2 TIMER AND WAIT FOR EVENT
	case display_dev2:
		//if (sound_is_on) play_sound;
		LCD_TEXT("2 0010")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_dev2;		
		break;
	case state_dev2:
		ON_JOYSTICK_LEFT   c = display_time;
		ON_JOYSTICK_RIGHT  c = display_dev1;
		ON_JOYSTICK_UP     {} //function turning on device 2
		ON_JOYSTICK_DOWN   {} //function turning off device 2
		ON_JOYSTICK_CENTER {} //start timer
		//if (now - start_timer > 1min) c = display_dev2;
		break;

	// WAIT 5sec to get to set_time
	case state_time_center_pressed:
		//t0 = now;
		c = display_time;
		while (!(PINB & (1 << 4))) {};//while JOYSTICK_CENTER is pressed
		//{ if (now - t0 >= 5sec) c = display_set_time; }		
		break;

	// WAIT 5sec to get to settings_timer1 else start_timer1
	case state_d1_center_pressed:
		//t0 = now;
		c = state_start_timer1;
		while (!(PINB & (1 << 4))) {};//while JOYSTICK_CENTER is pressed
		//{ if (now - t0 >= 5sec) c = display_set_timer1; }		
		break;

	// WAIT 5sec to get to settings_timer2 else start_timer2
	case state_d2_center_pressed:
		//t0 = now;
		c = state_start_timer2;
		while (!(PINB & (1 << 4))) {};//while JOYSTICK_CENTER is pressed
		//{ if (now - t0 >= 5sec) c = display_set_timer2; }		
		break;

	// DISPLAY SET TIME AND WAIT FOR EVENT
	case display_set_time:
		//if (sound_is_on) play_sound;
		LCD_TEXT("SET T")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_set_time;
		break;
	case state_set_time:
		ON_JOYSTICK_LEFT   c = display_leave_menu;
		ON_JOYSTICK_RIGHT  c = display_set_sound;
		ON_JOYSTICK_DOWN   c = display_set_time_input;
		ON_JOYSTICK_CENTER c = display_set_time_input;
		break;

	// DISPLAY LEAVE MENU AND WAIT FOR EVENT
	case display_leave_menu:
		//if (sound_is_on) play_sound;
		LCD_TEXT("BACK")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_leave_menu;
		break;
	case state_leave_menu:
		ON_JOYSTICK_LEFT   c = display_set_timer1;
		ON_JOYSTICK_RIGHT  c = display_set_time;
		ON_JOYSTICK_UP     c = display_time;
		ON_JOYSTICK_DOWN   c = display_time;
		ON_JOYSTICK_CENTER c = display_time;
		break;

	// DISPLAY SET SOUND AND WAIT FOR EVENT
	case display_set_sound:
		//if (sound_is_on) play_sound;
		LCD_TEXT("SOUND")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_set_sound;
		break;
	case state_set_sound:
		ON_JOYSTICK_LEFT   c = display_set_time;
		ON_JOYSTICK_RIGHT  c = display_set_timer2;
		ON_JOYSTICK_UP     c = get_sound_on_off;
		ON_JOYSTICK_DOWN   c = get_sound_on_off;
		ON_JOYSTICK_CENTER c = get_sound_on_off;
		break;

	// ASK IF SOUND IS ACTUAL ON OR OFF --> GO IN THE STATE
	case get_sound_on_off:
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		//if ( sound_is_on ) c = state_sound_on; //function get_sound_settings
		//else c = state_sound_off;
		break;
	case state_sound_on:
		//if (sound_is_on) play_sound;
		ON_JOYSTICK_LEFT   { c = state_sound_off;   } //function set_sound_settings (SET OFF!!!!) 
		ON_JOYSTICK_RIGHT  { c = state_sound_off;   } //function set_sound_settings (SET OFF!!!!)
		ON_JOYSTICK_UP     { c = display_set_sound; } //function set_sound_settings
		ON_JOYSTICK_DOWN   { c = display_set_sound; } //function set_sound_settings
		ON_JOYSTICK_CENTER { c = display_set_sound; } //function set_sound_settings
		break;
	case state_sound_off:
	    //if (sound_is_on) play_sound;
		ON_JOYSTICK_LEFT   { c = state_sound_on;    } //function set_sound_settings (SET ON!!!!) 
		ON_JOYSTICK_RIGHT  { c = state_sound_on;    } //function set_sound_settings (SET ON!!!!)
		ON_JOYSTICK_UP     { c = display_set_sound; } //function set_sound_settings
		ON_JOYSTICK_DOWN   { c = display_set_sound; } //function set_sound_settings
		ON_JOYSTICK_CENTER { c = display_set_sound; } //function set_sound_settings
		break;

	// DISPLAY SET TIMER1 AND WAIT FOR EVENT
	case display_set_timer1:
		//if (sound_is_on) play_sound;
		LCD_TEXT("SET T1")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_set_timer1;
		break;
	case state_set_timer1:
		ON_JOYSTICK_LEFT   c = display_set_timer2;
		ON_JOYSTICK_RIGHT  c = display_leave_menu;
		ON_JOYSTICK_UP     c = display_set_timer1_on_off;
		ON_JOYSTICK_DOWN   c = display_set_timer1_on_off;
		ON_JOYSTICK_CENTER c = display_set_timer1_on_off;
		break;

	// DISPLAY SET TIMER2 AND WAIT FOR EVENT
	case display_set_timer2:
		//if (sound_is_on) play_sound;
		LCD_TEXT("SET T2")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_set_timer2;
		break;
	case state_set_timer2:
		ON_JOYSTICK_LEFT   c = display_set_sound;
		ON_JOYSTICK_RIGHT  c = display_set_timer1;
		ON_JOYSTICK_UP     c = display_set_timer2_on_off;
		ON_JOYSTICK_DOWN   c = display_set_timer2_on_off;
		ON_JOYSTICK_CENTER c = display_set_timer2_on_off;
		break;
	
	// DISPLAY SET TIMER1 ON/OFF AND WAIT FOR EVENT
	case display_set_timer1_on_off:
		//if (sound_is_on) play_sound;
		LCD_TEXT("ON/OFF")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_set_timer1_on_off;
		break;
	case state_set_timer1_on_off:
		ON_JOYSTICK_LEFT   c = display_set_timer1_countdown;
		ON_JOYSTICK_RIGHT  c = display_set_timer1_countdown;
		ON_JOYSTICK_UP     c = display_set_timer1;
		ON_JOYSTICK_DOWN   c = get_timer1_on_off;
		ON_JOYSTICK_CENTER c = get_timer1_on_off;
		break;

	// ASK IF TIMER1 IS ACTUAL ON OR OFF --> GO IN THE STATE
	case get_timer1_on_off:
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		//if ( timer1_is_on ) c = state_timer1_on; //function get_timer1_on_off_settings
		//else c = state_timer1_off;
		break;
	case state_timer1_on:
		//if (sound_is_on) play_sound;
		ON_JOYSTICK_LEFT   c = state_timer1_off;
		ON_JOYSTICK_RIGHT  c = state_timer1_off;
		ON_JOYSTICK_UP     { c = display_set_timer1_on_off; } //function set_timer1_on_off_settings
		ON_JOYSTICK_DOWN   { c = display_set_timer1_on_off; } //function set_timer1_on_off_settings
		ON_JOYSTICK_CENTER { c = display_set_timer1_on_off; } //function set_timer1_on_off_settings
		break;
	case state_timer1_off:
		//if (sound_is_on) play_sound;
		ON_JOYSTICK_LEFT   c = state_timer1_on;
		ON_JOYSTICK_RIGHT  c = state_timer1_on;
		ON_JOYSTICK_UP     { c = display_set_timer1_on_off; } //function set_timer1_on_off_settings
		ON_JOYSTICK_DOWN   { c = display_set_timer1_on_off; } //function set_timer1_on_off_settings
		ON_JOYSTICK_CENTER { c = display_set_timer1_on_off; } //function set_timer1_on_off_settings
		break;

	// DISPLAY SET TIMER2 ON/OFF AND WAIT FOR EVENT
	case display_set_timer2_on_off:
		//if (sound_is_on) play_sound;
		LCD_TEXT("ON/OFF")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_set_timer2_on_off;
		break;
	case state_set_timer2_on_off:
		ON_JOYSTICK_LEFT   c = display_set_timer2_countdown;
		ON_JOYSTICK_RIGHT  c = display_set_timer2_countdown;
		ON_JOYSTICK_UP     c = display_set_timer2;
		ON_JOYSTICK_DOWN   c = get_timer2_on_off;
		ON_JOYSTICK_CENTER c = get_timer2_on_off;
		break;

	// ASK IF TIMER2 IS ACTUAL ON OR OFF --> GO IN THE STATE
	case get_timer2_on_off:
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		//if ( timer2_is_on ) c = state_timer2_on; //function get_timer2_on_off_settings
		//else c = state_timer2_off;
		break;
	case state_timer2_on:
		//if (sound_is_on) play_sound;
		ON_JOYSTICK_LEFT   c = state_timer2_off;
		ON_JOYSTICK_RIGHT  c = state_timer2_off;
		ON_JOYSTICK_UP     { c = display_set_timer2_on_off; } //function set_timer2_on_off_settings
		ON_JOYSTICK_DOWN   { c = display_set_timer2_on_off; } //function set_timer2_on_off_settings
		ON_JOYSTICK_CENTER { c = display_set_timer2_on_off; } //function set_timer2_on_off_settings
		break;
	case state_timer2_off:
		//if (sound_is_on) play_sound;
		ON_JOYSTICK_LEFT   c = state_timer2_on;
		ON_JOYSTICK_RIGHT  c = state_timer2_on;
		ON_JOYSTICK_UP     { c = display_set_timer2_on_off; } //function set_timer2_on_off_settings
		ON_JOYSTICK_DOWN   { c = display_set_timer2_on_off; } //function set_timer2_on_off_settings
		ON_JOYSTICK_CENTER { c = display_set_timer2_on_off; } //function set_timer2_on_off_settings
		break;

	// DISPLAY SET TIMER1 COUNTDOWN AND WAIT FOR EVENT
	case display_set_timer1_countdown:
		//if (sound_is_on) play_sound;
		LCD_TEXT("CNTDW1")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_set_timer1_countdown;
		break;
	case state_set_timer1_countdown:
		ON_JOYSTICK_LEFT   c = display_set_timer1_on_off;
		ON_JOYSTICK_RIGHT  c = display_set_timer1_on_off;
		ON_JOYSTICK_UP     c = display_set_timer1;
		ON_JOYSTICK_DOWN   c = display_timer1_input;
		ON_JOYSTICK_CENTER c = display_timer1_input;
		break;
	case display_timer1_input:
		//if (sound_is_on) play_sound;
		LCD_TEXT("0000")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_timer1_input;
		break;
	case state_timer1_input:
		ON_JOYSTICK_UP     c = display_set_timer1_countdown;
		ON_JOYSTICK_DOWN   c = display_set_timer1_countdown;
		ON_JOYSTICK_CENTER c = display_set_timer1_countdown;
		break;

	// DISPLAY SET TIMER2 COUNTDOWN AND WAIT FOR EVENT
	case display_set_timer2_countdown:
		//if (sound_is_on) play_sound;
		LCD_TEXT("CNTDW2")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_set_timer1_countdown;
		break;
	case state_set_timer2_countdown:
		ON_JOYSTICK_LEFT   c = display_set_timer2_on_off;
		ON_JOYSTICK_RIGHT  c = display_set_timer2_on_off;
		ON_JOYSTICK_UP     c = display_set_timer2;
		ON_JOYSTICK_DOWN   c = display_timer2_input;
		ON_JOYSTICK_CENTER c = display_timer2_input;
		break;
	case display_timer2_input:
		//if (sound_is_on) play_sound;
		LCD_TEXT("0000")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_timer2_input;
		break;
	case state_timer2_input:
		ON_JOYSTICK_UP     c = display_set_timer2_countdown;
		ON_JOYSTICK_DOWN   c = display_set_timer2_countdown;
		ON_JOYSTICK_CENTER c = display_set_timer2_countdown;
		break;


	// If we are somehow in an undefined state go back to time again
    default:
		c = display_time;
		break;
	}


PROGRAM_END
