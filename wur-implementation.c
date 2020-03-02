/***************************************************************************//**
 * @file
 * @brief
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

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_STACK
#include EMBER_AF_API_HAL
#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif
#include EMBER_AF_API_ZCL_CORE

#include "wur-implementation.h"
#include "stack/ip/ip-address.h"
#include "ook_wur.h"
#include "i2c_wur.h"
#include "wur.h"

// WARNING: This sample application uses fixed network parameters and the well-
// know sensor/sink network key as the master key.  This is done for
// demonstration purposes, so nodes can join without a commissioner (i.e., out-
// of-band commissioning), and so that packets will decrypt automatically in
// Simplicity Studio.  Predefined network parameters only work for a single
// deployment and using predefined keys is a significant security risk.  Real
// devices MUST use random parameters and keys.
//
// These parameters have been chosen to match the border router sample
// application, which will allow this design to perform out of band joining with
// the border router sample application without need for modification.
static const uint8_t preferredChannel = 19;
static const uint8_t networkId[EMBER_NETWORK_ID_SIZE] = "precommissioned";
static const EmberPanId panId = 0x1076;

static const EmberIpv6Prefix ulaPrefix = {
  { 0xFD, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
};

static const uint8_t extendedPanId[EXTENDED_PAN_ID_SIZE] = {
  0xc6, 0xef, 0xe1, 0xb4, 0x5f, 0xc7, 0x8e, 0x4f
};

static const EmberKeyData masterKey = {
  { 0x65, 0x6D, 0x62, 0x65, 0x72, 0x20, 0x45, 0x4D,
    0x32, 0x35, 0x30, 0x20, 0x63, 0x68, 0x69, 0x70, },
};

EmberEventControl ezModeEventControl;
static bool waitingForReleased = false;

enum {
  INITIAL                 = 0,
  RESUME_NETWORK          = 1,
  COMMISSION_NETWORK      = 2,
  JOIN_COMMISSIONED       = 3,
  STEADY                  = 5,
  RESET_NETWORK_STATE     = 6,
};
static uint8_t state = INITIAL;
EmberEventControl stateEventControl;
static void setNextState(uint8_t nextState);

void emberAfNetworkStatusCallback(EmberNetworkStatus newNetworkStatus,
                                  EmberNetworkStatus oldNetworkStatus,
                                  EmberJoinFailureReason reason)
{
  // This callback is called whenever the network status changes, like when
  // we finish joining to a network or when we lose connectivity.  If we have
  // no network, we try joining to one.  If we have a saved network, we try to
  // resume operations on that network.  When we are joined and attached to the
  // network, we are in the steady state and wait for input from other nodes.

  emberEventControlSetInactive(stateEventControl);

  switch (newNetworkStatus) {
    case EMBER_NO_NETWORK:
      if (reason != EMBER_JOIN_FAILURE_REASON_NONE) {
        emberAfCorePrintln("ERR: Joining failed: 0x%x", reason);
      }
      setNextState(COMMISSION_NETWORK);
      break;
    case EMBER_SAVED_NETWORK:
      setNextState(RESUME_NETWORK);
      break;
    case EMBER_JOINING_NETWORK:
      // Wait for either the "attaching" or "no network" state.
      break;
    case EMBER_JOINED_NETWORK_ATTACHING:
      // Wait for the "attached" state.
      if (oldNetworkStatus == EMBER_JOINED_NETWORK_ATTACHED) {
        emberAfCorePrintln("ERR: Lost connection to network");
      }
      break;
    case EMBER_JOINED_NETWORK_ATTACHED:
      emberAfCorePrintln("Attached");
      state = STEADY;
      break;
    case EMBER_JOINED_NETWORK_NO_PARENT:
      // We always join as a router, so we should never end up in the "no parent"
      // state.
      assert(false);
      break;
    default:
      assert(false);
      break;
  }
}

static void resumeNetwork(void)
{
  emberAfCorePrintln("Resuming...");
  emberResumeNetwork();
}

void emberResumeNetworkReturn(EmberStatus status)
{
  // This return indicates whether the stack is going to attempt to resume.  If
  // so, the result is reported later as a network status change.  If we cannot
  // even attempt to resume, we just give up and reset our network state, which
  // will trigger join instead.

  if (status != EMBER_SUCCESS) {
    emberAfCorePrintln("ERR: Unable to resume: 0x%x", status);
    setNextState(RESET_NETWORK_STATE);
  }
}

static void commissionNetwork(void)
{
  emberAfCorePrintln("Commissioning...");
  emberCommissionNetwork(preferredChannel,
                         0,                 // fallback channel mask - ignored
                         networkId,
                         sizeof(networkId),
                         panId,
                         ulaPrefix.bytes,
                         extendedPanId,
                         &masterKey,
                         0);                // key sequence
}

void emberCommissionNetworkReturn(EmberStatus status)
{
  // This return indicates whether the network was commissioned.  If so, we can
  // proceed to joining.  Otherwise, we just give up and reset our network
  // state, which will trigger a fresh join attempt.

  if (status == EMBER_SUCCESS) {
    setNextState(JOIN_COMMISSIONED);
  } else {
    emberAfCorePrintln("ERR: Unable to commission: 0x%x", status);
    setNextState(RESET_NETWORK_STATE);
  }
}

static void joinCommissioned(void)
{
  emberAfCorePrintln("Joining...");
  emberJoinCommissioned(3,            // radio tx power
                        EMBER_ROUTER,
                        true);        // require connectivity
}

void emberJoinNetworkReturn(EmberStatus status)
{
  // This return indicates whether the stack is going to attempt to join.  If
  // so, the result is reported later as a network status change.  Otherwise,
  // we just give up and reset our network state, which will trigger a fresh
  // join attempt.

  if (status != EMBER_SUCCESS) {
    emberAfCorePrintln("ERR: Unable to join: 0x%x", status);
    setNextState(RESET_NETWORK_STATE);
  }
}

static void resetNetworkState(void)
{
  emberAfCorePrintln("Resetting...");
  emberResetNetworkState();
}

void emberResetNetworkStateReturn(EmberStatus status)
{
  // If we ever leave the network, we go right back to joining again.  This
  // could be triggered by an external CLI command.

  if (status != EMBER_SUCCESS) {
    emberAfCorePrintln("ERR: Unable to reset: 0x%x", status);
  }
}

void emberBackbonePrefixHandler(const uint8_t *prefix,
                                uint8_t prefixLengthInBits,
                                bool available)
{
  uint8_t prefixLengthInBytes = (prefixLengthInBits >> 3)
                                + ((prefixLengthInBits & 0x07) ? 1 : 0);

  emberAfAppPrintln("bbr prefix: ");
  emberAfAppDebugExec(emberAfPrintIpv6Prefix((const EmberIpv6Address *) prefix, prefixLengthInBits));
  emberAfAppPrintln(" | %p", available ? "available" : "unavailable");
  if (available) {
    // Choose a unique DUA every time.
    EmberIpv6Address dua = { { 0 } };
    MEMCOPY(dua.bytes, prefix, prefixLengthInBytes);
    MEMCOPY(dua.bytes + 8, emMeshLocalIdentifier, 8);
    dua.bytes[15] += 1;
    emberRequestDomainUniqueAddress(dua.bytes, EMBER_IPV6_BITS);
  }
}

void emberMulticastListenerHandler(const EmberIpv6Address * multicastGroupAddress)
{
  emberAfAppPrint("Subscribed to off-mesh multicast group ");
  emberAfAppDebugExec(emberAfPrintIpv6Address(multicastGroupAddress));
  emberAfAppPrintln("");
}


void emberZclPostAttributeChangeCallback(EmberZclEndpointId_t endpointId,
                                         const EmberZclClusterSpec_t *clusterSpec,
                                         EmberZclAttributeId_t attributeId,
                                         const void *buffer,
                                         size_t bufferLength)
{
  // This callback is called whenever an attribute changes.  When the OnOff
  // attribute changes, we toggle our LED accordingly.

  if (emberZclAreClusterSpecsEqual(&emberZclClusterOnOffServerSpec, clusterSpec)
      && attributeId == EMBER_ZCL_CLUSTER_ON_OFF_SERVER_ATTRIBUTE_ON_OFF) {
    bool on = *((bool *)buffer);
    (on ? halSetLed : halClearLed)(BOARD_ACTIVITY_LED);
  }
}

void emberZclGetDefaultReportingConfigurationCallback(EmberZclEndpointId_t endpointId,
                                                      const EmberZclClusterSpec_t *clusterSpec,
                                                      EmberZclReportingConfiguration_t *configuration)
{
  // We have reportable attributes in the On/Off and Level Control servers.

  if (emberZclAreClusterSpecsEqual(&emberZclClusterOnOffServerSpec,
                                   clusterSpec)) {
    configuration->minimumIntervalS =  1;
    configuration->maximumIntervalS = 60;
  } else if (emberZclAreClusterSpecsEqual(&emberZclClusterLevelControlServerSpec,
                                          clusterSpec)) {
    configuration->minimumIntervalS = 10;
    configuration->maximumIntervalS = 60;
  }
}

void emberZclGetDefaultReportableChangeCallback(EmberZclEndpointId_t endpointId,
                                                const EmberZclClusterSpec_t *clusterSpec,
                                                EmberZclAttributeId_t attributeId,
                                                void *buffer,
                                                size_t bufferLength)
{
}

void emberZclIdentifyServerStartIdentifyingCallback(uint16_t identifyTimeS)
{
  // This callback is called whenever the endpoint should identify itself.  The
  // identification procedure is application specific, and could be implemented
  // by blinking an LED, playing a sound, or displaying a message.

  emberAfCorePrintln("Identifying...");
}

void emberZclIdentifyServerStopIdentifyingCallback(void)
{
  // This callback is called whenever the endpoint should stop identifying
  // itself.

  emberAfCorePrintln("Identified");
}

void halButtonIsr(uint8_t button, uint8_t state)
{
  // Buttons can be used to bind to a switch.  Pressing both buttons will cause
  // the node to start EZ Mode commissioning to find other nodes on the
  // network.  If other nodes implement clusters that correspond to ours (e.g.,
  // we have the On/Off server and they have the On/Off client) and they are
  // also in EZ Mode, we will create a binding to them and they will create a
  // binding to us.

  bool thisPressed = (state == BUTTON_PRESSED);
  bool otherPressed = (halButtonState(button == BUTTON0 ? BUTTON1 : BUTTON0)
                       == BUTTON_PRESSED);
  waitingForReleased = waitingForReleased && (thisPressed || otherPressed);

  if (thisPressed && otherPressed && !waitingForReleased) {
    emberEventControlSetActive(ezModeEventControl);
    waitingForReleased = true;
  }
}

void ezModeEventHandler(void)
{
  emberEventControlSetInactive(ezModeEventControl);
  emberZclStartEzMode();
}

static app_ctxt_t app_ctxt;
static test_ctxt_t test_ctxt;

static void init_test_context(test_ctxt_t* ctxt){
    ctxt->start_timestamp = get_timestamp_ms();
    ctxt->finish_timestamp = 0;
    ctxt->current_frame = 0;
    ctxt->total_frames = TOTAL_TEST_FRAMES;
    ctxt->KO_frames= 0;
    ctxt->OK_frames = 0;
    ctxt->test_status = TEST_IN_PROGRESS;
    memset(ctxt->failure_reason, 0, TEST_REASON_LEN);
}

static void generate_test_frame(uint8_t* buffer, uint16_t req_len_bytes){
    esp_fill_random(buffer, req_len_bytes);
}

static void fail_test_context(test_ctxt_t* ctxt, char* reason, uint32_t fail_timestamp){
    ctxt->test_status = TEST_FAILED;
    strcpy(ctxt->failure_reason, reason);
    ctxt->finish_timestamp = fail_timestamp;
    printf("[%d]: Finished test with failure on frame: %d/%d !\n", fail_timestamp, test_ctxt.current_frame, test_ctxt.total_frames);
    printf("[%d]: Test run in %d milliseconds!\n", fail_timestamp, test_ctxt.finish_timestamp - test_ctxt.start_timestamp);
    app_ctxt.app_status = APP_IDLE;

}

static void aprove_test_context(test_ctxt_t* ctxt, uint32_t success_timestamp){
    ctxt->test_status = TEST_FINISHED;
    ctxt->finish_timestamp = success_timestamp;
    printf("[%d]: Finished test with results: %d/%d !\n", success_timestamp, test_ctxt.OK_frames, test_ctxt.total_frames);
    printf("[%d]: Test run in %d milliseconds!\n", success_timestamp, test_ctxt.finish_timestamp - test_ctxt.start_timestamp);
    uint32_t bitrate = (test_ctxt.OK_frames * TEST_FRAME_SIZE * 8* 1000) / ((test_ctxt.finish_timestamp - test_ctxt.start_timestamp));
    printf("[%d]: Calculated bitrate is: %d !\n", success_timestamp, bitrate);
    app_ctxt.app_status = APP_IDLE;
}


static void update_text_context(test_ctxt_t* ctxt, bool OK_result){
    ctxt->current_frame++;
    if(OK_result){
        ctxt->OK_frames++;
    }
    else{
        ctxt->KO_frames++;
    }
    if(ctxt->current_frame == ctxt->total_frames){
        aprove_test_context(ctxt, get_timestamp_ms());
    }
}

/* wur related handlers */
void WuRTestStartDevice(EmberCoapCode code,
					 uint8_t *uri,
					 EmberCoapReadOptions *options,
					 const uint8_t *payload,
					 uint16_t payloadLength,
					 const EmberCoapRequestInfo *info){
    printf("Received /test/start request");

    if(test_ctxt.test_status == TEST_IN_PROGRESS){
		emberCoapRespondWithCode(info, EMBER_COAP_CODE_412_PRECONDITION_FAILED);
		return;
    }

    init_test_context(&test_ctxt);
    app_ctxt.app_status = TEST_SENDING_WAKE;
	emberCoapRespondWithCode(info, EMBER_COAP_CODE_203_VALID);
}

void WuRTestStatusDevice(EmberCoapCode code,
					 uint8_t *uri,
					 EmberCoapReadOptions *options,
					 const uint8_t *payload,
					 uint16_t payloadLength,
					 const EmberCoapRequestInfo *info){

	uint8_t resp_payload[TEST_STATUS_PAYLOAD_LEN];
	uint16_t u16_tmp_val;
	uint32_t u32_tmp_val;
    printf("Received /test/status request");

    /*
     * 1. Copy the test status.
     */
    resp_payload[0] = (uint8_t) test_ctxt.test_status;
    /*
     * 2. Copy the test current frame.
     */
    u16_tmp_val = htons(test_ctxt.current_frame);
    memcpy(resp_payload, &u16_tmp_val, TEST_STATUS_PAYLOAD_OFFSET_CURR_FRAME);
    /*
     * 3. Copy the total test frame.
     */
    u16_tmp_val = htons(test_ctxt.total_frames);
    memcpy(resp_payload, &u16_tmp_val, TEST_STATUS_PAYLOAD_OFFSET_TOTAL_FRAME);
    /*
     * 4. Copy the OK frame count.
     */
    u16_tmp_val = htons(test_ctxt.OK_frames);
    memcpy(resp_payload, &u16_tmp_val, TEST_STATUS_PAYLOAD_OFFSET_OK_FRAME);
    /*
     * 5. Copy the KO frame count.
     */
    u16_tmp_val = htons(test_ctxt.KO_frames);
    memcpy(resp_payload, &u16_tmp_val, TEST_STATUS_PAYLOAD_OFFSET_KO_FRAME);
    /*
     * 6. Copy the timestamp.
     */
    if(test_ctxt.finish_timestamp != 0){
    	u32_tmp_val = htonl(test_ctxt.finish_timestamp - test_ctxt.start_timestamp);
    }
    else{
    	u32_tmp_val = 0;
    }
    memcpy(resp_payload, &u32_tmp_val, TEST_STATUS_PAYLOAD_OFFSET_RUNTIME);

	 emberCoapRespondWithPayload(&app_ctxt.app_request_info, EMBER_COAP_CODE_203_VALID,
			 resp_payload, TEST_STATUS_PAYLOAD_LEN);
}


void WuRWakeDevice(EmberCoapCode code,
					 uint8_t *uri,
					 EmberCoapReadOptions *options,
					 const uint8_t *payload,
					 uint16_t payloadLength,
					 const EmberCoapRequestInfo *info){

	uint32_t current_timestamp = halCommonGetInt32uMillisecondTick();
	emberAfCorePrintln("[%d]: Got Wake Device REQ!", current_timestamp);

	if(app_ctxt.app_status != APP_IDLE){
		emberAfCorePrintln("[%d]: Refusing Wake Device REQ, already busy!", current_timestamp);
		emberCoapRespondWithCode(info, EMBER_COAP_CODE_412_PRECONDITION_FAILED);
		return;
	}

	if((payloadLength > MAX_APP_DATA_BUF) || (payloadLength < MIN_WAKE_REQ_LEN)){
		emberAfCorePrintln("[%d]: Payload of %d bytes disallowed for WAKE!", current_timestamp, payloadLength);
		emberCoapRespondWithCode(info, EMBER_COAP_CODE_400_BAD_REQUEST);
		return;
	}

	/* retrieve data*/
	memcpy(app_ctxt.app_data_buf, payload, payloadLength);
	app_ctxt.app_data_buf_len = payloadLength;

	/* change status and notify main loop */
	emberSaveRequestInfo(info, &(app_ctxt.app_request_info));
	app_ctxt.app_status = APP_SENDING_WAKE;
}

void WuRRequestDevice(EmberCoapCode code,
						 uint8_t *uri,
						 EmberCoapReadOptions *options,
						 const uint8_t *payload,
						 uint16_t payloadLength,
						 const EmberCoapRequestInfo *info){
	uint32_t current_timestamp = halCommonGetInt32uMillisecondTick();
	emberAfCorePrintln("[%d]: Got Data Device REQ!", current_timestamp);

	if(app_ctxt.app_status != APP_IDLE){
		emberAfCorePrintln("[%d]: Refusing DATA to Device REQ, already busy!", current_timestamp);
		emberCoapRespondWithCode(info, EMBER_COAP_CODE_412_PRECONDITION_FAILED);
		return;
	}

	if((payloadLength > MAX_APP_DATA_BUF) || (payloadLength < MIN_DATA_REQ_LEN)){
		emberAfCorePrintln("[%d]: Payload of %d bytes disallowed for DATA!", current_timestamp, payloadLength);
		emberCoapRespondWithCode(info, EMBER_COAP_CODE_400_BAD_REQUEST);
		return;
	}

	/* retrieve data*/
	memcpy(app_ctxt.app_data_buf, payload, payloadLength);
	app_ctxt.app_data_buf_len = payloadLength;

	/* change status and notify main loop */
	emberSaveRequestInfo(info, &(app_ctxt.app_request_info));
	app_ctxt.app_status = APP_SENDING_DATA;
}


static inline uint16_t _getuint16t(uint8_t* buff){
	uint16_t wur_addr;
	memcpy(&wur_addr, buff, sizeof(uint16_t));
	wur_addr = NTOHS(wur_addr);
	return wur_addr;
}

static inline void _setint16t(uint8_t* buff, int16_t num){
	num = HTONS(num);
	memcpy(buff, &num, sizeof(int16_t));
}

static void _respondWithError(app_trans_result_t res){
	const uint8_t res_payload[2] = {0};
	_setint16t((uint8_t*) res_payload, res);
	 emberCoapRespondWithPayload(&app_ctxt.app_request_info, EMBER_COAP_CODE_500_INTERNAL_SERVER_ERROR,
			res_payload, 2);
}

static void _respondWithPayload(uint8_t* payload, uint8_t payload_len){
	if(payload_len > 0){
		 emberCoapRespondWithPayload(&app_ctxt.app_request_info, EMBER_COAP_CODE_203_VALID,
				 &payload[WUR_PAYLOAD_OFFSET], payload_len - WUR_PAYLOAD_OFFSET);
	}
	emberCoapRespondWithCode(&app_ctxt.app_request_info, EMBER_COAP_CODE_203_VALID);
}


static void _printBuffer(uint8_t* res, uint8_t res_length){
	uint16_t i;
	emberAfCorePrintln("Buffer is:");

	for(i=0; i < (res_length -1); i++){
		emberAfCorePrint("%01X:", res[i]);
	}
	emberAfCorePrintln("%01X", res[i]);
}

static void _wur_tx_cb(wur_tx_res_t tx_res){
	uint32_t current_timestamp = halCommonGetInt32uMillisecondTick();

	switch(app_ctxt.app_status){
		case APP_WAITING_WAKE:
		case APP_WAITING_DATA:
			if(tx_res == WUR_ERROR_TX_OK){
				if(app_ctxt.app_status == APP_WAITING_WAKE){
					//emberAfCorePrintln("[%d]: Going to respond to Wake!", current_timestamp);
					app_ctxt.app_status = APP_RESPONDING_WAKE;

				}
				else{
					//emberAfCorePrintln("[%d]: Going to respond to Data!", current_timestamp);
					app_ctxt.app_status = APP_RESPONDING_DATA;
				}
				memset(app_ctxt.app_data_buf, 0, MAX_APP_DATA_BUF);
				app_ctxt.app_data_buf_len = 0;
			}
			else if((tx_res ==  WUR_ERROR_TX_ACK_DATA_TIMEOUT)
					|| (tx_res ==  WUR_ERROR_TX_ACK_WAKE_TIMEOUT)){
				emberAfCorePrintln("[%d]: Received Timeout, going to idle!", current_timestamp);
				app_ctxt.app_status = APP_IDLE;
				_respondWithError(APP_TRANS_KO_TIMEOUT);
			}
			else{
				emberAfCorePrintln("[%d]: Received Error, going to idle!", current_timestamp);
				app_ctxt.app_status = APP_IDLE;
				_respondWithError(APP_TRANS_KO_TIMEOUT);
			}
			break;
        case TEST_WAITING_WAKE:
            if(tx_res == WUR_ERROR_TX_OK){
                app_ctxt.app_status = TEST_GENERATE_FRAME;
            }
            else{
                app_ctxt.app_status = TEST_COMPLETE_FAILURE;
            }
            break;
        case TEST_WAIT_FRAME:
            if(tx_res == WUR_ERROR_TX_OK){
                //printf("[%d]: Received ACK!\n", current_timestamp);
                app_ctxt.app_status = TEST_COMPLETE_OK_FRAME;
            }
            else if(tx_res == WUR_ERROR_TX_ACK_DATA_TIMEOUT){
                //printf("[%d]: Received NACK!\n", current_timestamp);
                app_ctxt.app_status = TEST_COMPLETE_KO_FRAME;
            }
            else{
                //printf("[%d]: Received Error!\n", current_timestamp);
                app_ctxt.app_status = TEST_COMPLETE_FAILURE;
            }
            memset(app_ctxt.app_data_buf, 0, MAX_APP_DATA_BUF);
            app_ctxt.app_data_buf_len = 0;
            break;
		default:
			emberAfCorePrintln("[%d]: Received ACK while not waiting. Is this an error?!", current_timestamp);
			break;
	}
}


static void _wur_rx_cb(wur_rx_res_t rx_res, uint8_t* rx_bytes, uint8_t rx_bytes_len){
	uint32_t current_timestamp = halCommonGetInt32uMillisecondTick();

	//emberAfCorePrintln("[%d]: Received response with status %d!", current_timestamp, rx_res);
	//_printBuffer(rx_bytes, rx_bytes_len);
	if(rx_bytes_len > MAX_APP_DATA_BUF){
		emberAfCorePrintln("[%d]: Received response above max frame size %d!", current_timestamp);
	}
	memcpy(app_ctxt.app_data_buf, rx_bytes, rx_bytes_len);
	app_ctxt.app_data_buf_len = rx_bytes_len;
}


void WuRInitApp(void){
	wur_init(WUR_ADDR);

	memset(&app_ctxt.app_request_info, 0, sizeof(EmberCoapRequestInfo));
	app_ctxt.app_status = APP_IDLE;
	memset(app_ctxt.app_data_buf, 0, MAX_APP_DATA_BUF);
	app_ctxt.app_data_buf_len = 0;
	wur_set_tx_cb(_wur_tx_cb);
	wur_set_rx_cb(_wur_rx_cb);
}

uint8_t test_data_buf[TEST_FRAME_SIZE];

void WuRAppTick(void){

	uint32_t current_timestamp = halCommonGetInt32uMillisecondTick();
	uint16_t wur_addr, wake_ms;
	wur_tx_res_t tx_res;

	wur_tick(current_timestamp);

	switch(app_ctxt.app_status){
		case APP_IDLE:
			if(current_timestamp % 10000 == 0){
				emberAfCorePrintln("[%d]: Device IDLE", current_timestamp);
			}
			break;
		case APP_SENDING_WAKE:
			//emberAfCorePrintln("[%d]: Sending Wake Device REQ!", current_timestamp);
			wur_addr = _getuint16t(app_ctxt.app_data_buf);
			wur_addr = wur_addr & 0x03FF;
			wake_ms = _getuint16t(&app_ctxt.app_data_buf[PAYLOAD_OFFSET]);

			tx_res = wur_send_wake(wur_addr, wake_ms);
			if(tx_res != WUR_ERROR_TX_OK){
				emberAfCorePrintln("[%d]: Failure to send Wake Device REQ!", current_timestamp);
				_respondWithError(APP_TRANS_KO_TX);
				app_ctxt.app_status = APP_IDLE;
				break;
			}
			app_ctxt.app_status = APP_WAITING_WAKE;
			break;
		case APP_SENDING_DATA:
			//emberAfCorePrintln("[%d]: Sending Data to Device REQ!", current_timestamp);
			wur_addr = _getuint16t(app_ctxt.app_data_buf);
			wur_addr = wur_addr & 0x03FF;
			tx_res = wur_send_data(wur_addr, &app_ctxt.app_data_buf[PAYLOAD_OFFSET], app_ctxt.app_data_buf_len - PAYLOAD_OFFSET, false, -1);
			if(tx_res != WUR_ERROR_TX_OK){
				emberAfCorePrintln("[%d]: Failure to send Data to Device REQ!", current_timestamp);
				_respondWithError(APP_TRANS_KO_TX);
				app_ctxt.app_status = APP_IDLE;
				break;
			}
			app_ctxt.app_status = APP_WAITING_DATA;
			break;
		case APP_WAITING_WAKE:
		case APP_WAITING_DATA:
			/* wait for the OK/KO Tx callback to change the state*/
			if(current_timestamp % 500 == 0){
				emberAfCorePrintln("[%d]: Device Waiting ACK", current_timestamp);
			}
			break;
		case APP_RESPONDING_WAKE:
			//emberAfCorePrintln("[%d]: Sending Response to Wake Device REQ!", current_timestamp);
			_respondWithPayload(NULL, 0);
			app_ctxt.app_status = APP_IDLE;
			break;
		case APP_RESPONDING_DATA:
			//emberAfCorePrintln("[%d]: Sending Response to Data to Device REQ!", current_timestamp);
			_respondWithPayload(app_ctxt.app_data_buf, app_ctxt.app_data_buf_len );
			app_ctxt.app_status = APP_IDLE;
			break;

        case TEST_SENDING_WAKE:
            //printf("[%d]: Sending Test Wake Device REQ!\n", current_timestamp);
            wur_addr = TEST_ADDR & (0x03FF);
            wake_ms = TEST_WAKE_INTERVAL;

            app_ctxt.app_status = TEST_WAITING_WAKE;
            tx_res = wur_send_wake(wur_addr, wake_ms);
            if(tx_res != WUR_ERROR_TX_OK){
                printf("[%d]: Failure to send Wake Device REQ!\n", current_timestamp);
                _respondWithError(APP_TRANS_KO_TX);
                app_ctxt.app_status = TEST_COMPLETE_FAILURE;
                break;
            }
            //printf("[%d]: Sent Test Wake Device REQ!\n", current_timestamp);
            break;

        case TEST_WAITING_WAKE:
            //printf("Waiting frame!");
            break;


        case TEST_GENERATE_FRAME:
            //printf("Generating frame!");
            generate_test_frame(test_data_buf, TEST_FRAME_SIZE);
            app_ctxt.app_status = TEST_SEND_FRAME;
            break;

        case TEST_SEND_FRAME:
            //printf("[%d]: Sending Data to Device test, frame %d/%d!\n", current_timestamp, test_ctxt.current_frame, test_ctxt.total_frames);
            wur_addr = TEST_ADDR & (0x03FF);
            app_ctxt.app_status = TEST_WAIT_FRAME;
            tx_res = wur_send_data(wur_addr, (uint8_t*)&test_data_buf, TEST_FRAME_SIZE, false, -1);
            if(tx_res != WUR_ERROR_TX_OK){
                printf("[%d]: Failure to send Data to Device REQ!\n", current_timestamp);
                update_text_context(&test_ctxt, false);
                app_ctxt.app_status = TEST_GENERATE_FRAME;
            }
            //printf("[%d]: Sent Test Data Device REQ!\n", current_timestamp);
            break;

        case TEST_WAIT_FRAME:
            //printf("Wait Frame!");
            break;

        case TEST_COMPLETE_OK_FRAME:
            //printf("Frame sent OK!");
            update_text_context(&test_ctxt, true);
            if(test_ctxt.test_status == TEST_IN_PROGRESS){
                //printf("Prepare next frame!");
                app_ctxt.app_status = TEST_GENERATE_FRAME;
            }
            break;
        case TEST_COMPLETE_KO_FRAME:
            //printf("Frame sent KO!");
            update_text_context(&test_ctxt, false);
            if(test_ctxt.test_status == TEST_IN_PROGRESS){
                //printf("Prepare next frame!");
                app_ctxt.app_status = TEST_GENERATE_FRAME;
            }
            break;
        case TEST_COMPLETE_FAILURE:
            //printf("Test failure!");
            fail_test_context(&test_ctxt, "Error while taking the test.\n", current_timestamp);
            app_ctxt.app_status = APP_IDLE;
            break;
		default:
			break;
	}
}



void stateEventHandler(void)
{
  emberEventControlSetInactive(stateEventControl);

  switch (state) {
    case RESUME_NETWORK:
      resumeNetwork();
      break;
    case COMMISSION_NETWORK:
      commissionNetwork();
      break;
    case JOIN_COMMISSIONED:
      joinCommissioned();
      break;
    case RESET_NETWORK_STATE:
      resetNetworkState();
      break;
    default:
      assert(false);
      break;
  }
}

static void setNextState(uint8_t nextState)
{
  state = nextState;
  emberEventControlSetActive(stateEventControl);
}
