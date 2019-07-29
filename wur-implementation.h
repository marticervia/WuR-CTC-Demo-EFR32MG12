/*
 * wur-implementation.h
 *
 *  Created on: 29 jul. 2019
 *      Author: marti
 */

#ifndef WUR_IMPLEMENTATION_H_
#define WUR_IMPLEMENTATION_H_

void WuRWakeDevice(EmberCoapCode code,
                                         uint8_t *uri,
                                         EmberCoapReadOptions *options,
                                         const uint8_t *payload,
                                         uint16_t payloadLength,
                                         const EmberCoapRequestInfo *info);

void WuRRequestDevice (EmberCoapCode code,
                                         uint8_t *uri,
                                         EmberCoapReadOptions *options,
                                         const uint8_t *payload,
                                         uint16_t payloadLength,
                                         const EmberCoapRequestInfo *info);


#endif /* WUR_IMPLEMENTATION_H_ */
