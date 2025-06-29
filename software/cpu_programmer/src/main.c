#include <system.h>
#include <gpio.h>
#include <stdbool.h>
#include <console.h>

#define LED PA9
#define LED2 PB7
#define SW1 PC17
#define SW2 PB7
#define DATA0 PA0
#define DATA1 PA1
#define DATA2 PA2
#define DATA3 PA3
#define DATA4 PA4
#define DATA5 PA5
#define DATA6 PA6
#define DATA7 PA7
#define ADDRESS0 PA9
#define ADDRESS1 PA11
#define ADDRESS2 PA10
#define ADDRESS3 PC3
#define WRITE PB1

int dataMap[8] = { DATA0, DATA1, DATA2, DATA3, DATA4, DATA5, DATA6, DATA7 };
int addressMap[8] = { ADDRESS0, ADDRESS1, ADDRESS2, ADDRESS3 };

uint8_t flashProgramm[BLOCK_SIZE];
uint8_t * currentProgram;

// INC (B) intraction (default 5 steps)
uint8_t program0[BLOCK_SIZE] = { 0x51, 0x4e, 0x50, 0x2e, 0x70, 0xe0, 0x4f, 0x1e, 0x4d, 0x1f, 0x4e, 0x1d, 0x63  }; // Fibonacci
uint8_t program1[BLOCK_SIZE] = { 0x51, 0x48, 0xE0, 0x28, 0x70, 0xE0, 0x48, 0x63 }; // Double
uint8_t program2[BLOCK_SIZE] = { 0x1D, 0xB1, 0x4D, 0xC3, 0x8F, 0x73, 0x1D, 0xC5, 0x8E, 0x77, 0x1D, 0xE0, 0x60, 0x00, 0xC1, 0xE0 }; // FizzBuzz
uint8_t program3[BLOCK_SIZE] = { 0xE0, 0x2E, 0x74, 0x60, 0x3F, 0xE0, 0x80, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01 }; // Counts up to 255 and then counts down to 0

// uint8_t program3[BLOCK_SIZE] = { 0x51, 0x47, 0x50, 0xE0, 0x27, 0xE0, 0x64 }; // Count up
// uint8_t program3[BLOCK_SIZE] = { 0x50, 0xB1, 0xE0, 0x61 }; // Test1 inc
// uint8_t program3[BLOCK_SIZE] = { 0xE0, 0x2E, 0x74, 0x60, 0x3F, 0xE0, 0x88, 0x64, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x06 }; // Counts up to 255 by 3s and then counts down to 0 by 6s
// uint8_t program3[BLOCK_SIZE] = { 0x1F, 0xB1, 0x4F, 0x3E, 0x87, 0x73, 0x60, 0x1F, 0xE0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00 }; // Modulo 3

//ADDS (B) intraction (required 6 steps modification)
// uint8_t program0[BLOCK_SIZE] = { 0x51, 0x4e, 0x50, 0x2e, 0x70, 0xe0, 0x4f, 0x1e, 0x4d, 0x1f, 0x4e, 0x1d, 0x63  }; // Fibonacci
// uint8_t program1[BLOCK_SIZE] = { 0x51, 0xBF, 0x52, 0x4E, 0x1F, 0x3E, 0x80, 0x75, 0x51, 0xBE, 0x3F, 0xA4, 0xDF, 0x60, 0x00, 0x02 }; // Primes
// uint8_t program2[BLOCK_SIZE] = { 0x51, 0xBD, 0xC3, 0x8F, 0x72, 0x1D, 0xC5, 0x8E, 0x76, 0x1D, 0xE0, 0x60, 0x00, 0x00, 0xC1, 0xE0 }; // FizzBuzz
// uint8_t program3[BLOCK_SIZE] = { 0xE0, 0x2E, 0x74, 0x60, 0x3F, 0xE0, 0x80, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01 }; // Counts up to 255 and then counts down to 0

#define PROGRAMS_LEN 4
#define FLASH_PROGRAM_NUM PROGRAMS_LEN - 1
uint8_t* programs[] = { program0, program1, program2, flashProgramm };
int programNum = PROGRAMS_LEN - 1;

void writeLine(int pin, bool value) {
	if (value) {
		PIN_input(pin);
	} else {
		PIN_output(pin);
		PIN_low(pin);
	}
}

void writeData(uint8_t address, uint8_t data) {
	for (int i = 0; i < 4; i++) {
		int bit = (address >> i) & 1;
		writeLine(addressMap[i], bit);
	}
	for (int i = 0; i < 8; i++) {
		int bit = (data >> i) & 1;
		writeLine(dataMap[i], bit);
	}
	
	writeLine(WRITE, false);
	DLY_ms(5);
	writeLine(WRITE, true);
	DLY_ms(5);
}

void setLinesIn() {
	for (int i = 0; i < 4; i++) {
		PIN_input(addressMap[i]);
	}
	for (int i = 0; i < 8; i++) {
		PIN_input(dataMap[i]);
	}
	PIN_input(WRITE);
}

bool checkLines() {
	setLinesIn();
	bool ok = true;
	for (int i = 0; i < 4; i++) {
		if (!PIN_read(addressMap[i])) ok = false;
	}
	for (int i = 0; i < 8; i++) {
		if (!PIN_read(dataMap[i])) ok = false;
	}
	if (!PIN_read(WRITE)) ok = false;
	return ok;
}

void blinkError() {
	PIN_output(LED2);
	for (int i = 0; i < 3; i++) {
		PIN_low(LED2);
		DLY_ms(50);
		//PIN_high(LED2);
		//DLY_ms(50);
	}
	PIN_input_PU(LED2);
  DLY_ms(1);
}

void blinkOk() {
	PIN_output(LED2);
	for (int i = 0; i < 5; i++) {
		PIN_low(LED2);
		DLY_ms(250);
		PIN_high(LED2);
		DLY_ms(250);
	}
	PIN_input_PU(LED2);
}

void printCurrentProgram() {
	if (CDC_ready()) {
		for (int i = 0; i < BLOCK_SIZE; i++) {
			printByte(currentProgram[i]);
			CDC_print(" ");
		}
		CDC_newline();
		CDC_flush();	
	}
}

void writeToCpu() {
	printCurrentProgram();
	// writeLine(LED, false);
	writeLine(addressMap[programNum], false);
	DLY_ms(250);
	// writeLine(LED, true);
	writeLine(addressMap[programNum], true);
	DLY_ms(100);
	for (int a = 0; a < BLOCK_SIZE; a++) {
		writeData(a, currentProgram[a]);
	}
}

void writeFlashProgramToCpu() {
	programNum = FLASH_PROGRAM_NUM;
	currentProgram = programs[programNum];
	writeToCpu();
}

void nextProgram() {
	programNum++;
	if (programNum >= PROGRAMS_LEN) {
		programNum = 0;
	}
	currentProgram = programs[programNum];
	writeToCpu();
}

void initFlashProgram() {
	loadFromFlash();
	// if not found use preset program4
	if (flashProgramm[0] == 0xff) {
		for (int i = 0; i < BLOCK_SIZE; i++) {
			flashProgramm[i] = program3[i];
		}
	}
}

bool lastButtonState = true;
int main() {
	CDC_init();
	// PIN_input_PD(SW1); // USB will not work
	PIN_input_PU(SW2);
	initFlashProgram();
	while(1) {
		if (!checkLines()) {
			blinkError();
		}
		else {
			bool currentState = PIN_read(SW2);
			if (!lastButtonState && currentState) {
				DLY_ms(30); // debounce
				currentState = PIN_read(SW2);
			
				if (currentState) {
					nextProgram();
				}
			}	
			lastButtonState = currentState;
		}
	    consoleRoutine();
	}
}
