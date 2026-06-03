
# Compiler
CC = arm-none-eabi-gcc

# Debugger
GDB = arm-none-eabi-gdb

# Target Microcontroller Architecture is Arm Cortex M4
MACH = -mcpu=cortex-m4 -mthumb

# Directories
SRC_DIR = src
BUILD_DIR = build
INC_DIR = inc
LINKER_DIR = linker
STARTUP_DIR = startup

# Specify the name of the binary file
TARGET = $(BUILD_DIR)/ambient_environment_station

# Find all C source files in the source directory
C_SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(STARTUP_DIR)/*.c)


# Use source filenames to create object filenames
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(C_SRCS)))

# Path to linker script
LDSCRIPT = $(LINKER_DIR)/aes_linker_stm32f401re.ld

# OpenOCD config file for target hardware
OOCD_INTERFACE = interface/stlink.cfg
OOCD_TARGET = target/stm32f4x.cfg

# Compiler flags
CFLAGS = -c $(MACH) -g -O0 -Wall -I$(INC_DIR) 

# Linker flags
LDFLAGS = $(MACH) -T $(LDSCRIPT) -nostdlib --specs=nano.specs --specs=nosys.specs -Wl,-Map=$(TARGET).map,--cref

# ==========================================
# Rules
# ==========================================
.PHONY: all clean flash oocd debug

all: $(TARGET).elf

# Link object files into the final .elf file
$(TARGET).elf: $(OBJS)
	@echo "Linking $@"
	@$(CC) $(LDFLAGS) -o $@ $^
	@echo "Build successful!"

# Compile application C files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling application C files: $<"
	@$(CC) $(CFLAGS) -o $@ $<

# Compile startup C file
$(BUILD_DIR)/%.o: $(STARTUP_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling startup C files: $<"
	@$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

# ==========================================
# Utility Commands
# ==========================================
clean:
	rm -rf $(BUILD_DIR)

flash: all
	openocd -f $(OOCD_INTERFACE) -f $(OOCD_TARGET) -c "program $(TARGET).elf verify reset exit"

oocd:
	openocd -f $(OOCD_INTERFACE) -f $(OOCD_TARGET)

debug: all
	$(GDB) $(TARGET).elf -ex "target extended-remote localhost:3333"


