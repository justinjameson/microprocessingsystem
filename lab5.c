#include <msp430g2553.h>
unsigned int go = 0, step = 0, up = 1, tic = 0; ;//Example of Global variables.
int main (void)
{
    //Setup
    WDTCTL = WDTPW + WDTHOLD;//Stop Watchdog Timer

    //Setup P1.3
    P1DIR &= ~BIT3;
    P1REN |= BIT3;
    P1OUT |= BIT3;

    //Setup LEDs
    P1DIR |= BIT0 + BIT6;
    P1OUT |= BIT0;
    P1OUT &= ~BIT6;

    //Setup PWM - Timer A0.1 - P1.2
    P1DIR |= 0x40;
    P1SEL |= 0x40;
    P1SEL2 &= ~0x40;


    TA0CTL = TASSEL_2+MC_0;//Setup Timer
    TA0CCTL0 = CCIE;
    //TA0CTL ^= //Stop Timer Here (This and previous line can be combined).
    TA0CCTL1 = OUTMOD_7;//??
    TA0CCR0 = 20001;//Period of 20ms (50Hz)
    TA0CCR1 = 4000;//Duty Cycle of 1ms (5%)



//    //Setup Timer A1.0
//    TA1CTL = //Setup Timer
//    TA1CTL ^= //Start Timer Here (This and previous line can be combined).
//    TA1CCTL0 = //Enable Timer Capture Compare Interrupt
//    TA1CCR0 = // 50000 clock cycles

    //Setup Port 1 Interrupts
    P1IE |= BIT3;
    P1IES |= BIT3;
    P1IFG = 0;
    _enable_interrupts(); //Enable General Interrupts. Best to do this last.

    while (1){} //Empty infinite while loop.
}
// Port 1 Interrupt Service Routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
if(P1IFG&BIT3){
    __delay_cycles(50000);
    go ^= 0x01;
    if (go == 0){
     TA0CTL &= ~MC_1;
     P1OUT |= 0x01;
     P1OUT &= ~0x40;
    }
    if (go == 1){
         TA0CTL |= MC_1;
         P1OUT |= 0x40;
         P1OUT &=~0x01;
        }
}
P1IFG &= ~BIT3; // Clear Button Flag
}
// Timer 1 A0 Interrupt Service Routine
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR( void )
{

if(up == 1){
    TA0CCR1 = 1000;
}
if(up == 0){
    TA0CCR1 = 4000;
}
tic++;
if (tic == 100){
    up ^=0x01;
    tic = 0;
}


}
