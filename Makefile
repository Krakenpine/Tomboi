# Project Name
TARGET = Tomboi
APP_TYPE = BOOT_QSPI
# Sources
CPP_SOURCES += Tomboi-seed.cpp \
adsrLP.cpp \
lfoLP.cpp \
delayLP.cpp \
pitchLP.cpp \
chorusLP.cpp \
sample.cpp \
voice.cpp \
polyvoice.cpp \
tomvoice.cpp

# Library Locations
LIBDAISY_DIR = ../libDaisy
DAISYSP_DIR = ../DaisyExamples/DaisySP

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

