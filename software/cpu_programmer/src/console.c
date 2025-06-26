#include "console.h"

const char hex[] = "0123456789abcdef";
int lineLen = 0;
char line[120];

uint8_t parseCommand(char c) {
    uint8_t cmd = CMD_INVALID;
    // Convert the command to lowercase.
    if ((c >= 'A') && (c <= 'Z')) {
        c |= 0x20;
    }

    switch (c) {
        case 'w':
			cmd = CMD_CPU_WRITE;
			break;
        case 'r':
			cmd = CMD_CPU_READ;
			break;
        case 's':
			cmd = CMD_STORE;
			break;
        case 'l':
			cmd = CMD_LOAD;
			break;
        case 'e':
			cmd = CMD_RUN_ON_CPU;
			break;
        default:
			cmd = CMD_INVALID;
			break;
    }
    return cmd;
}

/************************************************************
* convert a single hex character [0-9a-fA-F] to its value
* @param char c single character (digit)
* @return uint8_t value of the digit (0-15)
************************************************************/
uint8_t hexDigit(char c) {
    if ((c >= '0') && (c <= '9')) {
        return c - '0';
	} else if ((c >= 'a') && (c <= 'f')) {
        return c - 'a' + 10;
    } else if ((c >= 'A') && (c <= 'F')) {
        return c - 'A' + 10;
    } else {
        return 0xff;
    }
}

/************************************************************
* Convert a hex string to a uint32_t value.
* Skips leading spaces and terminates on the first non-hex
* character.  Leading zeroes are not required.
*
* No error checking is performed - if no hex is found then
* defaultValue is returned.  Similarly, a hex string of more than
* 8 digits will return the value of the last 8 digits.
* @param pointer to string with the hex value of the word (modified)
* @return unsigned int represented by the digits
************************************************************/
uint32_t getHex32(char **pData, uint32_t defaultValue) {
    uint32_t u32 = 0;

    while (isspace(**pData)) {
        ++(*pData);
    }

    if (isxdigit(**pData)) {
        while (isxdigit(**pData)) {
            u32 = (u32 << 4) | hexDigit(*(*pData)++);
        }
    } else {
        u32 = defaultValue;
    }

    return u32;
}

void printByte(uint8_t b) {
    char line[3];

    line[0] = hex[b >> 4];
    line[1] = hex[b & 0x0f];
    line[2] = '\0';

    CDC_print(line);
}

bool readLine() {
    memset(line, 0x0, sizeof(line));

    // read serial data until linebreak or buffer is full
    char c = ' ';
    if (CDC_available()) {
        c = CDC_read();
        if ((c == '\b') && (lineLen > 0))
        {
            // Backspace, forget last character
            --lineLen;
        }
        line[lineLen++] = c;
        CDC_write(c);
        if (lineLen >= sizeof(line)) {
            lineLen = 0;
        }
    }
    bool end = (c == '\n') || (c == '\r');
    if (end) {
        line[lineLen - 1] = 0;
        lineLen = 0;
    }

    return end;
}

void writeCursorBytes(char * pCursor) {
    uint32_t val;
    unsigned byteCtr = 0;

    uint8_t data[BLOCK_SIZE];

    while (((val = getHex32(&pCursor, unspec)) != unspec) && (byteCtr < BLOCK_SIZE)) {
        data[byteCtr++] = (uint8_t) val;
    }

    if (byteCtr > 0) {
		memset(flashProgramm, 0x0, sizeof(flashProgramm));
		for (int i = 0; i < byteCtr; i++) {
			flashProgramm[i] = data[i];
			printByte(flashProgramm[i]);
			CDC_print(" ");
		}
		CDC_newline();
    } else {
        return;
    }
	CDC_flush();
}

void storeToFlash() {
	FLASH_unlock();
	FLASH_END_erase();
	for (int i = 0; i < BLOCK_SIZE; i++) {
		printByte(flashProgramm[i]);
		FLASH_END_write(i * 2, flashProgramm[i]);
		CDC_print(" ");
	}
	FLASH_lock();
	CDC_newline();
	CDC_flush();
}

void loadFromFlash() {
	for (int i = 0; i < BLOCK_SIZE; i++) {
		flashProgramm[i] = FLASH_END_read(i * 2);
	}
    if (CDC_ready()) {
        for (int i = 0; i < BLOCK_SIZE; i++) {
            printByte(flashProgramm[i]);
            CDC_print(" ");
        }
        CDC_newline();
        CDC_flush();
    }
}

void consoleRoutine() {
    if (!CDC_ready()) return;
	if (!readLine()) return;
	uint8_t cmd = parseCommand(line[0]);

	switch (cmd) {
		case CMD_CPU_WRITE:
			CDC_print("Write: ");
	        char * pCursor = line + 1;
			writeCursorBytes(pCursor);
            writeFlashProgramToCpu();
			break;
		case CMD_STORE:
			CDC_print("Store: ");
			storeToFlash();
			break;
		case CMD_LOAD:
			CDC_print("Load: ");
			loadFromFlash();
            writeFlashProgramToCpu();
			break;

		default:
			CDC_println("Commands:");
			CDC_println("  w dd dd   - write to CPU");
			CDC_println("  s         - store current programm to flash");
			CDC_println("  l         - load from flash and write to CPU");
			// CDC_println("  e         - execute programm");
			break;
	}
}
