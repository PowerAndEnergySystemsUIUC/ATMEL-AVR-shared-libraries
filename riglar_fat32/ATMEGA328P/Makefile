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
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../fat_access.c \
../fat_cache.c \
../fat_filelib.c \
../fat_format.c \
../fat_misc.c \
../fat_string.c \
../fat_table.c \
../fat_write.c \
../riglar_fat32_demo.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
fat_access.o \
fat_cache.o \
fat_filelib.o \
fat_format.o \
fat_misc.o \
fat_string.o \
fat_table.o \
fat_write.o \
riglar_fat32_demo.o


OBJS_AS_ARGS +=  \
fat_access.o \
fat_cache.o \
fat_filelib.o \
fat_format.o \
fat_misc.o \
fat_string.o \
fat_table.o \
fat_write.o \
riglar_fat32_demo.o


C_DEPS +=  \
fat_access.d \
fat_cache.d \
fat_filelib.d \
fat_format.d \
fat_misc.d \
fat_string.d \
fat_table.d \
fat_write.d \
riglar_fat32_demo.d


C_DEPS_AS_ARGS +=  \
fat_access.d \
fat_cache.d \
fat_filelib.d \
fat_format.d \
fat_misc.d \
fat_string.d \
fat_table.d \
fat_write.d \
riglar_fat32_demo.d


OUTPUT_FILE_PATH +=libriglar_fat32_ATMEGA328P.a

OUTPUT_FILE_PATH_AS_ARGS +=libriglar_fat32_ATMEGA328P.a

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

# AVR32/GNU C Compiler



















./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(QUOTE)C:\Program Files\Atmel\AVR Studio 5.1\extensions\Atmel\AVRGCC\3.3.1\AVRToolchain\bin\avr-gcc.exe$(QUOTE)  -O0 -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"  -mmcu=atmega328p  -o"$@" "$<" 
	@echo Finished building: $<



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)


$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU C Archiver
	$(QUOTE)C:\Program Files\Atmel\AVR Studio 5.1\extensions\Atmel\AVRGCC\3.3.1\AVRToolchain\bin\avr-ar.exe$(QUOTE) -r  -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS)
	@echo Finished building target: $@
	



# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS)$(C_DEPS_AS_ARGS) $(EXECUTABLES) 
	rm -rf "libriglar_fat32_ATMEGA328P.hex" "libriglar_fat32_ATMEGA328P.lss" "libriglar_fat32_ATMEGA328P.eep" "libriglar_fat32_ATMEGA328P.map"
	