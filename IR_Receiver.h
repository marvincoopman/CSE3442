// Marvin Coopman
// 1001781933
// IR_Receiver

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

#ifndef IR_RECEIVER_H_
#define IR_RECEIVER_H_


//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

typedef struct _button{
    uint8_t add;
    uint8_t data;
    char name[50];
}button;

void initIR();
void enableWTimer3();
void createCode();
void wideTimer3Isr();
char* findButton(button* button1);
bool isValid();
void clearCode();



#endif /* IR_RECEIVER_H_ */
