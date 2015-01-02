/*
    FreeRTOS V8.1.2 - Copyright (C) 2014 Real Time Engineers Ltd.
    All rights reserved

	***********************************************************************
	*	FILE NAME:		housekeep_test.c
	*
	*	PURPOSE:		
	*	This file will be used to initialize and test the housekeeping task.
	*
	*	FILE REFERENCES:	stdio.h, FreeRTOS.h, task.h, partest.h, asf.h
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
	*	New tasks should be written to use as much of CMSIS as possible. The ASF and 
	*	FreeRTOS API libraries should also be used whenever possible to make the program
	*	more portable.

	*	DEVELOPMENT HISTORY:		
	*	12/31/2014			Created
	*
	*	DESCRIPTION:
	*	
 */

/* Standard includes. */
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Atmel library includes. */
#include "asf.h"

/* Common demo includes. */
#include "partest.h"

/* Priorities at which the tasks are created. */
#define HouseKeep_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )		// Lower the # means lower the priority

/* Values passed to the two tasks just to check the task parameter
functionality. */
#define HK_PARAMETER			( 0xABCD )

/*-----------------------------------------------------------*/

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvHouseKeepTask( void *pvParameters );

void my_blink( void );

void housekeep_test( void )
{
		
		/* Start the two tasks as described in the comments at the top of this
		file. */
		xTaskCreate( prvHouseKeepTask,					/* The function that implements the task. */
					"ON", 									/* The text name assigned to the task - for debug only as it is not used by the kernel. */
					configMINIMAL_STACK_SIZE, 				/* The size of the stack to allocate to the task. */
					( void * ) HK_PARAMETER, /* The parameter passed to the task - just to check the functionality. */
					HouseKeep_TASK_PRIORITY, 		/* The priority assigned to the task. */
					NULL );									/* The task handle is not required, so NULL is passed. */
		 
		/* Start the tasks and timer running. */
		vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the idle and/or
	timer tasks	to be created.  See the memory management section on the
	FreeRTOS web site for more details. */
	for( ;; );
}
/*-----------------------------------------------------------*/

static void prvHouseKeepTask( void *pvParameters )
{
	configASSERT( ( ( unsigned long ) pvParameters ) == TurnOn_PARAMETER );
	
	/* Set up timer to be used for Housekeeping */
	
	
	for( ;; )
	{

	}
}
/*-----------------------------------------------------------*/

