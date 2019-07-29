/*
 * ook_wur.c
 *
 *  Created on: 29 jul. 2019
 *      Author: marti
 */

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER

#include "ook_com.h"
#include "ook_wur.h"
#include "micro/endian.h"
#include <string.h>

ook_tx_errors_t _ook_wur_transmit(uint8_t* data, uint8_t len);

typedef struct ook_wur_ctxt{
	ook_tx_errors_t tx_result;
	ook_tx_status_t tx_status;
	uint8_t 		tx_current_seq;
}ook_wur_ctxt_t;

/* will be modified in RAIL ISR */
static volatile ook_wur_ctxt_t ook_wur_ctxt;

void ook_wur_finish_tx(ook_tx_status_t status){
	ook_wur_ctxt.tx_status = OOK_WUR_TX_STATUS_IDLE;
	ook_wur_ctxt.tx_result = status;
	ook_wur_ctxt.tx_current_seq = 0;
}

void ook_wur_init(void){
	ook_com_startRadio();

	ook_wur_ctxt.tx_status = OOK_WUR_TX_STATUS_IDLE;
	ook_wur_ctxt.tx_result = OOK_WUR_TX_ERROR_SUCCESS;
	ook_wur_ctxt.tx_current_seq = 0;
}

ook_tx_errors_t ook_wur_wake(uint16_t dest, uint16_t ms_wake){
	uint8_t wake_frame[5] = {0};

	wake_frame[0] = dest & 0xFF;
	wake_frame[1] = (dest & 0xF00) >> 4;
	wake_frame[1] |= (DATA_FLAG << 1) | ook_wur_ctxt.tx_current_seq;
	wake_frame[2] = WUR_WAKE_LEN;
	ms_wake = HTONS(ms_wake);
	memcpy(&wake_frame[3], &ms_wake, 2);

	return _ook_wur_transmit(wake_frame, WUR_WAKE_LEN + WUR_HEADER_LEN);
}

ook_tx_errors_t ook_wur_data(uint16_t dest, uint8_t* data, uint8_t len, bool ack){
	uint8_t data_frame[WUR_MAX_DATA_LEN] = {0};
	uint8_t flags = 0;

	if(len > WUR_MAX_DATA_LEN){
		return OOK_WUR_TX_ERROR_FRAME_FORMAT;
	}

	data_frame[0] = dest & 0xFF;
	data_frame[1] = (dest & 0xF00) >> 4;

	/* set type and seq flag */
	flags |= DATA_FLAG;
	if(ack){
		flags |= ACK_FLAG;
	}

	data_frame[1] |= (flags << 1) | ook_wur_ctxt.tx_current_seq;
	data_frame[2] = len;

	memcpy(&data_frame[3], data, len);

	return _ook_wur_transmit(data_frame, len + WUR_HEADER_LEN);
}

ook_tx_errors_t ook_wur_ack(uint16_t dest){
	return ook_wur_data(dest, NULL, 0, true);
}

ook_tx_errors_t _ook_wur_transmit(uint8_t* data, uint8_t len){
	int32_t res;

	if(ook_wur_ctxt.tx_status != OOK_WUR_TX_STATUS_IDLE){
		return OOK_WUR_TX_ERROR_BUSY;
	}

	ook_wur_ctxt.tx_status = OOK_WUR_TX_STATUS_BUSY;

	res = ook_com_sendFrame(data, len);
	if(res != 0){
		ook_wur_ctxt.tx_status = OOK_WUR_TX_STATUS_IDLE;
		return OOK_WUR_TX_ERROR_FRAME_FORMAT;
	}

	/* busy wait until transmission (at most a few ms) */
	while(ook_wur_ctxt.tx_status == OOK_WUR_TX_STATUS_BUSY);

	return ook_wur_ctxt.tx_result;
}

void ook_wur_callback(ook_tx_errors_t dest){
	ook_wur_ctxt.tx_status = OOK_WUR_TX_STATUS_IDLE;
	ook_wur_ctxt.tx_result = dest;
}


