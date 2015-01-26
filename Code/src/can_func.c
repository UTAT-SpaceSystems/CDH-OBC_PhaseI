/*
    Author: Keenan Burnett

	***********************************************************************
	*	FILE NAME:		can_func.c
	*
	*	PURPOSE:		
	*	This file contains routines and interrupt handlers that pertain to the
	*	Controller Area Network (CAN).
	*
	*	FILE REFERENCES:	can_func.h
	*
	*	EXTERNAL VARIABLES:		g_ul_recv_status  (= 1 when a message was received)
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
	*	01/02/2015		Created.
	*
	*	DESCRIPTION:	
	*
	*	This file is being used to house the housekeeping task.
	*	
 */

#include "can_func.h"

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
				
				// Decode CAN Message
				decode_can_msg(&can1_mailbox, CAN1);
				//assert(g_ul_recv_status); ***Implement assert here.
				g_ul_recv_status = 0;
				break;
			}
		}
	}
}
/************************************************************************/
/* Default Interrupt Handler for CAN0								    */
/************************************************************************/

/**
 * \brief Default interrupt handler for CAN0
 */
void CAN0_Handler(void)
{
	uint32_t ul_status;

	ul_status = can_get_status(CAN0);
	if (ul_status & GLOBAL_MAILBOX_MASK) {
		for (uint8_t i = 0; i < CANMB_NUMBER; i++) {
			ul_status = can_mailbox_get_status(CAN0, i);
			
			if ((ul_status & CAN_MSR_MRDY) == CAN_MSR_MRDY) {
				can0_mailbox.ul_mb_idx = i;
				can0_mailbox.ul_status = ul_status;
				can_mailbox_read(CAN0, &can0_mailbox);
				g_ul_recv_status = 1;
				
				// Decode CAN Message
				decode_can_msg(&can0_mailbox, CAN0);
				//assert(g_ul_recv_status); ***implement assert here.
				break;
			}
		}
	}
}

/************************************************************************/
/* Decode CAN Message													*/
/* Performs a prescribed action depending on the message received       */
/************************************************************************/

/**
 * \brief Decoodes the CAN message and performs a prescribed action depending on 
 * the message received.
 * @param *controller:  	CAN sending controller
 * @param *p_mailbox:		CAN mailbox structure of sending controller
 */
void decode_can_msg(can_mb_conf_t *p_mailbox, Can* controller)
{
	//assert(g_ul_recv_status);		// Only decode if a message was received.	***Asserts here.
	//assert(controller);				// CAN0 or CAN1 are nonzero.
	uint32_t ul_data_incom = p_mailbox->ul_datal;
	if(controller == CAN0)
		pio_toggle_pin(LED0_GPIO);
	if(controller == CAN1)
		pio_toggle_pin(LED1_GPIO);
	if (ul_data_incom == COMMAND_OUT)
		pio_toggle_pin(LED0_GPIO);
	if (ul_data_incom == COMMAND_IN)
		pio_toggle_pin(LED1_GPIO);

	if ((ul_data_incom == COMMAND_IN) & (controller == CAN0)) 
	{
		// Command has been received, respond.
		pio_toggle_pin(LED0_GPIO);
		//command_in();
	}
	if ((ul_data_incom == COMMAND_OUT) & (controller == CAN1))
	{
		pio_toggle_pin(LED2_GPIO);	// LED2 indicates the response to the command
	}								// has been received.
	if ((ul_data_incom == HK_TRANSMIT) & (controller == CAN1))
	{
		pio_toggle_pin(LED3_GPIO);	// LED3 indicates housekeeping has been received.
	}
	return;
}

/************************************************************************/
/* RESET MAILBOX CONFIGURE STRUCTURE                                    */
/************************************************************************/

/**
 * \brief Resets the mailbox configure structure.  
 * @param *p_mailbox:		Mailbox structure that will be reset. 
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

/************************************************************************/
/*                 SEND A 'COMMAND' FROM CAN1 TO CAN0		            */
/************************************************************************/

/**
 * \brief Sends a 'command' from CAN0 to CAN1
 */
void command_out(void)
{
	pio_toggle_pin(LED0_GPIO);
	can_reset_all_mailbox(CAN0);
	can_reset_all_mailbox(CAN1);

	/* Init CAN0 Mailbox 0 to Reception Mailbox. */
	reset_mailbox_conf(&can0_mailbox);
	can0_mailbox.ul_mb_idx = 0;
	can0_mailbox.uc_obj_type = CAN_MB_RX_MODE;
	can0_mailbox.ul_id_msk = CAN_MAM_MIDvA_Msk | CAN_MAM_MIDvB_Msk;
	can0_mailbox.ul_id = CAN_MID_MIDvA(7);
	can_mailbox_init(CAN0, &can0_mailbox);

	/* Init CAN1 Mailbox 0 to Transmit Mailbox. */
	reset_mailbox_conf(&can1_mailbox);
	can1_mailbox.ul_mb_idx = 0;
	can1_mailbox.uc_obj_type = CAN_MB_TX_MODE;
	can1_mailbox.uc_tx_prio = 15;
	can1_mailbox.uc_id_ver = 0;
	can1_mailbox.ul_id_msk = 0;
	can_mailbox_init(CAN1, &can1_mailbox);

	/* Write transmit information into mailbox. */
	can1_mailbox.ul_id = CAN_MID_MIDvA(7);
	can1_mailbox.ul_datal = COMMAND_IN;
	can1_mailbox.ul_datah = CAN_MSG_DUMMY_DATA;
	can1_mailbox.uc_length = MAX_CAN_FRAME_DATA_LEN;
	can_mailbox_write(CAN1, &can1_mailbox);

	/* Enable CAN1 mailbox 0 interrupt. */
	can_enable_interrupt(CAN0, CAN_IER_MB0);

	/* Send out the information in the mailbox. */
	can_global_send_transfer_cmd(CAN1, CAN_TCR_MB0);

	/* potentially @non-terminating@ */
	while (!g_ul_recv_status) {
	}

}

/************************************************************************/
/*				RESPOND TO THE COMMAND FROM CAN0 AND SEND TO CAN1       */
/************************************************************************/

/**
 * \brief Responds to he command from CAN0 and sends to CAN1
 **/
void command_in(void)
{
	pio_toggle_pin(LED0_GPIO);
	
	can_disable_interrupt(CAN0, CAN_IER_MB0);
	NVIC_DisableIRQ(CAN0_IRQn);
	
	can_reset_all_mailbox(CAN0);
	can_reset_all_mailbox(CAN1);

	/* Init CAN1 Mailbox 0 to Reception Mailbox. */
	reset_mailbox_conf(&can0_mailbox);
	can1_mailbox.ul_mb_idx = 1;
	can1_mailbox.uc_obj_type = CAN_MB_RX_MODE;
	can1_mailbox.ul_id_msk = CAN_MAM_MIDvA_Msk | CAN_MAM_MIDvB_Msk;
	can1_mailbox.ul_id = CAN_MID_MIDvA(7);
	can_mailbox_init(CAN1, &can1_mailbox);

	/* Init CAN0 Mailbox 0 to Transmit Mailbox. */
	reset_mailbox_conf(&can0_mailbox);
	can0_mailbox.ul_mb_idx = 1;
	can0_mailbox.uc_obj_type = CAN_MB_TX_MODE;
	can0_mailbox.uc_tx_prio = 15;
	can0_mailbox.uc_id_ver = 0;
	can0_mailbox.ul_id_msk = 0;
	can_mailbox_init(CAN0, &can0_mailbox);

	/* Write transmit information into mailbox. */
	can0_mailbox.ul_id = CAN_MID_MIDvA(7);
	can0_mailbox.ul_datal = COMMAND_OUT;
	can0_mailbox.ul_datah = CAN_MSG_DUMMY_DATA;
	can0_mailbox.uc_length = MAX_CAN_FRAME_DATA_LEN;
	can_mailbox_write(CAN0, &can0_mailbox);

	/* Enable CAN1 mailbox 0 interrupt. */
	can_enable_interrupt(CAN1, CAN_IER_MB1);

	/* Send out the information in the mailbox. */
	can_global_send_transfer_cmd(CAN0, CAN_TCR_MB1);

	/* potentially @non-terminating@ */
	while (!g_ul_recv_status) {
	}
}

/**
 * \brief Initializes and enables CAN0 & CAN1 tranceivers and clocks. 
 * CAN0/CAN1 mailboxes are reset and interrupts disabled.
 */
void can_initialize(void)
{
	uint32_t ul_sysclk;

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
		
	NVIC_EnableIRQ(CAN0_IRQn);
	NVIC_EnableIRQ(CAN1_IRQn);
	
	can_reset_all_mailbox(CAN0);
	can_reset_all_mailbox(CAN1);
	}
	return;
}
			
	