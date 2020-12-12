################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
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
all: T5-ADC.axf

# Tool invocations
T5-ADC.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/home/fedebos/Documents/MCUXpresso_10.2.1_795/workspace/CMSISv2p00_LPC17xx/Debug" -Xlinker -Map="T5-ADC.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m3 -mthumb -T "T5-ADC_Debug.ld" -o "T5-ADC.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) T5-ADC.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "T5-ADC.axf"; # arm-none-eabi-objcopy -v -O binary "T5-ADC.axf" "T5-ADC.bin" ; # checksum -p LPC1769 -d "T5-ADC.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
