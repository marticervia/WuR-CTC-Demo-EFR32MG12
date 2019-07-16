// This file is generated by Simplicity Studio.  Please do not edit manually.
//
//

#ifdef CONFIGURATION_HEADER
  #include CONFIGURATION_HEADER
#endif // CONFIGURATION_HEADER
#include EMBER_AF_API_COAP_DISPATCH

EmberAfCoapDispatchHandler diagnosticAnswerHandler;
EmberAfCoapDispatchHandler emZclHandler;

const EmberAfCoapDispatchEntry emberAfCoapDispatchTable[] = {
  {EMBER_AF_COAP_DISPATCH_METHOD_ANY, "d/da", 4, diagnosticAnswerHandler},
  {EMBER_AF_COAP_DISPATCH_METHOD_ANY, "zcl/", 4, emZclHandler},
  {EMBER_AF_COAP_DISPATCH_METHOD_ANY, ".well-known/core/", 17, emZclHandler},
  {EMBER_AF_COAP_DISPATCH_METHOD_ANY, "rd/", 3, emZclHandler},
  {0, NULL, 0, NULL}, // terminator
};
