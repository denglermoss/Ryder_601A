# Project Makefile

CXX := g++
CC  := gcc
CXXFLAGS := -O2 -std=c++17 \
	-I. -Idrivers -Iwrappers -Imain -Iwaveform -Icomm -Iconfig
CFLAGS   := -O2 \
	-I. -Idrivers -Iwrappers -Imain -Iwaveform -Icomm -Iconfig
LDFLAGS  := -lbcm2835 -lpthread

# Drivers (C)
DRIVER_C_SRC := \
	drivers/ADS1256.c \
	drivers/DAC8532.c \
	drivers/DEV_Config.c

DRIVER_OBJS := $(DRIVER_C_SRC:.c=.o)

# Wrappers (C++)
WRAPPER_CPP_SRC := \
	wrappers/ads1256_driver.cpp \
	wrappers/dac8532_driver.cpp

WRAPPER_OBJS := $(WRAPPER_CPP_SRC:.cpp=.o)

# App sources (C++)
APP_CPP_SRC := \
	main/controller.cpp \
	main/main.cpp \
	waveform/wave_gen.cpp \
	waveform/functions.cpp \
	comm/comm.cpp

APP_OBJS := $(APP_CPP_SRC:.cpp=.o)

# Final binary
TARGET := app

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(DRIVER_OBJS) $(WRAPPER_OBJS) $(APP_OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Compile rules
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(DRIVER_OBJS) $(WRAPPER_OBJS) $(APP_OBJS) $(TARGET)

# Convenience test target for the minimal Waveshare DAC test
.PHONY: test_dac

test_dac:
	$(CXX) -O2 -std=c++17 tests/waveshare_test.cpp -lbcm2835 -o tests/waveshare_test
