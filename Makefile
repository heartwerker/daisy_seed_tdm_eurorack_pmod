# Project Name
TARGET = iobay

CXXFLAGS = -Wall -g -Os

# https://electro-smith.github.io/libDaisy/md_doc_2md_2__a7___getting-_started-_daisy-_bootloader.html
# APP_TYPE = BOOT_SRAM

# Sources
CPP_SOURCES = src/main.cpp

# Library Locations
LIBDAISY_DIR = libDaisy
DAISYSP_DIR = DaisySP

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

