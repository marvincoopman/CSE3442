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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "clock.h"
#include "uart0.h"
#include "tm4c123gh6pm.h"
#include "serial.h"





// Bitband aliases
#define RED_LED      (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 1*4)))
#define GREEN_LED    (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4)))

// PortF masks
#define GREEN_LED_MASK 8
#define RED_LED_MASK 2

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------


void getsUart0(USER_DATA *data)
{
    //MAX_CHARS
    uint32_t count = 0;
    char tempChar;
    while(1)
    {
         tempChar = getcUart0();
            //Backspace pressed
            if(tempChar == 8 || tempChar == 127)
            {
                if(count > 0)
                {
                    count--;
                }
            }
            //Enter key pressed
            else if(tempChar == 13)
            {
                data->buffer[count] = 0;
                return;
            }
            //Printable character or space pressed
            else if(tempChar >= 32)
            {
                data->buffer[count++] = tempChar;
                //if maxChars reached -> end string
                if(count == MAX_CHARS)
                {
                    data->buffer[count] = 0;
                    return;
                }
            }
    }

}

bool isAlpha(char alpha)
{
    if((alpha >= 'a' && alpha <= 'z') || (alpha >= 'A' && alpha <= 'Z'))
    {
        return true;
    }
    return false;
}

bool isDigit(char digit)
{
    if((digit >= '0' && digit <= '9') || digit == '.' || digit == '-')
    {
        return true;
    }
    return false;
}

bool isDelimiter(char delimiter)
{
    if(!(isAlpha(delimiter) || isDigit(delimiter)))
    {
        return true;
    }
    return false;
}

void parseFields(USER_DATA *data)
{
    int index = 0;
    int fieldTypeIndex = 0;
    while(data->buffer[index] != 0)
    {
        if(index == 0)
        {
            if(isDigit(data->buffer[index]))
            {
                data->fieldType[fieldTypeIndex] = 'n';
                data->fieldPosition[fieldTypeIndex] = index;
                fieldTypeIndex++;
            }
            else if(isAlpha(data->buffer[index]))
            {
                data->fieldType[fieldTypeIndex] = 'a';
                data->fieldPosition[fieldTypeIndex] = index;
                fieldTypeIndex++;
            }
            index++;
        }
        if(isAlpha(data->buffer[index]))
        {
            //if last char was a delimiter
            if(isDelimiter(data->buffer[index-1]))
            {
                data->fieldType[fieldTypeIndex] = 'a';
                data->fieldPosition[fieldTypeIndex] = index;
                fieldTypeIndex++;
            }
            index++;
        }
        else if(isDigit(data->buffer[index]))
        {
            if(isDelimiter(data->buffer[index-1]))
            {
                data->fieldType[fieldTypeIndex] = 'n';
                data->fieldPosition[fieldTypeIndex] = index;
                fieldTypeIndex++;
            }
            index++;
        }
        //delimiter
        else
        {
            //delimiters turn to NULL
            data->buffer[index] = 0;
            index++;
        }
    }
    data->fieldCount = fieldTypeIndex;
}



int32_t stringToint32(char* stringInt)
{
    uint8_t workingTemp = 0;
    int32_t sum = 0;
    int offset = 0;
    int32_t factor = 1;
    while(stringInt[offset] != '\0')
    {
        if(isDigit(stringInt[offset]))
        {
            //converts ascii numeric to real number
            workingTemp = stringInt[offset] - 48;
            workingTemp *= factor;
            factor *= 10;
            sum += workingTemp;
        }
        offset++;
    }
    return sum;
}

char* getFieldString(USER_DATA* data, uint8_t fieldNumber)
{
    if(fieldNumber > data->fieldCount)
    {
        return NULL;
    }
    if(data->fieldType[fieldNumber] != 'a')
    {
        return NULL;
    }

    return &data->buffer[data->fieldPosition[fieldNumber]];
}

int32_t getFieldInteger(USER_DATA* data, uint8_t fieldNumber)
{

    if(fieldNumber > data->fieldCount)
        {
            return NULL;
        }
        if(data->fieldType[fieldNumber] != 'n')
        {
            return NULL;
        }

        return stringToint32(&data->buffer[data->fieldPosition[fieldNumber]]);
}
//
bool isCommand(USER_DATA* data, const char strCommand[], uint8_t minArguments)
{
    if(data->fieldCount < minArguments)
    {
        return false;
    }
    uint8_t index = 0;
    while(1)
    {
        //data->buffer[data->fieldPosition[0] + index] accounts for initial offset of first command
        //in case delimiter first command
        if(strCommand[index] != data->buffer[data->fieldPosition[0] + index])
        {
            return false;
        }
        if(strCommand[index] == '\0' && data->buffer[data->fieldPosition[0] + index] == '\0')
        {
            return true;
        }
        index++;
    }

}



bool stringEqual(char* command, char* input)
{
    uint8_t index = 0;
        while(1)
        {
            //data->buffer[data->fieldPosition[0] + index] accounts for initial offset of first command
            //in case delimiter first command
            if(command[index] != input[index])
            {
                return false;
            }
            if(command[index] == '\0' && input[index] == '\0')
            {
                return true;
            }
            index++;
        }
}

//void alert(char* command)
//{
//    if(stringEqual(command,"ON"))
//    {
//        //turn on green
//        RED_LED = 0;
//        GREEN_LED = 1;
//    }
//    else if(stringEqual(command,"OFF"))
//    {
//        //turn off
//        RED_LED = 0;
//        GREEN_LED = 0;
//    }
//    else
//    {
//        //turn red
//        RED_LED = 1;
//        GREEN_LED = 0;
//    }
//}
//
//void set(int state)
//{
//    if(state == 1)
//    {
//        //turn on green
//        RED_LED = 0;
//        GREEN_LED = 1;
//    }
//    else if(state == 0)
//    {
//        //turn off
//        RED_LED = 0;
//        GREEN_LED = 0;
//    }
//    else
//    {
//        //turn red
//        RED_LED = 1;
//        GREEN_LED = 0;
//    }
//}
