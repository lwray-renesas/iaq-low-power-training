/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
*  File Name    : iaq_low_power.c
*  Description  : Main Program
*  Creation Date: 2023-03-21
*  This file was generated by Smart Configurator.
***********************************************************************************************************************/
#include "app/app.h"


/** @brief Function to handle low power modes*/
static void App_handle_low_power(void)
{
   /* Infinitely wait for a button click to progress*/
   App_wait_for_button_click();

   /* Before enterring low power state - isable the backlight*/
   Hw_disable_backlight();

   /* Handle sleep state*/
   switch(system_state)
   {
   case RUN_MODE:
   {
	   /* Infinitely wait for a button click to progress*/
	   App_wait_for_button_click();
   }
   break;
   case HALT_MODE:
   {
	   HALT();
   }
   break;

   case STOP_MODE:
   {
	   STOP();
   }
   break;

   case STOP_MODE_NL:
   {
	   /* Turn off LOCO*/
	   SELLOSC = 0U;
	   WUTMMCK0 = 0U;

	   STOP();

	   /* Turn on LOCO*/
	   SELLOSC = 1U;
	   WUTMMCK0 = 1U;
   }
   break;

   case STOP_MODE_NL_RAMSD:
   {
	   /* Turn off LOCO*/
	   SELLOSC = 0U;
	   WUTMMCK0 = 0U;

	   /* RAM Shutdown*/
	   RAMSDMD = 1U;
	   R_BSP_SoftwareDelay(1UL, BSP_DELAY_MICROSECS);
	   RAMSDS = 1U;

	   STOP();

	   /* RAM Wakeup*/
	   RAMSDS = 0U;
	   R_BSP_SoftwareDelay(2UL, BSP_DELAY_MICROSECS);
	   RAMSDMD = 0U;

	   /* Turn on LOCO*/
	   SELLOSC = 1U;
	   WUTMMCK0 = 1U;
   }
   break;

   case STOP_MODE_FIO:
   {
	   /* Make port an input to allow floating currents*/
	   PM2_bit.no7 = 1U;

	   STOP();

	   /* Reset to output*/
	   PM2_bit.no7 = 0U;
   }
   break;

   case STOP_MODE_CCIO:
   {
	   /* Introduce CCIO peripheral current draw - while backlight still disabled*/
	   CCS6 = 0x00U; /* Hi-Z backlight*/
	   CCDE |= 0x40U; /* Enable CCIO perihperal*/
	   P6_bit.no2 = 0U; /* Make appropriate port setting to allow CCIO*/

	   STOP();
   }
   break;

   default:
	   /* Do nothing - should never get here*/
	   break;
   }
}
/* END OF FUNCTION*/


/** @brief Main function - NOTE: Line 651, mcu_clocks.c, cmc register updated for low power*/
void main(void)
{
   /* Enable handling of interrupts*/
   EI();

   /* Prepare drivers and display*/
   App_init();

   while(1)
   {
	   /* Update the display*/
	   App_update_display();

	   /* Select, prepare and enter low power state*/
	   App_handle_low_power();

	   /* Increment system state & update previous system state*/
	   App_update_system_state();
   }
}
/* END OF MAIN*/
