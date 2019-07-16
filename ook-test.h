/*
 * ook-test.h
 *
 *  Created on: 10 jul. 2019
 *      Author: marti
 */

#ifndef OOK_TEST_H_
#define OOK_TEST_H_

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

int startOOKRadio(void);
int sendOOKFrame(void);
//int launch_ook_task(void);

#endif /* OOK_TEST_H_ */
