################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6/protocol/thread/app/thread/plugin/zcl/identify-server/identify-server.c 

OBJS += \
./identify-server/identify-server.o 

C_DEPS += \
./identify-server/identify-server.d 


# Each subdirectory must supply rules for building sources it contributes
identify-server/identify-server.o: /home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6/protocol/thread/app/thread/plugin/zcl/identify-server/identify-server.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=gnu99 '-DCORTEXM3=1' '-DCORTEXM3_EFM32_MICRO=1' '-DCORTEXM3_EFR32=1' '-DCORTEXM3_EFR32_MICRO=1' '-DAPP_GECKO_INFO_PAGE_BTL=1' '-DCONFIGURATION_HEADER="thread-configuration.h"' '-DPLATFORM_HEADER="platform/base/hal/micro/cortexm3/compiler/gcc.h"' '-DEFR32MG12P=1' '-DEFR32MG12P332F1024GL125=1' '-DEFR32_SERIES1_CONFIG2_MICRO=1' '-DLOCKBITS_IN_MAINFLASH_SIZE=0' '-DPSSTORE_SIZE=0' '-DLONGTOKEN_SIZE=0' '-DHAL_CONFIG=1' '-DEMBER_AF_USE_HWCONF=1' '-DEMBER_COMMAND_BUFFER_LENGTH=250' '-DEMBER_AF_API_EMBER_TYPES="stack/include/ember-types.h"' '-DEMBER_STACK_IP=1' '-DMBEDTLS_CONFIG_FILE="stack/ip/tls/mbedtls/config.h"' '-DEMBER_MBEDTLS_STACK=1' '-DUSE_NVM3=1' '-DNVM3_DEFAULT_NVM_SIZE=NVM3_FLASH_PAGES*FLASH_PAGE_SIZE' '-DMFGLIB=1' '-DHAVE_TLS_JPAKE=1' '-DHAVE_TLS_ECDHE_ECDSA=1' '-DDEBUG_LEVEL=FULL_DEBUG' '-DEMLIB_USER_CONFIG=1' '-DAPPLICATION_TOKEN_HEADER="thread-token.h"' '-DAPPLICATION_MFG_TOKEN_HEADER="thread-mfg-token.h"' '-DNVM3_FLASH_PAGES=18' '-DNVM3_DEFAULT_CACHE_SIZE=200' '-DNVM3_DEFAULT_MAX_OBJECT_SIZE=254' '-DNVM3_DEFAULT_REPACK_HEADROOM=0' '-DPHY_RAIL=1' '-DPHY_RAIL_MP=1' -I"/home/marti/SimplicityStudio/v4_workspace/DMP_thread_wur_noOS" -I"/home/marti/SimplicityStudio/v4_workspace/DMP_thread_wur_noOS/hal-config" -I"/home/marti/SimplicityStudio/v4_workspace/DMP_thread_wur_noOS/external-generated-files" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6/" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base/hal" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base/hal//plugin" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base/hal//micro/cortexm3/efm32" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base/hal//micro/cortexm3/efm32/config" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base/hal//micro/cortexm3/efm32/efr32" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../CMSIS/Include" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../emdrv/common/inc" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../emdrv/config" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../emdrv/dmadrv/inc" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../emdrv/gpiointerrupt/inc" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../emdrv/rtcdrv/inc" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../emdrv/rtcdrv/test" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../emdrv/sleep/inc" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../emdrv/spidrv/inc" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../emdrv/tempdrv/inc" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../emdrv/uartdrv/inc" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../emdrv/ustimer/inc" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../emlib/inc" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../middleware/glib" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../middleware/glib/glib" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base//../radio/rail_lib/plugin" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/halconfig/inc/hal-config" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/module/config" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/halconfig" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//protocol/thread/app/thread/plugin/dtls-auth-params" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base/hal/micro/cortexm3/efm32" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/bootloader/api" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//protocol/thread" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//protocol/thread/stack" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//protocol/thread/app/util" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs-Restricted/EFR32MG12P/Include" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFR32MG12P/Include" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/bootloader" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/emdrv/nvm3/inc" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/radio/rail_lib/chip/efr32/efr32xg1x" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/radio/rail_lib/common" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/radio/rail_lib/plugin/coexistence/common" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/radio/rail_lib/plugin/coexistence/hal/efr32" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/radio/rail_lib/plugin/coexistence/protocol/ieee802154" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/radio/rail_lib/protocol/ble" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/radio/rail_lib/protocol/ieee802154" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/radio/rail_lib/protocol/zwave" -I"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//util/third_party/mbedtls/include" -Os -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"identify-server/identify-server.d" -MT"identify-server/identify-server.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


