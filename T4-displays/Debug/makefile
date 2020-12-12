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
all: T4-displays.axf

# Tool invocations
T4-displays.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/home/fedebos/Documents/MCUXpresso_10.2.1_795/workspace/CMSISv2p00_LPC17xx/Debug" -Xlinker -Map="T4-displays.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m3 -mthumb -T "T4-displays_Debug.ld" -o "T4-displays.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) T4-displays.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "T4-displays.axf"; # arm-none-eabi-objcopy -v -O binary "T4-displays.axf" "T4-displays.bin" ; # checksum -p LPC1769 -d "T4-displays.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
