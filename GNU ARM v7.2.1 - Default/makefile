################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include zcl-core-cli/subdir.mk
-include zcl-core/subdir.mk
-include version-debug/subdir.mk
-include udp-debug/subdir.mk
-include stack/thread-mbedtls-stack/EFR32/subdir.mk
-include stack/thread-mbedtls-stack/subdir.mk
-include slaac-client/subdir.mk
-include serial/EFR32/subdir.mk
-include serial/subdir.mk
-include scenes-server/subdir.mk
-include scan-debug/subdir.mk
-include ota-bootload-storage-eeprom/subdir.mk
-include ota-bootload-core/subdir.mk
-include ota-bootload-client-policy/subdir.mk
-include ota-bootload-client/subdir.mk
-include on-off-server/subdir.mk
-include nvm3/EFR32/subdir.mk
-include nvm3/subdir.mk
-include main/subdir.mk
-include level-control-server/subdir.mk
-include led/EFR32/subdir.mk
-include idle-sleep/subdir.mk
-include identify-server/subdir.mk
-include icmp-debug/subdir.mk
-include heartbeat/subdir.mk
-include hal-library/EFR32/subdir.mk
-include groups-server/subdir.mk
-include external-generated-files/subdir.mk
-include emlib/subdir.mk
-include emdrv/subdir.mk
-include eeprom/subdir.mk
-include dtls-auth-params/subdir.mk
-include diagnostic-stub/subdir.mk
-include dhcp-client/subdir.mk
-include debug-print/subdir.mk
-include debug-channel/EFR32/subdir.mk
-include command-interpreter2/subdir.mk
-include coexistence-stub/EFR32/subdir.mk
-include coap-dispatch/subdir.mk
-include cli/subdir.mk
-include buzzer/EFR32/subdir.mk
-include button/EFR32/subdir.mk
-include basic-server/subdir.mk
-include antenna-stub/subdir.mk
-include address-configuration-debug/subdir.mk
-include adc/EFR32/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: pre-build main-build

# Main-build Target
main-build: DMP_thread_wur_noOS.axf

# Tool invocations
DMP_thread_wur_noOS.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base/hal/micro/cortexm3/efm32/gcc-cfg.ld" -L"/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/base/hal/micro/cortexm3/" -Xlinker --defsym=FLASH_SIZE=1048576 -Xlinker --defsym=RAM_SIZE=262144 -Xlinker --defsym=FLASH_PAGE_SIZE=2048 -Xlinker --defsym=SIMEEPROM_SIZE=36864 -Xlinker --defsym=PSSTORE_SIZE=0 -Xlinker --defsym=LONGTOKEN_SIZE=0 -Xlinker --defsym=HEADER_SIZE=256 -Xlinker --defsym=LOCKBITS_IN_MAINFLASH_SIZE=0 -Xlinker --defsym=APP_GECKO_INFO_PAGE_BTL=1 -Xlinker --defsym=EMBER_MALLOC_HEAP_SIZE=0 -Xlinker --gc-sections -Xlinker -Map="DMP_thread_wur_noOS.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o DMP_thread_wur_noOS.axf -Wl,--start-group "./adc/EFR32/adc-efr32.o" "./address-configuration-debug/address-configuration-debug.o" "./antenna-stub/antenna-stub.o" "./basic-server/basic-server.o" "./button/EFR32/button.o" "./buzzer/EFR32/buzzer-efr32.o" "./cli/cli.o" "./coap-dispatch/coap-dispatch.o" "./coexistence-stub/EFR32/coexistence-802154.o" "./coexistence-stub/EFR32/coulomb-counter-802154.o" "./command-interpreter2/command-interpreter2-error.o" "./command-interpreter2/command-interpreter2-util.o" "./command-interpreter2/command-interpreter2.o" "./debug-channel/EFR32/debug-jtag-efr32.o" "./debug-print/debug-print.o" "./dhcp-client/dhcp-client.o" "./diagnostic-stub/diagnostic-stub.o" "./dtls-auth-params/dtls-auth-params.o" "./eeprom/eeprom-cli.o" "./eeprom/eeprom-print.o" "./eeprom/eeprom.o" "./emdrv/dmadrv.o" "./emdrv/gpiointerrupt.o" "./emdrv/rtcdriver.o" "./emdrv/sleep.o" "./emdrv/tempdrv.o" "./emdrv/uartdrv.o" "./emdrv/ustimer.o" "./emlib/em_adc.o" "./emlib/em_cmu.o" "./emlib/em_core.o" "./emlib/em_emu.o" "./emlib/em_gpio.o" "./emlib/em_i2c.o" "./emlib/em_ldma.o" "./emlib/em_leuart.o" "./emlib/em_msc.o" "./emlib/em_prs.o" "./emlib/em_rmu.o" "./emlib/em_rtcc.o" "./emlib/em_se.o" "./emlib/em_system.o" "./emlib/em_timer.o" "./emlib/em_usart.o" "./emlib/em_wdog.o" "./emlib/system_efr32mg12p.o" "./external-generated-files/bootloader-interface-app.o" "./external-generated-files/bootloader-interface.o" "./external-generated-files/btl_interface.o" "./external-generated-files/btl_interface_storage.o" "./external-generated-files/sample-dotdot-certificates.o" "./groups-server/groups-server.o" "./hal-library/EFR32/assert-crash-handlers.o" "./hal-library/EFR32/crc.o" "./hal-library/EFR32/cstartup-common.o" "./hal-library/EFR32/endian.o" "./hal-library/EFR32/faults-v7m.o" "./hal-library/EFR32/hal-config-gpio.o" "./hal-library/EFR32/hal-config.o" "./hal-library/EFR32/isr-stubs.o" "./hal-library/EFR32/long-token.o" "./hal-library/EFR32/mem-util.o" "./hal-library/EFR32/mfg-token.o" "./hal-library/EFR32/micro-common.o" "./hal-library/EFR32/micro.o" "./hal-library/EFR32/mpu.o" "./hal-library/EFR32/random.o" "./hal-library/EFR32/sleep-efm32.o" "./hal-library/EFR32/token-def.o" "./hal-library/EFR32/token.o" "./heartbeat/heartbeat.o" "./icmp-debug/icmp-debug.o" "./identify-server/identify-server.o" "./idle-sleep/idle-sleep.o" "./led/EFR32/led.o" "./level-control-server/level-control-server.o" "./light-implementation.o" "./ook-test.o" "./rail_config.o" "./thread-bookkeeping.o" "./thread-callback-stubs.o" "./thread-callbacks.o" "./thread-cli.o" "./thread-coap-dispatch.o" "./thread-events.o" "./thread-zclip.o" "./main/main.o" "./nvm3/EFR32/nvm3_default.o" "./nvm3/EFR32/nvm3_hal_flash.o" "./nvm3/nvm3_lock.o" "./on-off-server/on-off-server.o" "./ota-bootload-client/ota-bootload-client.o" "./ota-bootload-client-policy/ota-bootload-client-policy.o" "./ota-bootload-core/ota-bootload-core.o" "./ota-bootload-storage-eeprom/ota-bootload-storage-eeprom.o" "./scan-debug/scan-debug.o" "./scenes-server/scenes-server.o" "./serial/EFR32/ember-printf.o" "./serial/EFR32/serial.o" "./serial/com.o" "./slaac-client/slaac-client.o" "./stack/thread-mbedtls-stack/EFR32/SEGGER_RTT.o" "./stack/thread-mbedtls-stack/ember-ip-configuration.o" "./udp-debug/udp-debug.o" "./version-debug/version-debug.o" "./zcl-core/cbor-encoder.o" "./zcl-core/zcl-core-access.o" "./zcl-core/zcl-core-attribute.o" "./zcl-core/zcl-core-binding.o" "./zcl-core/zcl-core-cache.o" "./zcl-core/zcl-core-cli.o" "./zcl-core/zcl-core-command.o" "./zcl-core/zcl-core-dtls-manager.o" "./zcl-core/zcl-core-endpoint.o" "./zcl-core/zcl-core-ez-mode.o" "./zcl-core/zcl-core-group.o" "./zcl-core/zcl-core-message.o" "./zcl-core/zcl-core-notification.o" "./zcl-core/zcl-core-reporting.o" "./zcl-core/zcl-core-resource-directory.o" "./zcl-core/zcl-core-uri.o" "./zcl-core/zcl-core-well-known.o" "./zcl-core/zcl-core.o" "./zcl-core/zclip-struct.o" "./zcl-core-cli/zcl-core-cli-implementation.o" "/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//protocol/thread/build/thread-mbedtls-stack-cortexm3-gcc-efr32mg12p-rail/thread-mbedtls-stack.a" "/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//protocol/thread/build/hal-library-cortexm3-gcc-efr32mg12p-rail/hal-library.a" "/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//protocol/thread/build/dhcp-library-cortexm3-gcc-efr32mg12p-rail/dhcp-library.a" "/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/radio/rail_lib/autogen/librail_release/librail_multiprotocol_efr32xg12_gcc_release.a" "/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//protocol/thread/build/sim-eeprom2-to-nvm3-upgrade-stub-library-cortexm3-gcc-efr32mg12p-rail/sim-eeprom2-to-nvm3-upgrade-stub-library.a" "/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//protocol/thread/build/mbedtls-dotdot-library-cortexm3-gcc-efr32mg12p-rail/mbedtls-dotdot-library.a" "/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//protocol/thread/build/mfglib-library-cortexm3-gcc-efr32mg12p-rail/mfglib-library.a" "/home/marti/workspace/sdks/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.6//platform/emdrv/nvm3/lib/libnvm3_CM4_gcc.a" -lm -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: DMP_thread_wur_noOS.hex'
	arm-none-eabi-objcopy -O ihex "DMP_thread_wur_noOS.axf" "DMP_thread_wur_noOS.hex"
	@echo ' '

	@echo 'Building bin file: DMP_thread_wur_noOS.bin'
	arm-none-eabi-objcopy -O binary "DMP_thread_wur_noOS.axf" "DMP_thread_wur_noOS.bin"
	@echo ' '

	@echo 'Building s37 file: DMP_thread_wur_noOS.s37'
	arm-none-eabi-objcopy -O srec "DMP_thread_wur_noOS.axf" "DMP_thread_wur_noOS.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "DMP_thread_wur_noOS.axf"
	@echo ' '

	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) DMP_thread_wur_noOS.axf
	-@echo ' '

pre-build:
	-sh '/home/marti/SimplicityStudio/v4_workspace/DMP_thread_wur_noOS/light-prebuild.sh' 'DMP_thread_wur_noOS' '/home/marti/SimplicityStudio/v4_workspace/DMP_thread_wur_noOS' '' '/home/marti/workspace/sdks/SimplicityStudio_v4'
	-@echo ' '

post-build:
	-sh '/home/marti/SimplicityStudio/v4_workspace/DMP_thread_wur_noOS/light-postbuild.sh' 'DMP_thread_wur_noOS' '/home/marti/SimplicityStudio/v4_workspace/DMP_thread_wur_noOS' '' '/home/marti/workspace/sdks/SimplicityStudio_v4'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: main-build pre-build post-build

-include ../makefile.targets
