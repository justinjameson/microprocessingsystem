#include <msp430.h>
#include <lcdLib.h>
int rd[1], sec = 0, tic = 0 , sleep = 1;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    lcdInit();
    lcdClear();
    lcdSetText("Sleeping....zZzZ", 0,0);

    P1DIR = 0x41;
    P1OUT = 0x40;
    ADC10CTL1 = INCH_1;
    ADC10CTL0 = ADC10ON+ADC10IE;
    ADC10SA = 1;
    ADC10AE0 = BIT1;

    TA0CTL = TASSEL_2+MC_1;
    TA0CCR0 = 20000-1;
    TA0CCTL0 = CCIE;


    ADC10CTL0 |= ENC+ADC10SC;
    __enable_interrupt();

    while(1){
        if(sleep == 1)
            LPM0;
    }
}

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void){
    rd[0] = ADC10MEM;
    if(rd[0] > 650){
        sec = 0;
        lcdClear();
        lcdSetText("Showtime!", 0,0);
        P1OUT = 0x01;
        LPM0_EXIT;
        sleep = 0;
    }
    ADC10CTL0 |= ENC+ADC10SC;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TA0_0ISR (void){

    tic++;
    if(tic == 50){
        tic = 0;
        sec++;
    }
    if(sec == 10){
        sec = 0;
        lcdClear();
        lcdSetText("Sleeping....zZzZ", 0,0);
        P1OUT = 0x40;
        sleep = 1;

    }

}
