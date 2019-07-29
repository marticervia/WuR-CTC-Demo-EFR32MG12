/*
 * ook_wur.h
 *
 *  Created on: 29 jul. 2019
 *      Author: marti
 */

#ifndef OOK_WUR_H_
#define OOK_WUR_H_

#define WUR_WAKE_LEN 2
#define WUR_HEADER_LEN 3
#define WUR_MAX_DATA_LEN 89

#define WAKE_FLAG 0b001
#define ACK_FLAG 0b010
#define DATA_FLAG 0b100

#define DEFAULT_WUR_FRAME { 														\
  0b01010101, /*sync word "11" and first byte of 12 bit address "010101010101"*/	\
  0b01010010, /*last nibble of 12 bit address, type "001" AND PARITY "0"*/			\
  0x0C, 	  /*length*/															\
  0x55, 0x33, 0x0F,																	\
  0x55, 0x33, 0x0F,																	\
  0x55, 0x33, 0x0F,																	\
  0x55, 0x33, 0x0F																	\
};

#define DEFAULT_WUR_LEN 16

typedef enum ook_tx_errors{
	OOK_WUR_TX_ERROR_SUCCESS = 0,
	OOK_WUR_TX_ERROR_QUEUED = 1,
	OOK_WUR_TX_ERROR_TIMEOUT = -1,
	OOK_WUR_TX_ERROR_BUSY = -2,
	OOK_WUR_TX_ERROR_FRAME_FORMAT = -3,
	OOK_WUR_TX_ERROR_FAILED = -4
}ook_tx_errors_t;

typedef enum ook_tx_status{
	OOK_WUR_TX_STATUS_IDLE = 0,
	OOK_WUR_TX_STATUS_BUSY = 1
}ook_tx_status_t;

typedef void (*ook_tx_callback_t)(ook_tx_errors_t tx_status);


void ook_wur_init(void);

ook_tx_errors_t ook_wur_wake(uint16_t dest, uint16_t ms_wake);
ook_tx_errors_t ook_wur_data(uint16_t dest, uint8_t* data, uint8_t len, bool ack);
ook_tx_errors_t ook_wur_ack(uint16_t dest);
void ook_wur_set_callback(ook_tx_callback_t tx_status_cb);

/* for use in ook_com.c*/
void ook_wur_callback(ook_tx_errors_t dest);

#endif /* OOK_WUR_H_ */
