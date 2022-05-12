// Marvin Coopman
// 1001781933

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// Red LED:
//   PF1 drives an NPN transistor that powers the red LED
// Green LED:
//   PF3 drives an NPN transistor that powers the green LED
// UART Interface:
//   U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
//   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port
//   Configured to 115,200 baud, 8N1
//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#ifndef SERIAL_H_
#define SERIAL_H_

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#define MAX_CHARS 80
#define MAX_FIELDS 5

typedef struct _USER_DATA
{
char buffer[MAX_CHARS+1];
uint8_t fieldCount;
uint8_t fieldPosition[MAX_FIELDS];
char fieldType[MAX_FIELDS]; //n = number a = alpha
} USER_DATA;

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void getsUart0(USER_DATA *data);
bool isAlpha(char alpha);
bool isDigit(char digit);
bool isDelimiter(char delimiter);
void parseFields(USER_DATA *data);
int32_t stringToint32(char* stringInt);
char* getFieldString(USER_DATA* data, uint8_t fieldNumber);
int32_t getFieldInteger(USER_DATA* data, uint8_t fieldNumber);
bool isCommand(USER_DATA* data, const char strCommand[], uint8_t minArguments);
bool stringEqual(char* command, char* input);
//void alert(char* command);
//void set(int state);



#endif /* SERIAL_H_ */
