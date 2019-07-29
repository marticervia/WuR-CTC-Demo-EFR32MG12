/*
 * ook-test.h
 *
 *  Created on: 10 jul. 2019
 *      Author: marti
 */

#ifndef OOK_COM_H_
#define OOK_COM_H_

#include "hal-config.h"

int ook_com_startRadio(void);
int ook_com_sendFrame(uint8_t* data, uint8_t len);

#endif /* OOK_COM_H_ */
