/*******************************************************************************
 * @file ook_wur.c
 * @brief Functions to transmit OOK WuR frames using the RAIL low-level radio API.
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include PLATFORM_HEADER
#include CONFIGURATION_HEADER

#include "rail.h"
#include "rail_types.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_emu.h"
#include <hal/micro/led.h>
#include <ook_com.h>
#include "gpiointerrupt.h"
#include "rail_config.h"
#include "bsp.h"

#include "ook_wur.h"

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events);
void radioInit();


/******************************************************************************
 * Private Typedefs
 *****************************************************************************/

typedef struct ButtonArray{
  GPIO_Port_TypeDef   port;
  unsigned int        pin;
} ButtonArray_t;



/******************************************************************************
 * Private Defines
 *****************************************************************************/

#define LED_TX (0)
#define LED_RX (1)

// Memory manager configuration
#define MAX_BUFFER_SIZE  256

// Minimum allowed size of the TX FIFO
#define RAIL_TX_FIFO_SIZE 64
#define DEFAULT_RAIL_FRAME_SIZE 15

// General application memory sizes
#define APP_MAX_PACKET_LENGTH  (MAX_BUFFER_SIZE - 12) /* sizeof(RAIL_RxPacketInfo_t) == 12) */


#define RAIL_CSMA_CONFIG_WuR_2p4_GHz_OOK_CSMA {                    \
    /* CSMA per WuR OOK on 2.4 GHz OOK, based on the specification on IEEE 802.11-2003
     * for OFDM on mixed bg networks. The maxCCA exponent is reduced from 10 to 8 for
     *  limitations on EFR hardware     */ \
    /* csmaMinBoExp */ 4,   /* 2^4-1 for 0..15 backoffs on 1st try           */ \
    /* csmaMaxBoExp */ 8,   /* 2^8-1 for 0..255 backoffs on 4rd+ tries       */ \
    /* csmaTries    */ 7,   /* 5 tries overall (4 re-tries)                 */ \
    /* ccaThreshold */ -82, /* Sensitivity for 6mbps OFDM                   */ \
    /* ccaBackoff   */ 20, /*  Slot time for mixed mode IEEE 802.11g        */ \
    /* ccaDuration  */ 15, /*  As specified on IEEE 802.11-2003.
    / * TODO: correct it for EFR32 radio turnaround times.                 */ \
    /* csmaTimeout  */ 0,   /* No timeout                                   */ \
}

#define RAIL_WUR_SCHEDULE_INFO {			\
				.priority = 1,				\
				.slipTime = 100,			\
				.transactionTime = 500		\
				}

#define RAIL_WUR_EVENTS (			\
	RAIL_EVENT_CAL_NEEDED			\
	| RAIL_EVENT_TX_PACKET_SENT		\
	| RAIL_EVENT_TX_UNDERFLOW		\
	| RAIL_EVENT_TX_BLOCKED			\
	| RAIL_EVENT_TX_CHANNEL_BUSY	\
	| RAIL_EVENT_TX_ABORTED)


/******************************************************************************
 * Private Static variables
 *****************************************************************************/

static RAIL_CsmaConfig_t csmaTxConf = RAIL_CSMA_CONFIG_WuR_2p4_GHz_OOK_CSMA;

static RAIL_SchedulerInfo_t scheduleTxInfo = RAIL_WUR_SCHEDULE_INFO;

static const RAIL_Events_t events = RAIL_WUR_EVENTS;


RAIL_Handle_t railHandle = NULL;

uint8_t channel = 0;

volatile bool packetTx = true; //go into transfer mode
volatile bool packetRx = false;  //go into receive mode

static uint8_t transmitFIFO[RAIL_TX_FIFO_SIZE] = {0};

static RAILSched_Config_t railSchedState;

static RAIL_Config_t railCfg = {
  .eventsCallback = &RAILCb_Generic,
  .scheduler = &railSchedState,
  .protocol = NULL
};


/******************************************************************************
 * Public Functions
 *****************************************************************************/


int ook_com_startRadio(void)
{

  emberAfCorePrintln("----------- Start of OOK Init! -----------");

  // Initialize Radio
  radioInit();

  // Initialize the PA now that the HFXO is up and the timing is correct
  RAIL_TxPowerConfig_t txPowerConfig = {
#if HAL_PA_2P4_LOWPOWER
    .mode = RAIL_TX_POWER_MODE_2P4_LP,
#else
    .mode = RAIL_TX_POWER_MODE_2P4_HP,
#endif
    .voltage = BSP_PA_VOLTAGE,
    .rampTime = HAL_PA_RAMP,
  };
  emberAfCorePrintln("Configuring RAIL TxPower!");
  if (RAIL_ConfigTxPower(railHandle, &txPowerConfig) != RAIL_STATUS_NO_ERROR) {
	  emberAfCorePrintln("----------- Power conf ERROR! -----------");
	  return -1;
  }

  emberAfCorePrintln("Set RAIL TxPower!");
  RAIL_SetTxPower(railHandle, HAL_PA_POWER);

  emberAfCorePrintln("Prepare RAIL FIFO!");
  int ires = RAIL_SetTxFifo(railHandle, &transmitFIFO[0], 0, RAIL_TX_FIFO_SIZE);
  if(ires != RAIL_TX_FIFO_SIZE){
	  emberAfCorePrintln("----------- FIFO ERROR %d! -----------", ires);
	  return -1;
  }
  emberAfCorePrintln("--------- Configured OOK RAIL! -----------");

  return 0;
}

int ook_com_sendFrame(uint8_t* data, uint8_t len){
	//if a frame is not pending
	if(len > RAIL_TX_FIFO_SIZE){
		emberAfCorePrintln("Frame too large: %d is larger than max frame size %d", len, RAIL_TX_FIFO_SIZE);
		return -1;
	}

	int ires = RAIL_WriteTxFifo(railHandle, data, len, true);
	if(ires != len){
	  emberAfCorePrintln("----------- TxFIFO ERROR %d! -----------", ires);
	  return -1;
	}

	if(RAIL_StartCcaCsmaTx(railHandle, channel, 0, &csmaTxConf, &scheduleTxInfo) == RAIL_STATUS_NO_ERROR){
	  return 0;
	} else {
	  emberAfCorePrintln("----------- Send ERROR %d! ------------", ires);
	  return -1;
	}
}


/******************************************************************************
 * Static Functions
 *****************************************************************************/

static void RAILCb_RadioConfigChanged(RAIL_Handle_t railHandle,
                                      const RAIL_ChannelConfigEntry_t *entry)
{
  // Ensure that the correct PA is configured for use. If it is correct,
  // we don't need to do anything as RAIL library takes care of setting
  // the power level according to channel limits. If the PA needs to change
  // however, the app needs to make that change explicitly and re-set the
  // power.
  RAIL_TxPowerConfig_t txConfig;
  RAIL_GetTxPowerConfig(railHandle, &txConfig);
  if ( entry->baseFrequency > 1000000000UL && txConfig.mode == RAIL_TX_POWER_MODE_SUBGIG ) {
#if HAL_PA_2P4_LOWPOWER
    txConfig.mode = RAIL_TX_POWER_MODE_2P4_LP;
#else
    txConfig.mode = RAIL_TX_POWER_MODE_2P4_HP;
#endif
    RAIL_ConfigTxPower(railHandle, &txConfig);
    RAIL_SetTxPower(railHandle, HAL_PA_POWER);
  } else if ( entry->baseFrequency < 1000000000UL && (txConfig.mode == RAIL_TX_POWER_MODE_2P4_HP
                                                      || txConfig.mode == RAIL_TX_POWER_MODE_2P4_LP)) {
    txConfig.mode = RAIL_TX_POWER_MODE_SUBGIG;
    RAIL_ConfigTxPower(railHandle, &txConfig);
    RAIL_SetTxPower(railHandle, HAL_PA_POWER);
  }
}

void radioInit()
{
  //emberAfCorePrintln("Prepare RAIL handle");
  railHandle = RAIL_Init(&railCfg, NULL);
  if (railHandle == NULL) {
    while (1) ;
  }
  //emberAfCorePrintln("RAIL handle ready!");
  RAIL_ConfigCal(railHandle, RAIL_CAL_ALL_PENDING);
  emberAfCorePrintln("Rail config CAL");
  // Set us to a valid channel for this config and force an update in the main
  // loop to restart whatever action was going on
  RAIL_ConfigChannels(railHandle, channelConfigs[0], RAILCb_RadioConfigChanged);

  RAIL_StateTransitions_t defaultStateTransition = {
    .error = RAIL_RF_STATE_IDLE,
    .success = RAIL_RF_STATE_IDLE
  };

  emberAfCorePrintln("Config RAIL events!");
  RAIL_ConfigEvents(railHandle,
                    events,
                    events);

  RAIL_SetRxTransitions(railHandle, &defaultStateTransition);
  RAIL_SetTxTransitions(railHandle, &defaultStateTransition);

}

/******************************************************************************
 * RAIL OOK Callback Implementation
 *****************************************************************************/
void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
  (void)railHandle;
  if (events & RAIL_EVENT_CAL_NEEDED) {
	// Calibrate if necessary
	RAIL_Calibrate(railHandle, NULL, RAIL_CAL_ALL_PENDING);
  }
  if( events & RAIL_EVENT_TX_BLOCKED){
	RAIL_ResetFifo(railHandle, true, false);
	ook_wur_callback(OOK_WUR_TX_ERROR_FAILED);
	halToggleLed(LED_TX);
  }
  if( events & RAIL_EVENT_TX_CHANNEL_BUSY){
	RAIL_ResetFifo(railHandle, true, false);
	ook_wur_callback(OOK_WUR_TX_ERROR_BUSY);
	halToggleLed(LED_TX);
  }
  if (events & RAIL_EVENT_TX_ABORTED) {
	RAIL_ResetFifo(railHandle, true, false);
	ook_wur_callback(OOK_WUR_TX_ERROR_BUSY);
	halToggleLed(LED_TX);
  }
  if (events & RAIL_EVENT_TX_PACKET_SENT) {
	ook_wur_callback(OOK_WUR_TX_ERROR_SUCCESS);
	halToggleLed(LED_TX);
  }
  if (events & RAIL_EVENT_TX_UNDERFLOW) {
	RAIL_ResetFifo(railHandle, true, false);
	ook_wur_callback(OOK_WUR_TX_ERROR_FAILED);
	halToggleLed(LED_TX);
  }
  RAIL_YieldRadio(railHandle);
}