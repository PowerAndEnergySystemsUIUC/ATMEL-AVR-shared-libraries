################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
LIB_AS_ARGS :=
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
HEX_FLASH_FILE_PATH :=
HEX_FLASH_FILE_PATH_AS_ARGS :=
HEX_EEPROM_FILE_PATH :=
HEX_EEPROM_FILE_PATH_AS_ARGS :=
LSS_FILE_PATH :=
LSS_FILE_PATH_AS_ARGS :=
MAP_FILE_PATH :=
MAP_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=C:/Program Files (x86)/Atmel/AVR Studio 5.0/AVR ToolChain/bin/
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../rtc_PCF8583.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
rtc_PCF8583.o


OBJS_AS_ARGS +=  \
rtc_PCF8583.o


C_DEPS +=  \
rtc_PCF8583.d


C_DEPS_AS_ARGS +=  \
rtc_PCF8583.d


OUTPUT_FILE_PATH +=lib_rtc.a

OUTPUT_FILE_PATH_AS_ARGS +=lib_rtc.a

HEX_FLASH_FILE_PATH +=_rtc.hex

HEX_FLASH_FILE_PATH_AS_ARGS +=_rtc.hex

HEX_EEPROM_FILE_PATH +=_rtc.eep

HEX_EEPROM_FILE_PATH_AS_ARGS +=_rtc.eep

LSS_FILE_PATH +=_rtc.lss

LSS_FILE_PATH_AS_ARGS +=_rtc.lss

MAP_FILE_PATH =_rtc.map

MAP_FILE_PATH_AS_ARGS =_rtc.map

LIB_AS_ARGS +=lib_rtc.a

ADDITIONAL_DEPENDENCIES:= $(HEX_FLASH_FILE_PATH) $(LSS_FILE_PATH) $(HEX_EEPROM_FILE_PATH) size

OUTPUT_FILE_DEP:= ./makedep.mk

# AVR/GNU C Compiler



./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(QUOTE)$(AVR_APP_PATH)avr-gcc.exe$(QUOTE) -funsigned-char -funsigned-bitfields -O0 -fpack-struct -fshort-enums -g2 -Wall -c -std=gnu99  -mmcu=atmega328p   -MD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<



# AVR/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

# AVR32/GNU C Archiver
$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP)
	@echo	Building target: $@
	@echo	Invoking: AVR/GNU C Archiver 
	$(QUOTE)$(AVR_APP_PATH)avr-ar.exe$(QUOTE) -r  $(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS)
	@echo	Finished building target: $@



$(HEX_FLASH_FILE_PATH): $(OUTPUT_FILE_PATH)
	

$(HEX_EEPROM_FILE_PATH): $(OUTPUT_FILE_PATH)
	

$(LSS_FILE_PATH): $(OUTPUT_FILE_PATH)
	

size: $(OUTPUT_FILE_PATH)
	

# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS)$(C_DEPS_AS_ARGS) $(EXECUTABLES) $(LIB_AS_ARGS) $(HEX_FLASH_FILE_PATH_AS_ARGS) $(HEX_EEPROM_FILE_PATH_AS_ARGS) $(LSS_FILE_PATH_AS_ARGS) $(MAP_FILE_PATH_AS_ARGS)

