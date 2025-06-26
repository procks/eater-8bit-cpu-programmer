
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <system.h>
#include "usb_cdc.h"
#include <gpio.h>
#include <stdbool.h>
#include <flash.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define BLOCK_SIZE 16
#define unspec 0xFFFFFFFF
inline uint32_t if_unspec(uint32_t val, uint32_t repl) { return val == unspec? repl: val; }

enum {
    CMD_INVALID,
    CMD_CPU_WRITE,
    CMD_CPU_READ,
    CMD_STORE,
    CMD_LOAD,
    CMD_RUN_ON_CPU
};

extern uint8_t flashProgramm[BLOCK_SIZE];

uint32_t getHex32(char **pData, uint32_t defaultValue);
void printByte(uint8_t b);
uint8_t parseCommand(char c);
void loadFromFlash();
void consoleRoutine();

void writeFlashProgramToCpu();

#ifdef __cplusplus
};
#endif
