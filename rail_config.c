/***************************************************************************//**
 * @brief RAIL Configuration
 * @details
 *   WARNING: Auto-Generated Radio Config  -  DO NOT EDIT
 *   Radio Configurator Version: 5.0.1
 *   RAIL Adapter Version: 2.4.10
 *   RAIL Compatibility: 2.x
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "em_common.h"
#include "rail_config.h"

uint32_t RAILCb_CalcSymbolRate(RAIL_Handle_t railHandle)
{
  (void) railHandle;
  return 0U;
}

uint32_t RAILCb_CalcBitRate(RAIL_Handle_t railHandle)
{
  (void) railHandle;
  return 0U;
}

void RAILCb_ConfigFrameTypeLength(RAIL_Handle_t railHandle,
                                  const RAIL_FrameType_t *frameType)
{
  (void) railHandle;
  (void) frameType;
}

static const uint8_t irCalConfig[] = {
  25, 63, 1, 6, 4, 16, 1, 0, 0, 1, 1, 6, 0, 16, 39, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0
};

static RAIL_ChannelConfigEntryAttr_t channelConfigEntryAttr = {
  { 0xFFFFFFFFUL }
};

static const uint32_t phyInfo[] = {
  5UL,
  0x00000000UL, // 0.0
  (uint32_t) NULL,
  (uint32_t) irCalConfig,
#ifdef RADIO_CONFIG_ENABLE_TIMING
  (uint32_t) &Channel_Group_1_timing,
#else
  (uint32_t) NULL,
#endif
  0x00000000UL,
  56250000UL,
  5250000UL,
  250000UL,
  (1UL << 8) | 1UL,
  0x02006147UL,
};

uint32_t dynamicSlicerTable0Config4[] = {
  0x1E0000E0UL,
  (uint32_t) 0UL,
};

uint32_t dynamicSlicerTable0Config3[] = {
  0x140000E0UL,
  (uint32_t) &dynamicSlicerTable0Config4,
};

uint32_t dynamicSlicerTable0Config2[] = {
  0x0E0000D3UL,
  (uint32_t) &dynamicSlicerTable0Config3,
};

uint32_t dynamicSlicerTable0Config1[] = {
  0x0A0000CCUL,
  (uint32_t) &dynamicSlicerTable0Config2,
};

uint32_t dynamicSlicerTable0Config0[] = {
  0x060000C7UL,
  (uint32_t) &dynamicSlicerTable0Config1,
};

const uint32_t WuR_OOK_modemConfigBase[] = {
  0x01051FECUL, (uint32_t) &dynamicSlicerTable0Config0,
  /*    1FF0 */ 0x0037003FUL,
  /*    1FF4 */ 0x00000002UL,
  /*    1FF8 */ (uint32_t) &phyInfo,
  /*    1FFC */ 0x00000000UL,
  0x00020004UL, 0x00038301UL,
  /*    0008 */ 0x00000102UL,
  0x00020018UL, 0x00000000UL,
  /*    001C */ 0x00000003UL,
  0x00070028UL, 0x00000000UL,
  /*    002C */ 0x00000000UL,
  /*    0030 */ 0x00000000UL,
  /*    0034 */ 0x00000000UL,
  /*    0038 */ 0x00000000UL,
  /*    003C */ 0x00000000UL,
  /*    0040 */ 0x000007A4UL,
  0x00010048UL, 0x00000000UL,
  0x00020054UL, 0x00000000UL,
  /*    0058 */ 0x00000000UL,
  0x000400A0UL, 0x00004803UL,
  /*    00A4 */ 0x00004CFFUL,
  /*    00A8 */ 0x00004903UL,
  /*    00AC */ 0x00004DFFUL,
  0x00012000UL, 0x000007A0UL,
  0x00012010UL, 0x00000000UL,
  0x00012018UL, 0x000000E0UL,
  0x00013008UL, 0x0100AC13UL,
  0x00023030UL, 0x00106147UL,
  /*    3034 */ 0x00000001UL,
  0x00013040UL, 0x00000000UL,
  0x000140A0UL, 0x0F00277AUL,
  0x000140B8UL, 0x00F3C000UL,
  0x000140F4UL, 0x00001020UL,
  0x00024134UL, 0x00000880UL,
  /*    4138 */ 0x000087E6UL,
  0x00024140UL, 0x0088008FUL,
  /*    4144 */ 0x4D52E6C0UL,
  0x00044160UL, 0x00000000UL,
  /*    4164 */ 0x00000000UL,
  /*    4168 */ 0x00000006UL,
  /*    416C */ 0x00000006UL,
  0x00086014UL, 0x00000010UL,
  /*    6018 */ 0x00000180UL,
  /*    601C */ 0x0601C20FUL,
  /*    6020 */ 0x00005000UL,
  /*    6024 */ 0x00080000UL,
  /*    6028 */ 0x03000000UL,
  /*    602C */ 0x20000000UL,
  /*    6030 */ 0x00000000UL,
  0x00066050UL, 0x00FF1320UL,
  /*    6054 */ 0x00000C41UL,
  /*    6058 */ 0x000C0000UL,
  /*    605C */ 0x000E0001UL,
  /*    6060 */ 0x0000D555UL,
  /*    6064 */ 0x0000FF00UL,
  0x000C6078UL, 0x13C00E0CUL,
  /*    607C */ 0x00000000UL,
  /*    6080 */ 0x00000330UL,
  /*    6084 */ 0x00000000UL,
  /*    6088 */ 0x00000000UL,
  /*    608C */ 0x00000000UL,
  /*    6090 */ 0x00000000UL,
  /*    6094 */ 0x00000000UL,
  /*    6098 */ 0x00000000UL,
  /*    609C */ 0x00000000UL,
  /*    60A0 */ 0x00000000UL,
  /*    60A4 */ 0x00000000UL,
  0x000760E4UL, 0x8C2E0080UL,
  /*    60E8 */ 0x00000000UL,
  /*    60EC */ 0x07830464UL,
  /*    60F0 */ 0x3AC81388UL,
  /*    60F4 */ 0x0006209CUL,
  /*    60F8 */ 0x00206100UL,
  /*    60FC */ 0x208556B7UL,
  0x00036104UL, 0x001247A9UL,
  /*    6108 */ 0x00003020UL,
  /*    610C */ 0x0000BB88UL,
  0x00016120UL, 0x00000000UL,
  0x10017014UL, 0x0007F800UL,
  0x30017014UL, 0x00000200UL,
  0x00067018UL, 0x00000300UL,
  /*    701C */ 0x844A0060UL,
  /*    7020 */ 0x00000000UL,
  /*    7024 */ 0x00000082UL,
  /*    7028 */ 0x01800000UL,
  /*    702C */ 0x000000D5UL,
  0x00027048UL, 0x00003D3CUL,
  /*    704C */ 0x000019BCUL,
  0x00037070UL, 0x00800105UL,
  /*    7074 */ 0x00083005UL,
  /*    7078 */ 0x00552300UL,
  0xFFFFFFFFUL,
};

const RAIL_ChannelConfigEntry_t WuR_OOK_channels[] = {
  {
    .phyConfigDeltaAdd = NULL,
    .baseFrequency = 2450000000,
    .channelSpacing = 1000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 0,
    .channelNumberEnd = 20,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &channelConfigEntryAttr
  },
};

const RAIL_ChannelConfig_t WuR_OOK_channelConfig = {
  .phyConfigBase = WuR_OOK_modemConfigBase,
  .phyConfigDeltaSubtract = NULL,
  .configs = WuR_OOK_channels,
  .length = 1U,
  .signature = 0UL,
};

const RAIL_ChannelConfig_t *channelConfigs[] = {
  &WuR_OOK_channelConfig,
  NULL
};

uint32_t wurAccelerationBuffer[183];
