/*
    Author: Keenan Burnett

	***********************************************************************
	*	FILE NAME:		analog_test.c
	*
	*	PURPOSE:		
	*	This file will be used to test reading an analog sensor from a PIO pin.
	*
	*	FILE REFERENCES:	None.
	*
	*	EXTERNAL VARIABLES:
	*
	*	EXTERNAL REFERENCES:	Same a File References.
	*
	*	ABORNOMAL TERMINATION CONDITIONS, ERROR AND WARNING MESSAGES: None yet.
	*
	*	ASSUMPTIONS, CONSTRAINTS, CONDITIONS:	None
	*
	*	NOTES:	 
	*	
	*	REQUIREMENTS/ FUNCTIONAL SPECIFICATION REFERENCES:			
	*
	*	DEVELOPMENT HISTORY:		
	*	02/19/2015		Created.
	*
	*	DESCRIPTION:	
	*
	*	This file simply creates a loop and polls the value of PIO register which is connected to an 
	*	analog sensor. When the sensor reaches an appropriate value, an LED is toggled.
	*	
 */

/* Standard includes. */
#include <stdio.h>
/*-----------------------------------------------------------*/

/*
 * Functions Prototypes.
 */

void analog_test(void);

/************************************************************************/
/*			2ND	TEST FUNCTION FOR HOUSEKEEPING                          */
/************************************************************************/
/**
 * \brief Tests the housekeeping task.
 */
void analog_test( void )
{
	uint32_t data;
	
	pio_configure_pin(ANALOG_IN0, 0x5);	//Enable the pin to being an output
	
	while(1)
	{
		pio_get_pin_value(uint32_t ul_pin);
		
		data = pio_get_pin_value(ANALOG_IN0);
	}
	return;
}
/*-----------------------------------------------------------*/


