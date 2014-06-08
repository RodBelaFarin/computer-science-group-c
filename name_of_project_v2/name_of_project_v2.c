#include "easybutterfly.h" // ACUASeasy library for Butterfly

#define DEBUG_MODE 0

void run_timer(int *s, int *m, int *h) {

	unsigned char ElapsedSeconds = 0;
	unsigned char ElapsedMinutes = 0;
	unsigned char ElapsedHours = 0;
 	while ( TCNT1 >= 49999 )
	{ 
		// Check timer value in if statement, true when count matches 1 second 
		TCNT1 = TCNT1 - 49999; // Reset timer value 
		ElapsedSeconds++; 

		if (ElapsedSeconds == 60) // Check if one minute has elapsed 
		{ 
			ElapsedMinutes++;
			ElapsedSeconds = 0; // Reset counter variable  
		}
		if (ElapsedMinutes == 60) // Check if one hour has elapsed 
		{ 
			ElapsedHours++;
			ElapsedMinutes = 0; // Reset counter variable  
		}  
		if (ElapsedHours == 24) // Check if one day has elapsed 
		{ 
			ElapsedHours = 0; // Reset counter variable  
		}
		LCD_D_NUMBER(ElapsedSeconds,1,0)
		*s = ElapsedSeconds;
		*m = ElapsedMinutes;
		*h = ElapsedHours;  
	} 
	return;
} 


void WAIT_FOR_RELEASE_JOYSTICK_ALL() {

  WAIT_FOR_RELEASE_JOYSTICK_RIGHT
  WAIT_FOR_RELEASE_JOYSTICK_LEFT
  WAIT_FOR_RELEASE_JOYSTICK_UP
  WAIT_FOR_RELEASE_JOYSTICK_DOWN
  WAIT_FOR_RELEASE_JOYSTICK_CENTER
  return;
}

void set_volume(int a, int *volume) {
  *volume = *volume + a;
  if (*volume >= 100) *volume = 100;
  else if (*volume <= 0) *volume = 0;
  BEEP_VOLUME(*volume)
  BEEP(246.942, 0.2) //beep with frequency 246.942 --> tone h for 0.25sec
  LCD_D_NUMBER(*volume,3,0)	
  int j = 0;
  while ( (!(PINE & (1 << 2)) & (*volume > 0)) | (!(PINE & (1 << 3)) & (*volume < 100)) ) {
  	for (int i = 0; i<500; i++) ;
	j = j + 1;
	if (j > 60) {
		*volume = *volume + a;
		if (*volume >= 100) *volume = 100;
		else if (*volume <= 0) *volume = 0;
		BEEP_VOLUME(*volume)
		BEEP(246.942, 0.2) //beep with frequency 246.942 --> tone h for 0.25sec
		LCD_D_NUMBER(*volume,3,0)
		j = 55;
	}	
  }
  j = 0;
  return;
}

PROGRAM_INIT
	ACTIVATE_LCD // initialize the LCD. The output begins at the first position of the display.
	CLEAR_LCD // Clears the LCD screen.
	TCCR1B = ((1 << CS10)); // Set up timer at Fcpu/256 
		
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
				  display_sound_volume, state_sound_volume,
				  get_sound_volume,
				  display_set_volume, state_set_volume,
				  display_set_timer1_on_off, state_set_timer1_on_off,
				  display_set_timer2_on_off, state_set_timer2_on_off,
				  get_timer1_on_off,
				  display_timer1_on, state_timer1_on,
				  display_timer1_off, state_timer1_off,
				  get_timer2_on_off,
				  display_timer2_on, state_timer2_on,
				  display_timer2_off, state_timer2_off,
				  display_set_timer1_countdown, state_set_timer1_countdown,
				  display_set_timer2_countdown, state_set_timer2_countdown,
				  display_timer1_input, state_timer1_input,
				  display_timer2_input, state_timer2_input
				};
	enum states c = display_time;
	int volume = 50;
	int *v;
	v = &volume;
	int seconds = 0;
	int *s;
	s = &seconds;
	int minutes = 0;
	int *m;
	m = &minutes;
	int hours = 0;
	int *h;
	h = &hours;
	BEEP_VOLUME(volume)
	float beep_frequency = 246.942; //between 20 and 20000 in Hz
	float beep_time = 0.2; //between 0.1 and 25 in s
	int actual_minute = 0;
	int actual_second = 0;

PROGRAM_START

	run_timer(s, m, h);
	switch (c)
	{

	// DISPLAY TIME AND WAIT FOR EVENT
	case display_time:

		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		#if DEBUG_MODE
			LCD_TEXT("DEBUG")
		#else
			run_timer(s, m, h);
			actual_minute = minutes;
			LCD_D_NUMBER(seconds,2,0)
		#endif
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_time;
		break;
	case state_time:
		ON_JOYSTICK_LEFT   c = display_dev1;
		ON_JOYSTICK_RIGHT  c = display_dev2;
		ON_JOYSTICK_CENTER c = state_time_center_pressed;
		run_timer(s, m, h);
		//if ((minutes - actual_minute >= 1) | ((actual_minute == 59) & (minutes == 0))) c = display_time;
		if ((seconds - actual_second >= 1) | ((actual_second == 59) & (seconds == 0))) c = display_time;
		break;

    // DISPLAY DEVICE1 TIMER AND WAIT FOR EVENT
	case display_dev1:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		LCD_TEXT("1 0010")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_dev1;
		break;
	case state_dev1:
		ON_JOYSTICK_LEFT   c = display_dev2;
		ON_JOYSTICK_RIGHT  c = display_time;
		ON_JOYSTICK_UP     {} //function turning on device 1
		ON_JOYSTICK_DOWN   {} //function turning off device 1
		ON_JOYSTICK_CENTER c = state_d1_center_pressed;
		//if (now - start_timer > 1min) c = display_dev1;
		break;

    // DISPLAY DEVICE2 TIMER AND WAIT FOR EVENT
	case display_dev2:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		LCD_TEXT("2 0005")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_dev2;		
		break;
	case state_dev2:
		ON_JOYSTICK_LEFT   c = display_time;
		ON_JOYSTICK_RIGHT  c = display_dev1;
		ON_JOYSTICK_UP     {} //function turning on device 2
		ON_JOYSTICK_DOWN   {} //function turning off device 2
		ON_JOYSTICK_CENTER c = state_d2_center_pressed;
		//if (now - start_timer > 1min) c = display_dev2;
		break;

	// WAIT 5sec to get to set_time
	case state_time_center_pressed:
		//t0 = now;
		c = display_time;
		#if DEBUG_MODE
			c = display_set_time;
		#else
			while (!(PINB & (1 << 4))) {};//while JOYSTICK_CENTER is pressed
			//{ if (now - t0 >= 5sec) c = display_set_time; }		
		#endif
		break;
	// WAIT 5sec to get to settings_timer1 else start_timer1
	case state_d1_center_pressed:
		//t0 = now;
		c = state_start_timer1;
		#if DEBUG_MODE
			c = display_set_timer1;
		#else
			while (!(PINB & (1 << 4))) {};//while JOYSTICK_CENTER is pressed
			//{ if (now - t0 >= 5sec) c = display_set_timer1; }
		#endif
		break;
	// WAIT 5sec to get to settings_timer2 else start_timer2
	case state_d2_center_pressed:
		//t0 = now;
		c = state_start_timer2;
		#if DEBUG_MODE
			c = display_set_timer2;
		#else
			while (!(PINB & (1 << 4))) {};//while JOYSTICK_CENTER is pressed
			//{ if (now - t0 >= 5sec) c = display_set_timer2; }		
		#endif		
		break;
	// DISPLAY SET TIME AND WAIT FOR EVENT
	case display_set_time:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
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
	case display_set_time_input:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		LCD_TEXT("  8888")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_set_time_input;
		break;
	case state_set_time_input:
		//wait for input
		ON_JOYSTICK_UP     c = display_set_time;
		ON_JOYSTICK_DOWN   c = display_set_time;
		ON_JOYSTICK_CENTER c = display_set_time;
		break;

	// DISPLAY LEAVE MENU AND WAIT FOR EVENT
	case display_leave_menu:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
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
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		LCD_TEXT("SOUND")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_set_sound;
		break;
	case state_set_sound:
		ON_JOYSTICK_LEFT   c = display_set_time;
		ON_JOYSTICK_RIGHT  c = display_set_timer2;
		ON_JOYSTICK_UP     c = display_sound_volume;
		ON_JOYSTICK_DOWN   c = display_sound_volume;
		ON_JOYSTICK_CENTER c = display_sound_volume;
		break;

	case display_sound_volume:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		LCD_TEXT("VOLUME")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_sound_volume;
		break;
	case state_sound_volume:
		ON_JOYSTICK_UP     c = display_set_sound;
		ON_JOYSTICK_DOWN   c = display_set_volume;
		ON_JOYSTICK_CENTER c = display_set_volume;
		break;
	case display_set_volume:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		LCD_D_NUMBER(volume,3,0)
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_set_volume;
		break;
	case state_set_volume:
		//wait for input
		ON_JOYSTICK_UP     c = display_sound_volume;
		ON_JOYSTICK_DOWN   c = display_sound_volume;
		ON_JOYSTICK_CENTER c = display_sound_volume;
		ON_JOYSTICK_LEFT   set_volume(-1, v);
		ON_JOYSTICK_RIGHT  set_volume(+1, v);
		break;

	// DISPLAY SET TIMER1 AND WAIT FOR EVENT
	case display_set_timer1:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
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
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
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
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
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
		//if ( timer1_is_on ) c = display_timer1_on; //function get_timer1_on_off_settings
		//else c = display_timer1_off;
		#if DEBUG_MODE
			c = display_timer1_on;
		#endif
		break;
	case display_timer1_on:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		LCD_TEXT("ON")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_timer1_on;
		break;
	case state_timer1_on:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		ON_JOYSTICK_LEFT   c = display_timer1_off;
		ON_JOYSTICK_RIGHT  c = display_timer1_off;
		ON_JOYSTICK_UP     { c = display_set_timer1_on_off; } //function set_timer2_on_off_settings
		ON_JOYSTICK_DOWN   { c = display_set_timer1_on_off; } //function set_timer2_on_off_settings
		ON_JOYSTICK_CENTER { c = display_set_timer1_on_off; } //function set_timer2_on_off_settings
		break;
	case display_timer1_off:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		LCD_TEXT("OFF")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_timer1_off;
		break;
	case state_timer1_off:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		ON_JOYSTICK_LEFT   c = display_timer1_on;
		ON_JOYSTICK_RIGHT  c = display_timer1_on;
		ON_JOYSTICK_UP     { c = display_set_timer1_on_off; } //function set_timer2_on_off_settings
		ON_JOYSTICK_DOWN   { c = display_set_timer1_on_off; } //function set_timer2_on_off_settings
		ON_JOYSTICK_CENTER { c = display_set_timer1_on_off; } //function set_timer2_on_off_settings
		break;

	// DISPLAY SET TIMER2 ON/OFF AND WAIT FOR EVENT
	case display_set_timer2_on_off:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
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
		//if ( timer2_is_on ) c = display_timer2_on; //function get_timer2_on_off_settings
		//else c = display_timer2_off;
		#if DEBUG_MODE
			c = display_timer2_on;
		#endif
		break;
	case display_timer2_on:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		LCD_TEXT("ON")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_timer2_on;
		break;
	case state_timer2_on:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		ON_JOYSTICK_LEFT   c = display_timer2_off;
		ON_JOYSTICK_RIGHT  c = display_timer2_off;
		ON_JOYSTICK_UP     { c = display_set_timer2_on_off; } //function set_timer2_on_off_settings
		ON_JOYSTICK_DOWN   { c = display_set_timer2_on_off; } //function set_timer2_on_off_settings
		ON_JOYSTICK_CENTER { c = display_set_timer2_on_off; } //function set_timer2_on_off_settings
		break;
	case display_timer2_off:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		LCD_TEXT("OFF")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_timer2_off;
		break;
	case state_timer2_off:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		ON_JOYSTICK_LEFT   c = display_timer2_on;
		ON_JOYSTICK_RIGHT  c = display_timer2_on;
		ON_JOYSTICK_UP     { c = display_set_timer2_on_off; } //function set_timer2_on_off_settings
		ON_JOYSTICK_DOWN   { c = display_set_timer2_on_off; } //function set_timer2_on_off_settings
		ON_JOYSTICK_CENTER { c = display_set_timer2_on_off; } //function set_timer2_on_off_settings
		break;

	// DISPLAY SET TIMER1 COUNTDOWN AND WAIT FOR EVENT
	case display_set_timer1_countdown:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
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
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		LCD_TEXT("  0010")
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
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		LCD_TEXT("CNTDW2")
		WAIT_FOR_RELEASE_JOYSTICK_ALL();
		c = state_set_timer2_countdown;
		break;
	case state_set_timer2_countdown:
		ON_JOYSTICK_LEFT   c = display_set_timer2_on_off;
		ON_JOYSTICK_RIGHT  c = display_set_timer2_on_off;
		ON_JOYSTICK_UP     c = display_set_timer2;
		ON_JOYSTICK_DOWN   c = display_timer2_input;
		ON_JOYSTICK_CENTER c = display_timer2_input;
		break;
	case display_timer2_input:
		BEEP(beep_frequency, beep_time) //beep with frequency 1046.5 --> tone c''' for 0.25sec
		LCD_TEXT("  0005")
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
