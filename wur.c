/*
 * wur.c
 *
 *  Created on: 29 jul. 2019
 *      Author: marti
 */
#include PLATFORM_HEADER
#include CONFIGURATION_HEADER

#include "wur.h"
#include "ook_wur.h"
#include "i2c_wur.h"
#include "micro/endian.h"
#include <string.h>

#define FRAME_BUF_LEN WUR_HEADER_LEN + WUR_MAX_DATA_LEN
typedef enum wur_tx_status{
	WUR_STATUS_IDLE,
	WUR_STATUS_WAIT_DATA_ACK,
	WUR_STATUS_WAIT_WAKE_ACK
}wur_tx_status_t;

typedef struct wur_context{
	uint32_t tx_timestamp;
	uint32_t rx_timestamp;
	wur_tx_status_t wur_status;
	uint8_t expected_seq_num;
	wur_tx_cb tx_cb;
	wur_rx_cb rx_cb;
	uint8_t frame_buffer[FRAME_BUF_LEN];
	uint8_t frame_len;
	uint16_t wur_addr;
}wur_context_t;

static wur_context_t wur_context;
static volatile uint8_t wur_op_pending;

void wur_init(uint16_t addr){
	ook_wur_init();

	wur_i2c_init();

	wur_context.tx_timestamp = 0;
	wur_context.rx_timestamp = 0;
	wur_context.wur_status = WUR_STATUS_IDLE;
	wur_context.expected_seq_num = 0;
	wur_context.tx_cb = NULL;
	wur_context.rx_cb = NULL;
	memset(wur_context.frame_buffer, 0, FRAME_BUF_LEN);
	wur_context.frame_len = 0;
	wur_context.wur_addr = addr;

	wur_set_address(addr);
}

void wur_tick(uint32_t systick){

	if(wur_context.wur_status == WUR_STATUS_WAIT_DATA_ACK){
		if(systick - wur_context.tx_timestamp > WUR_DATA_TIMEOUT){
			emberAfCorePrintln("Timeout to ack last data frame");
			if(wur_context.tx_cb){
				wur_context.wur_status = WUR_STATUS_IDLE;
				wur_context.tx_cb(WUR_ERROR_TX_ACK_DATA_TIMEOUT);
			}
		}
	}else if(wur_context.wur_status == WUR_STATUS_WAIT_WAKE_ACK){
		if(systick - wur_context.tx_timestamp > WUR_WAKE_TIMEOUT){
			emberAfCorePrintln("Timeout to ack last wake frame");
			if(wur_context.tx_cb){
				wur_context.wur_status = WUR_STATUS_IDLE;
				wur_context.tx_cb(WUR_ERROR_TX_ACK_WAKE_TIMEOUT);
			}
		}
	}

	if(!wur_op_pending){
		return;
	}

	i2c_wur_status_t wurx_state;
	if(wur_get_status(&wurx_state) != WUR_OK){
		emberAfCorePrintln("Warning: failed to get state from WuR after interrupt!");
		wur_op_pending = false;
		return;
	}
	if(wurx_state.wur_status != WURX_HAS_FRAME){
		emberAfCorePrintln("Warning: Woke up without frame available!");
		wur_op_pending = false;
		return;
	}
	if(wur_get_frame(wur_context.frame_buffer, wurx_state.wur_frame_len) != WUR_OK){
		emberAfCorePrintln("Warning: failed to get frame from WuR!");
		wur_op_pending = false;
		return;
	}

	wur_context.frame_len = wurx_state.wur_frame_len;
	uint8_t frame_type = wur_context.frame_buffer[2] & 0x07;
	uint8_t seq_num = wur_context.frame_buffer[2] & 0x07;

	uint16_t addr;
	memcpy(&addr, wur_context.frame_buffer, 2);
	addr = NTOHS(addr);

	/* check the frame flags! */
	if(frame_type & ACK_FLAG){
		if(wur_context.expected_seq_num == seq_num){
			if(wur_context.tx_cb){
				wur_context.wur_status = WUR_STATUS_IDLE;
				wur_context.tx_cb(WUR_ERROR_TX_OK);
				wur_context.expected_seq_num ^= 1;
			}
		}
		else{
			if(wur_context.tx_cb){
				wur_context.wur_status = WUR_STATUS_IDLE;
				wur_context.tx_cb(WUR_ERROR_TX_NACK);
				wur_context.expected_seq_num ^= 1;
			}
		}
	}

	if((frame_type & DATA_FLAG) || (frame_type & WAKE_FLAG)){
		/* an ack can piggiback a response frame, so continue*/
		if((wur_context.frame_len > 3) && wur_context.rx_cb){
			wur_context.rx_cb(WUR_ERROR_RX_OK, wur_context.frame_buffer, wur_context.frame_len);
			wur_context.rx_timestamp = systick;
		}
		if(!(frame_type & ACK_FLAG)){
			wur_send_ack(addr, seq_num);
		}
	}

	memset(wur_context.frame_buffer, 0, FRAME_BUF_LEN);
	wur_context.frame_len = 0;
	wur_op_pending = false;
}

void wur_set_tx_cb(wur_tx_cb tx_cb){
	wur_context.tx_cb = tx_cb;
}

void wur_set_rx_cb(wur_rx_cb rx_cb){
	wur_context.rx_cb = rx_cb;
}

wur_tx_res_t wur_send_wake(uint16_t addr, uint16_t ms){
	ook_tx_errors_t tx_res;

	if(wur_context.wur_status != WUR_STATUS_IDLE){
		return WUR_ERROR_TX_BUSY;
	}

	tx_res = ook_wur_wake(addr, ms, wur_context.expected_seq_num);
	if(tx_res != OOK_WUR_TX_ERROR_SUCCESS){
		emberAfCorePrintln("Warning: failed to start WAKE transmission!");
		wur_context.wur_status = WUR_STATUS_IDLE;
		return WUR_ERROR_TX_FAILED;
	}

	wur_context.tx_timestamp = halCommonGetInt32uMillisecondTick();
	wur_context.wur_status = WUR_STATUS_WAIT_WAKE_ACK;

	return WUR_ERROR_TX_OK;
}

wur_tx_res_t wur_send_data(uint16_t addr, uint8_t* data, uint8_t data_len, boolean is_ack, int8_t ack_seq_num){
	ook_tx_errors_t tx_res;

	if(wur_context.wur_status != WUR_STATUS_IDLE){
		return WUR_ERROR_TX_BUSY;
	}

	if(ack_seq_num < 0){
		ack_seq_num = wur_context.expected_seq_num;
	}

	tx_res = ook_wur_data(addr, data, data_len, is_ack, ack_seq_num);
	if(tx_res != OOK_WUR_TX_ERROR_SUCCESS){
		emberAfCorePrintln("Warning: failed to start DATA transmission!");
		wur_context.wur_status = WUR_STATUS_IDLE;
		return WUR_ERROR_TX_FAILED;
	}

	wur_context.tx_timestamp = halCommonGetInt32uMillisecondTick();
	wur_context.wur_status = WUR_STATUS_WAIT_DATA_ACK;

	return WUR_ERROR_TX_OK;
}

wur_tx_res_t wur_send_ack(uint16_t addr, int8_t ack_seq_num){
	ook_tx_errors_t tx_res;

	if(wur_context.wur_status != WUR_STATUS_IDLE){
		return WUR_ERROR_TX_BUSY;
	}

	if(ack_seq_num < 0){
		ack_seq_num = wur_context.expected_seq_num;
	}

	tx_res = ook_wur_ack(addr, ack_seq_num);
	if(tx_res != OOK_WUR_TX_ERROR_QUEUED){
		emberAfCorePrintln("Warning: failed to start ACK transmission!");
		return WUR_ERROR_TX_FAILED;
	}

	return WUR_ERROR_TX_OK;
}

