ROOT_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_DIR=$(ROOT_DIR)/build
SOURCE_DIR=$(ROOT_DIR)/source
OUTPUT_DIR=$(ROOT_DIR)/output

CC = g++
CFLAGS = $(INCLUDE_PATHS) -ggdb
LDFLAGS =
INCLUDE_PATHS=-I.

OUTPUT_FILENAME=sha1
OUTPUT=$(OUTPUT_DIR)/$(OUTPUT_FILENAME)

SOURCES = sha1.cpp
SOURCES_ABSOLUTE = $(addprefix $(SOURCE_DIR)/, $(SOURCES))
OBJECTS_ABSOLUTE = $(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))

.PHONY: default
default: link

.PHONY: bootstrap
bootstrap:
	mkdir -p build
	mkdir -p output

.PHONY: compile
compile: $(OBJECTS_ABSOLUTE) $(ROOT_DIR)/makefile

.PHONY: link
link: $(OUTPUT) compile

.PHONY: run
run: link
	$(OUTPUT)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)/*
	rm -rf $(OUTPUT_DIR)/*

$(OBJECTS_ABSOLUTE): $(SOURCES_ABSOLUTE)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OUTPUT): $(OBJECTS_ABSOLUTE)
	$(CC) $(LDFLAGS) -o $@ $^