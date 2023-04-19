/*
 * app.c
 *
 *  Created on: 19 Apr 2023
 *      Author: a5126135
 */

#include "app.h"

volatile hardware_event_t hw_event_flags = NO_EVENT;

/** @brief type for handling RAMSD section*/
typedef struct
{
	char __far *rom_sectop;         /*Start address of copy source section      */
	char __far *rom_secend;         /*End address + 1 of copy source section    */
	char __near *ram_sectop;        /*Start address of copy destination section */
} dsec_t;

/* Place system data in RAMSD safe section*/
#pragma section data RAMSD

/* Display area data*/
ST7735S_display_area_info_t disp_info = {ORIENT_0, 0U, 0U, 0U, 0U};
/* variable to track system state*/
system_state_type system_state = RUN_MODE;
/* variable to track previous system state*/
system_state_type prev_system_state = RUN_MODE;
/* Array of titles for each state*/
const char * titles[] = {
		"RUN",
		"HALT",
		"STOP",
		"STOP \n - NO LOCO",
		"STOP \n - NO LOCO \n - RAMSDS=1",
		"STOP \n - FLOATING IO",
		"STOP \n - CCIO ENABLED"
};

/* Reset section*/
#pragma section

/** section info for RAMSD*/
static const dsec_t dsec_info =
{
		__sectop("RAMSD_n"),
		__secend("RAMSD_n"),
		(char __near *)__sectop("RAMSDR_n")
};

void App_wait_for_button_click(void)
{
	bool event_occured = false;
	PREPARE_CRITICAL_SECTION();

	/* Flush any button presses*/
	do
	{
		ENTER_CRITICAL_SECTION();

		event_occured = HW_EVENT_OCCURRED(hw_event_flags, BUTTON_CLICK);
		HW_CLEAR_EVENT(hw_event_flags, BUTTON_CLICK);

		EXIT_CRITICAL_SECTION();

		Hw_delay_ms(100U);
	}
	while(event_occured);

	/* Blocking wait for a button click*/
	while(!event_occured)
	{
		Hw_delay_ms(1U);

		ENTER_CRITICAL_SECTION();

		event_occured = HW_EVENT_OCCURRED(hw_event_flags, BUTTON_CLICK);
		HW_CLEAR_EVENT(hw_event_flags, BUTTON_CLICK);

		EXIT_CRITICAL_SECTION();
	}

	/* Debounce*/
	while(event_occured)
	{
		Hw_delay_ms(100U);

		ENTER_CRITICAL_SECTION();

		event_occured = HW_EVENT_OCCURRED(hw_event_flags, BUTTON_CLICK);
		HW_CLEAR_EVENT(hw_event_flags, BUTTON_CLICK);

		EXIT_CRITICAL_SECTION();
	}
}
/* END OF FUNCTION*/

void App_init(void)
{
	/* Section initialise*/
	char __far *rom_p = dsec_info.rom_sectop;
	char __near *ram_p = dsec_info.ram_sectop;
	for ( ; rom_p != dsec_info.rom_secend; rom_p++, ram_p++)
	{
		*ram_p = *rom_p;
	}

	/* Start necessary drivers*/
	R_Config_CSI00_Start_app();
	R_Config_INTC_INTP4_Start();

	/* Display initialise*/
	St7735s_init(COLOUR_WHITE);
	St7735s_get_display_area_info(&disp_info);
	Text_init(disp_info.xmax, disp_info.ymax);
	Text_set_font(&default_font);
	St7735s_wake_display();
	St7735s_display_on();
}
/* END OF FUNCTION*/

void App_update_display(void)
{
	Text_put_str(TEXT_X_POSITION, TEXT_Y_POSITION, titles[prev_system_state], COLOUR_WHITE, COLOUR_WHITE);
	Text_put_str(TEXT_X_POSITION, TEXT_Y_POSITION, titles[system_state], COLOUR_BLACK, COLOUR_WHITE);
	St7735s_refresh();
	Hw_enable_backlight();
}
/* END OF FUNCTION*/

void App_update_system_state(void)
{
	prev_system_state = system_state;

	switch(system_state)
	{
	case RUN_MODE: 				system_state = HALT_MODE; break;
	case HALT_MODE: 			system_state = STOP_MODE; break;
	case STOP_MODE: 			system_state = STOP_MODE_NL; break;
	case STOP_MODE_NL: 			system_state = STOP_MODE_NL_RAMSD; break;
	case STOP_MODE_NL_RAMSD: 	system_state = STOP_MODE_FIO; break;
	case STOP_MODE_FIO: 		system_state = STOP_MODE_CCIO; break;
	case STOP_MODE_CCIO: 		system_state = RUN_MODE; break;
	default:
		system_state = RUN_MODE;
		break;
	}
}
/* END OF FUNCTION*/

