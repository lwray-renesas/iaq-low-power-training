#ifndef MAIN_H_
#define MAIN_H_

#include "r_smc_entry.h"
#include "display/driver/st7735s.h"
#include "display/text/text.h"

/** @brief sets a hardware event flag*/
#define HW_SET_EVENT(flags,event)	{flags |= event;}
/** @brief clears a hardware event flag*/
#define HW_CLEAR_EVENT(flags,event)	{flags &= ~event;}
/** @brief Checks if hardware event flag is set*/
#define HW_EVENT_OCCURRED(flags,event)	( ((flags) & (event)) == event )

/** @brief Configuration macro for state text X location*/
#define TEXT_X_POSITION		(20U)
/** @brief Configuration macro for state text Y location*/
#define TEXT_Y_POSITION		(10U)

/** @brief macro used to prepare for disabling interrupts*/
#define PREPARE_CRITICAL_SECTION() uint8_t l_int_status = Interrupts_enabled()
/** @brief macro used to disable interrupts*/
#define ENTER_CRITICAL_SECTION()	DI()
/** @brief macro used to enable interrupts*/
#define EXIT_CRITICAL_SECTION()	if(l_int_status == 1U) { EI(); }

/** @brief Enumerated type for hardware event flags*/
typedef enum
{
	NO_EVENT				=	0x0000U,
	BUTTON_CLICK 			=	0x0001U,
	ALL_HARDWARE_EVENTS		=	0x0001U,
}hardware_event_t;

/** @brief Enumerated type for system state level*/
typedef enum
{
	RUN_MODE			=	0x0000U, /* Disables backlight to loo at pure cpu power*/
	HALT_MODE,						/* Halt mode only*/
	STOP_MODE,						/* Stop mode only*/
	STOP_MODE_NL,					/* Stop mode with disabled LOCO*/
	STOP_MODE_NL_RAMSD,				/* Stop mode with disabled LOCO & RAM Shutdown*/
	STOP_MODE_FIO,					/* Stop mode with floating IO currents*/
	STOP_MODE_CCIO,					/* Stop mode with current controlled IO enabled (Hi-Z output though)*/
}system_state_type;

/** Colours*/
static const uint8_t COLOUR_BLACK[] = {0x00, 0x00};
static const uint8_t COLOUR_WHITE[] = {0xFF, 0xFF};

/* Display area data*/
extern ST7735S_display_area_info_t disp_info;
/* variable to track system state*/
extern system_state_type system_state;
/* variable to track previous system state*/
extern system_state_type prev_system_state;
/* Array of titles for each state*/
extern const char * titles[];
extern volatile hardware_event_t hw_event_flags;

/** @brief turn on the backlight*/
static inline void Hw_enable_backlight(void)
{
	CCDE |= 0x40U;
	CCS6 = 0x02U; /* 5mA backlight*/
	P6_bit.no2 = 0U;
}
/* END OF FUNCTION*/

/** @brief turn on the backlight*/
static inline void Hw_disable_backlight(void)
{
	P6_bit.no2 = 1U;
	CCS6 = 0x00U; /* Hi-Z backlight*/
	CCDE &= ~0x40U;
}
/* END OF FUNCTION*/

/** @brief Inline assembly function to return interrupt enabled state
 * @return 1 = enabled, 0 otherwise.
 */
#pragma inline_asm Interrupts_enabled
static uint8_t Interrupts_enabled(void)
{
	MOV 	A, PSW
	SHR		A, 7
}
/* END OF FUNCTION*/

/** @brief Helper function to wait for button click*/
void App_wait_for_button_click(void);

/** @brief Copies data into RAMSD safe section, initialises the display & starts drivers*/
void App_init(void);

/** @brief updates the display depending on system state*/
void App_update_display(void);

/** @brief helper function to update system state*/
void App_update_system_state(void);

#endif
