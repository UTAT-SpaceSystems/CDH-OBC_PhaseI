/*
    FreeRTOS V8.1.2 - Copyright (C) 2014 Real Time Engineers Ltd.
    All rights reserved

	***********************************************************************
	*	FILE NAME:		can_test0.h
	*
	*	PURPOSE:		
	*	This file allows can_test0.c to have it's set of includes and variable
	*	declarations before sysclk_init and board_init are called. This is important
	*	if CAN is to be initialized correctly.
	*	
	*
	*	FILE REFERENCES:	None.
	*
	*	EXTERNAL VARIABLES:	sn65hvd234.h, can.h
	*
	*	EXTERNAL REFERENCES:	Same a File References.
	*
	*	ABORNOMAL TERMINATION CONDITIONS, ERROR AND WARNING MESSAGES: None yet.
	*
	*	ASSUMPTIONS, CONSTRAINTS, CONDITIONS:	None
	*
	*	NOTES:	 In a separate project this was modified to work with Arduino DUE
	*			 instead of the SAM3X-EK. It was again modified to work in conjunction
	*			 with the framework set up by this project.
	*
	*	REQUIREMENTS/ FUNCTIONAL SPECIFICATION REFERENCES:
	*	New tasks should be written to use as much of CMSIS as possible. The ASF and
	*	FreeRTOS API libraries should also be used whenever possible to make the program
	*	more portable.

	*	DEVELOPMENT HISTORY:
	*	12/20/2014		Created.
	*
*/
#include <asf/sam/components/can/sn65hvd234.h>
#include <asf/sam/drivers/can/can.h>
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "sysclk.h"
#include "exceptions.h"
#include "pmc.h"
//#include <asf/common/utils/stdio/stdio_serial/stdio_serial.h>
#include "conf_board.h"
#include "conf_clock.h"
#include "pio.h"


#define CAN_MSG_TOGGLE_LED_0        0x11223344
#define CAN_MSG_TOGGLE_LED_1        0xAABBCCDD
#define CAN_MSG_DUMMY_DATA          0x55AAAA55

/** CAN frame max data length */
#define MAX_CAN_FRAME_DATA_LEN      8

/** CAN0 Transceiver */
sn65hvd234_ctrl_t can0_transceiver;

/** CAN1 Transceiver */
sn65hvd234_ctrl_t can1_transceiver;

/** CAN0 Transfer mailbox structure */
can_mb_conf_t can0_mailbox;

/** CAN1 Transfer mailbox structure */
can_mb_conf_t can1_mailbox;

/** Receive status */
//volatile uint32_t g_ul_recv_status = 0;

void CAN1_Handler0(void);
void decode_can_msg0(can_mb_conf_t *p_mailbox);
void reset_mailbox_conf0(can_mb_conf_t *p_mailbox);
void test_0(void);
int can_test0(void);
/*---------------------------------------------------------*/

