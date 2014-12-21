/*
    FreeRTOS V8.1.2 - Copyright (C) 2014 Real Time Engineers Ltd.
    All rights reserved

	***********************************************************************
	*	FILE NAME:		can_test0.c
	*
	*	PURPOSE:		
	*	This is a ported version of the CAN Example used in Atmel studio.
	*	
	*
	*	FILE REFERENCES:	can_test0.h 
	*
	*	EXTERNAL VARIABLES:
	*
	*	EXTERNAL REFERENCES:	sn65hvd234.h and can.h by extension of can_test0.h
	*
	*	ABORNOMAL TERMINATION CONDITIONS, ERROR AND WARNING MESSAGES: 
	*
	*	In Atmel's CAN example, sn65hvd234_disable() tend to run into an infinite loop,
	*	although within the main program here, there does not seem to be any issues.
	*	Simply, refrain from disabling the transceiver component and all should be fine.
	*
	*	ASSUMPTIONS, CONSTRAINTS, CONDITIONS:	None
	*
	*	NOTES:	 In future code, avoid the use of sn65hvd234_disable(), there isn't
	*	really a need for it as CAN doesn't need to be disabled once the CPU is running.
	*	
	*	REQUIREMENTS/ FUNCTIONAL SPECIFICATION REFERENCES:			
	*	New tasks should be written to use as much of CMSIS as possible. The ASF and 
	*	FreeRTOS API libraries should also be used whenever possible to make the program
	*	more portable.

	*	DEVELOPMENT HISTORY:		
	*	12/20/2014		Created.
	*
 */
#include "can_test0.h"

volatile uint32_t g_ul_recv_status = 0;

/**
 * \brief Default interrupt handler for CAN 1.
 */
void CAN1_Handler(void)
{
	uint32_t ul_status;

	ul_status = can_get_status(CAN1);
	if (ul_status & GLOBAL_MAILBOX_MASK) {
		for (uint8_t i = 0; i < CANMB_NUMBER; i++) {
			ul_status = can_mailbox_get_status(CAN1, i);
			if ((ul_status & CAN_MSR_MRDY) == CAN_MSR_MRDY) {
				can1_mailbox.ul_mb_idx = i;
				can1_mailbox.ul_status = ul_status;
				can_mailbox_read(CAN1, &can1_mailbox);
				g_ul_recv_status = 1;
				break;
			}
		}
	}
}

/**
 * \brief Decode CAN messages.
 *
 *  \param p_mailbox Pointer to CAN Mailbox structure.
 */
void decode_can_msg(can_mb_conf_t *p_mailbox)
{
	uint32_t ul_led_Ctrl = p_mailbox->ul_datal;

	//("CAN message:" STRING_EOL);
	if (ul_led_Ctrl == CAN_MSG_TOGGLE_LED_0) {
		//("  Toggle LED 0" STRING_EOL);
		pio_toggle_pin(LED0_GPIO);
	} else if (ul_led_Ctrl == CAN_MSG_TOGGLE_LED_1) {
		//("  Toggle LED 1" STRING_EOL);
		pio_toggle_pin(LED1_GPIO);
	}
}

/**
 * \brief Reset mailbox configure structure.
 *
 *  \param p_mailbox Pointer to mailbox configure structure.
 */
void reset_mailbox_conf(can_mb_conf_t *p_mailbox)
{
	p_mailbox->ul_mb_idx = 0;
	p_mailbox->uc_obj_type = 0;
	p_mailbox->uc_id_ver = 0;
	p_mailbox->uc_length = 0;
	p_mailbox->uc_tx_prio = 0;
	p_mailbox->ul_status = 0;
	p_mailbox->ul_id_msk = 0;
	p_mailbox->ul_id = 0;
	p_mailbox->ul_fid = 0;
	p_mailbox->ul_datal = 0;
	p_mailbox->ul_datah = 0;
}

#define TEST1_CAN_COMM_MB_IDX    0
#define TEST1_CAN_TRANSFER_ID    0x07
#define TEST1_CAN0_TX_PRIO       15
/**
 *  \brief Test the transmission from CAN0 Mailbox 0 to CAN1 Mailbox 0.
 */

void test_1(void)
{
	pio_toggle_pin(LED0_GPIO);
	can_reset_all_mailbox(CAN0);
	can_reset_all_mailbox(CAN1);

	//("\n\rTest1: CAN0 Mailbox 0 transmitting to CAN1 Mailbox 0" STRING_EOL);

	/* Init CAN1 Mailbox 0 to Reception Mailbox. */
	reset_mailbox_conf(&can1_mailbox);
	can1_mailbox.ul_mb_idx = TEST1_CAN_COMM_MB_IDX;
	can1_mailbox.uc_obj_type = CAN_MB_RX_MODE;
	can1_mailbox.ul_id_msk = CAN_MAM_MIDvA_Msk | CAN_MAM_MIDvB_Msk;
	can1_mailbox.ul_id = CAN_MID_MIDvA(TEST1_CAN_TRANSFER_ID);
	can_mailbox_init(CAN1, &can1_mailbox);

	/* Init CAN0 Mailbox 0 to Transmit Mailbox. */
	reset_mailbox_conf(&can0_mailbox);
	can0_mailbox.ul_mb_idx = TEST1_CAN_COMM_MB_IDX;
	can0_mailbox.uc_obj_type = CAN_MB_TX_MODE;
	can0_mailbox.uc_tx_prio = TEST1_CAN0_TX_PRIO;
	can0_mailbox.uc_id_ver = 0;
	can0_mailbox.ul_id_msk = 0;
	can_mailbox_init(CAN0, &can0_mailbox);

	/* Write transmit information into mailbox. */
	can0_mailbox.ul_id = CAN_MID_MIDvA(TEST1_CAN_TRANSFER_ID);
	can0_mailbox.ul_datal = CAN_MSG_TOGGLE_LED_0;
	can0_mailbox.ul_datah = CAN_MSG_DUMMY_DATA;
	can0_mailbox.uc_length = MAX_CAN_FRAME_DATA_LEN;
	can_mailbox_write(CAN0, &can0_mailbox);

	/* Enable CAN1 mailbox 0 interrupt. */
	can_enable_interrupt(CAN1, CAN_IER_MB0);

	/* Send out the information in the mailbox. */
	can_global_send_transfer_cmd(CAN0, CAN_TCR_MB0);

	while (!g_ul_recv_status) {
	}

	if ((can1_mailbox.ul_datal == CAN_MSG_TOGGLE_LED_0) &&
	(can1_mailbox.uc_length == MAX_CAN_FRAME_DATA_LEN)) {
		//("Test1 passed" STRING_EOL);
		decode_can_msg(&can1_mailbox);
		} else {
		//("Test1 ERROR" STRING_EOL);
	}
}

int can_test(void)
{
	uint32_t ul_sysclk;
	//uint8_t uc_char;			*** UNUSED?

	/* Initialize the SAM system. */
	//sysclk_init();
	//board_init();

	/* Initialize CAN0 Transceiver. */
	sn65hvd234_set_rs(&can0_transceiver, PIN_CAN0_TR_RS_IDX);
	sn65hvd234_set_en(&can0_transceiver, PIN_CAN0_TR_EN_IDX);
	/* Enable CAN0 Transceiver. */
	sn65hvd234_disable_low_power(&can0_transceiver);
	sn65hvd234_enable(&can0_transceiver);

	/* Initialize CAN1 Transceiver. */
	sn65hvd234_set_rs(&can1_transceiver, PIN_CAN1_TR_RS_IDX);
	sn65hvd234_set_en(&can1_transceiver, PIN_CAN1_TR_EN_IDX);
	/* Enable CAN1 Transceiver. */
	sn65hvd234_disable_low_power(&can1_transceiver);
	sn65hvd234_enable(&can1_transceiver);

	/* Enable CAN0 & CAN1 clock. */
	pmc_enable_periph_clk(ID_CAN0);
	pmc_enable_periph_clk(ID_CAN1);

	ul_sysclk = sysclk_get_cpu_hz();
	if (can_init(CAN0, ul_sysclk, CAN_BPS_250K) &&
	can_init(CAN1, ul_sysclk, CAN_BPS_250K)) {

		/* Disable all CAN0 & CAN1 interrupts. */
		can_disable_interrupt(CAN0, CAN_DISABLE_ALL_INTERRUPT_MASK);
		can_disable_interrupt(CAN1, CAN_DISABLE_ALL_INTERRUPT_MASK);

		/* Configure and enable interrupt of CAN1, as the tests will use receiver interrupt. */
		NVIC_EnableIRQ(CAN1_IRQn);

		/* Run tests. */
		test_1();
		g_ul_recv_status = 0;

		/* Disable CAN0 Controller */
		can_disable(CAN0);
		/* Disable CAN0 Transceiver */
		sn65hvd234_enable_low_power(&can0_transceiver);
		sn65hvd234_disable(&can0_transceiver);

		/* Disable CAN1 Controller */
		can_disable(CAN1);
		/* Disable CAN1 Transceiver */
		sn65hvd234_enable_low_power(&can1_transceiver);
		sn65hvd234_disable(&can1_transceiver);

		} else {
	}

	while (1) {
	}
}

