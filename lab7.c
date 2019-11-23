#include <msp430.h> 
#include "keypad12.h"
#include "lcdlib.h"
#include "flashmem.h"


char loc1[] = "XXXXX";
char loc2[] = "XXXXX";
char loc3[] = "XXXXX";
int *ptrl;
char *ptrs;
void retrieve(void);
void indel(void);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    _enable_interrupts();
    P1DIR &= ~(BIT3);
    P1REN |= BIT3;
    P1OUT |= BIT3;
    P1IE   |= BIT3;             //Setting up the interrupt
    P1IES  |= BIT3;             //for the onboard button
    P1IFG    = 0;

    lcdInit();
   lcdClear();

    flashConfig(FSSEL_3, FN0);
    flashInterrupts(disable, enable, disable);

    ptrl = SegmentB;
    ptrs = loc1;

    retrieve();

    keypad12Config(BIT0 + BIT1 + BIT2 + BIT4, BIT5 + BIT6 + BIT7, Port1, Port1, 10);
    volatile char gotkey  = 0, key  = 0;

    LPM0;

}

void indel(void){                   //This subroutine either erases an
    char gotkey  = 0, key[]  = " "; //entry, or rewrites it in both flash
    gotkey = getKey12(1);           //memory and updates the LCD to the current
    key[0] = standardDecode(gotkey);//number inputed.
    __delay_cycles(50000);
    if(key[0] != '*'){
        ptrs[0] = key[0];
        lcdSetText(key,0,0);
        volatile int i;
        for(i = 1; i < 5; i++){
            gotkey = getKey12(1);
            key[0] = standardDecode(gotkey);
            ptrs[i]= key[0];
            lcdSetText(key,i,0);
            __delay_cycles(50000);
        }
        while(key[0] != '#'){
            gotkey = getKey12(1);
            key[0] = standardDecode(gotkey);
            __delay_cycles(50000);
        }
        flashErase(ptrl);
        flashWrite(ptrs, ptrl, block, 2);
    }else {
        flashErase(ptrl);
    }
    retrieve();
}

void retrieve(void){    //This subroutine retrieves a value from
    volatile int temp;  //flash memory and prints it on the LCD
    flashRead((volatile int*)ptrs, ptrl, block, 2);
    if((ptrs[0] == 0xFF) && (ptrs[1] == 0xFF) && (ptrs[2] == 0xFF) && (ptrs[3] == 0xFF))
        ptrs[0]='X',ptrs[1]='X',ptrs[2]='X',ptrs[3]='X';

    lcdSetText(ptrs,0,0);
}

#pragma vector = PORT1_VECTOR   //This port interrupt is activated
__interrupt void Port_1(void){  //whenever the on board button is pressed
    if(ptrl == SegmentB){       //and rotates the the flash memory location
        ptrl = SegmentC;        //and updates the LCD.
        ptrs = loc2;
    }else if(ptrl == SegmentC){
        ptrl = SegmentD;
        ptrs = loc3;
    }else if(ptrl == SegmentD){
        ptrl = SegmentB;
        ptrs = loc1;
    }
    retrieve();
    indel();

    P1IFG = 0;
}
