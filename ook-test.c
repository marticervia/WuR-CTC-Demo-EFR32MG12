/***************************************************************************//**
 * @file
 * @brief Application specific overrides of weak functions defined as part of
 * the test application.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

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
#include "gpiointerrupt.h"
#include "rail_config.h"
#include "bsp.h"

#include "ook-test.h"

// Memory manager configuration
#define MAX_BUFFER_SIZE  256

// Minimum allowed size of the TX FIFO
#define RAIL_TX_FIFO_SIZE 64
#define RAIL_FRAME_SIZE 15

// General application memory sizes
#define APP_MAX_PACKET_LENGTH  (MAX_BUFFER_SIZE - 12) /* sizeof(RAIL_RxPacketInfo_t) == 12) */

/*
#define OOK_TASK_PRIO         6u
#define OOK_TASK_STACK_SIZE   1000
static CPU_STK ookTaskStk[OOK_TASK_STACK_SIZE];
static OS_TCB  ookTaskTCB;
static void    ookAppTask(void *p_arg);
*/
// Prototypes
void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events);
void radioInit();
void gpioCallback(uint8_t pin);

RAIL_Handle_t railHandle = NULL;

typedef struct ButtonArray{
  GPIO_Port_TypeDef   port;
  unsigned int        pin;
} ButtonArray_t;

static volatile RAIL_RxPacketHandle_t packetHandle;
static volatile bool txPacketPending;
static int32_t frame_num = 0;

static volatile int32_t status;

uint8_t channel = 0;
volatile bool packetTx = true; //go into transfer mode
volatile bool packetRx = false;  //go into receive mode

static uint8_t transmitFIFO[RAIL_TX_FIFO_SIZE] = {0};

static uint8_t transmitData[] = {
  0b01010101, //sync word "11" and first byte of 12 bit address "010101010101"
  0b01010010, //last nibble of 12 bit address, type "001" AND PARITY "0"
  0x0C, 	  //length
  0x55, 0x33, 0x0F,
  0x55, 0x33, 0x0F,
  0x55, 0x33, 0x0F,
  0x55, 0x33, 0x0F
};

static RAILSched_Config_t railSchedState;

static RAIL_Config_t railCfg = {
  .eventsCallback = &RAILCb_Generic,
  .scheduler = &railSchedState,
  .protocol = NULL
};

/*
static RAIL_ScheduleTxConfig_t scheduleTx = { .when = 50,
	  	  	  	  	  	  	  	  	  	 .mode = RAIL_TIME_DELAY,
	  	  	  	  	  	  	  	  	  	 .txDuringRx = RAIL_SCHEDULED_TX_DURING_RX_ABORT_TX
	  	  	  	  	  	  	  	  	    };
*/

static RAIL_CsmaConfig_t csmaTxConf = RAIL_CSMA_CONFIG_WuR_2p4_GHz_OOK_CSMA;

static RAIL_SchedulerInfo_t scheduleTxInfo = {
		  .priority = 1,
		  .slipTime = 100,
		  .transactionTime = 500
		  };

static const RAIL_Events_t events = (
		   RAIL_EVENT_CAL_NEEDED
         | RAIL_EVENT_TX_PACKET_SENT
         | RAIL_EVENT_TX_UNDERFLOW
		 | RAIL_EVENT_TX_BLOCKED
		 | RAIL_EVENT_TX_CHANNEL_BUSY
		 | RAIL_EVENT_TX_ABORTED);

#define LED_TX (0)
#define LED_RX (1)

/*
int launch_ook_task(void){
	  RTOS_ERR err;

	  //emberAfCorePrintln("----------- Starting OOK TASK! -----------");
	  //emberAfCorePrintln("Prio %d. Stack size %d words.", OOK_TASK_PRIO, OOK_TASK_STACK_SIZE);
	  //emberAfCorePrintln("Stack watermark %d. Stack pinter 0x%08x.", OOK_TASK_STACK_SIZE / 10, &ookTaskStk[0]);
	  // Create the systemStart task which initializes things
	  OSTaskCreate(&ookTaskTCB,
	               "System Start",
				   ookAppTask,
	               NULL,
				   OOK_TASK_PRIO,
	               &ookTaskStk[0],
				   OOK_TASK_STACK_SIZE / 10,
				   OOK_TASK_STACK_SIZE,
	               0, // Not receiving messages
	               0, // Default time quanta
	               NULL, // No TCB extensions
	               OS_OPT_TASK_STK_CLR | OS_OPT_TASK_STK_CHK,
	               &err);

	  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
	  return RTOS_ERR_NONE;

}
*/

int startOOKRadio(void)
{

  emberAfCorePrintln("----------- Start of OOK Init! -----------");

  // Initialize Radio
  radioInit();
  // Configure RAIL callbacks


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
  emberAfCorePrintln("Config RAIL TxPower!");
  if (RAIL_ConfigTxPower(railHandle, &txPowerConfig) != RAIL_STATUS_NO_ERROR) {
    // Error: The PA could not be initialized due to an improper configuration.
    // Please ensure your configuration is valid for the selected part.
    while (1) ;
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

int sendOOKFrame(void){
	//if a frame is not pending
	if(!txPacketPending){
		frame_num++;
		if(frame_num % 100 == 0){
		  emberAfCorePrintln("------------ Send %ld ! -------------", frame_num);
		}

		int ires = RAIL_WriteTxFifo(railHandle, &transmitData[0], RAIL_FRAME_SIZE, true);
		if(ires != RAIL_FRAME_SIZE){
		  emberAfCorePrintln("----------- TxFIFO ERROR %d! -----------", ires);
		  return -1;
		}

		if(RAIL_StartCcaCsmaTx(railHandle, channel, 0, &csmaTxConf, &scheduleTxInfo) == RAIL_STATUS_NO_ERROR){
		  txPacketPending = true;
		  return 0;
		} else {
		  emberAfCorePrintln("----------- Send ERROR %d! ------------", ires);
		  return -1;
		}
	}else{
		  emberAfCorePrintln("Warning, still waiting for previous frame %d to be sent!", frame_num);
		  return -1;
	}
}


/******************************************************************************
 * Configuration Utility Functions
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
	  emberAfCorePrintln("CAL NEEDED!");
  }
  if( events & RAIL_EVENT_TX_BLOCKED){
	RAIL_ResetFifo(railHandle, true, false);
	txPacketPending = false;
	halToggleLed(LED_TX);
	emberAfCorePrintln("TX_BLOCKED!");
  }
  if( events & RAIL_EVENT_TX_CHANNEL_BUSY){
	RAIL_ResetFifo(railHandle, true, false);
	txPacketPending = false;
	halToggleLed(LED_TX);
	emberAfCorePrintln("CHANNEL_BUSY!");
  }
  if (events & RAIL_EVENT_TX_ABORTED) {
	RAIL_ResetFifo(railHandle, true, false);
	txPacketPending = false;
	halToggleLed(LED_TX);
	emberAfCorePrintln("TX_ABORTED!");
  }
  if (events & RAIL_EVENT_TX_PACKET_SENT) {
	txPacketPending = false;
	halToggleLed(LED_TX);
  }
  if (events & RAIL_EVENT_TX_UNDERFLOW) {
	RAIL_ResetFifo(railHandle, true, false);
	txPacketPending = false;
	halToggleLed(LED_TX);
	emberAfCorePrintln("TX_UNDERFLOW!");
  }
  RAIL_YieldRadio(railHandle);
}
