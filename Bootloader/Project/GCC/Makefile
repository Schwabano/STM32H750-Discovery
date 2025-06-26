###############################################################################
# Makefile for Embedded Wizard GUI Application
###############################################################################
export MAKEFLAGS += --silent

###############################################################################
# GENERAL PROJECT CONFIGURATION
###############################################################################
APP_FILE          = Bootloader-STM32H750-Discovery

###############################################################################
# GENERAL SETTINGS & PATHS
###############################################################################
SRC_PATH          = ../../Source
MISC_PATH         = ../../../ThirdParty

OBJ_PATH          = ./Obj
BIN_PATH          = ./Bin

SDK_PATH          = ../../../ThirdParty/STM32Cube_FW_H7
DRIVERS_PATH      = $(SDK_PATH)/Drivers
STARTUP           = ./startup_stm32h750xx.s
LINK_SCRIPT       = ./stm32h7_flash.ld

###############################################################################
# Include standard rules and utilities
# Include Embedded Wizard configuration and list of generated source code
###############################################################################
include ./utilities.mk
include ./rules.mk

###############################################################################
# Standard directories for C files
###############################################################################
vpath %.c           $(SRC_PATH)                                                \
                    $(DRIVERS_PATH)/STM32H7xx_HAL_Driver/Src                   \

###############################################################################
# SOURCES
###############################################################################
APP_C =                                                                        \
                    main.c                                                     \
                    fmc.c                                                      \
                    memory_msp.c                                               \
                    qspi.c                                                     \
                    stm32h7xx_it.c                                             \
                    system_stm32h7xx.c                                         \
                    stm32h7xx_hal.c                                            \
                    stm32h7xx_hal_rcc.c                                        \
                    stm32h7xx_hal_rcc_ex.c                                     \
                    stm32h7xx_hal_cortex.c                                     \
                    stm32h7xx_hal_gpio.c                                       \
                    stm32h7xx_hal_sdram.c                                      \
                    stm32h7xx_hal_qspi.c                                       \
                    stm32h7xx_hal_dma.c                                        \
                    stm32h7xx_ll_fmc.c                                         \
                    stm32h7xx_hal_pwr_ex.c                                     \

###############################################################################
# LIBRARIES
###############################################################################
LIBS :=             m                                                          \
                    c                                                          \
                    nosys                                                      \

###############################################################################
# INCLUDES
###############################################################################
INCLUDES =                                                                     \
                    -I$(SRC_PATH)                                              \
                    -I$(DRIVERS_PATH)/STM32H7xx_HAL_Driver/Inc                 \
                    -I$(DRIVERS_PATH)/CMSIS/Device/ST/STM32H7xx/Include        \
                    -I$(DRIVERS_PATH)/CMSIS/Include                            \
                    -I$(DRIVERS_PATH)/BSP/STM32H750B-DK                        \
                    -I$(DRIVERS_PATH)/BSP/Components                           \

###############################################################################
# DEFINES
###############################################################################
CFLAGS =            -O2 -Wall -mcpu=cortex-m7 -mlittle-endian -mthumb          \
                    -mthumb-interwork -mfpu=fpv5-d16 -mfloat-abi=hard          \
                    -ffunction-sections -fdata-sections                        \
                    -DSTM32H750xx -DUSE_STM32H750B_DK                          \


###############################################################################
# OBJECTS
###############################################################################
APP_OBJ           := $(foreach file,$(APP_C),             $(OBJ_PATH)/$(strip $(basename $(file))).o)

OBJS := $(APP_OBJ)

MAPFILE   = -Wl,-Map=$(BIN_PATH)/$(APP_FILE).map

LINKING   = $(LD) -mcpu=cortex-m7 -mlittle-endian -mthumb -mthumb-interwork    \
            -mfpu=fpv5-sp-d16 -mfloat-abi=hard -Wl,--gc-sections               \
            $(MAPFILE)                                                         \
            $(STARTUP)                                                         \
            $(OBJS)                                                            \
            $(addprefix -l,$(LIBS))                                            \
            -T $(LINK_SCRIPT)                                                  \
            -o $(BIN_PATH)/$(APP_FILE).elf

TRANSLATE = $(OBJCOPY) -O ihex $(BIN_PATH)/$(APP_FILE).elf $(BIN_PATH)/$(APP_FILE).hex

FLASHING  = ..\..\..\Application\FlashDownload\FlashDownload.cmd $(BIN_PATH)/$(APP_FILE).hex

PRINT_SIZE = $(SIZE) $(BIN_PATH)/$(APP_FILE).elf

###############################################################################
# RULES
###############################################################################
$(APP_FILE): precompile $(OBJS)
	@echo Linking $(APP_FILE)
	@echo $(LINKING)
	$(LINKING)
	@echo Memory Usage:
	$(PRINT_SIZE)
	$(TRANSLATE)
	@echo $(APP_FILE) successfully built !!

install: $(APP_FILE)
	$(FLASHING)

projectecho:
	@echo -------------------------------------------------
	@echo Creating $(APP_FILE)
	@echo -------------------------------------------------
	@echo Compiler Options: $(CFLAGS)

createdirs:
	@echo -------------------------------------------------
	@echo Creating object and binary directories
	-$(MKDIR) $(OBJ_PATH)
	-$(MKDIR) $(BIN_PATH)
	@echo -------------------------------------------------

precompile: projectecho createdirs ;

clean:
	@echo Cleaning $(OBJ_PATH) and $(BIN_PATH)
	-$(RMDIR) $(OBJ_PATH)
	-$(RMDIR) $(BIN_PATH)

include $(wildcard $(patsubst %,%.d,$(basename $(OBJS))))
