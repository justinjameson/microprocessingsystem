#include "lcdLib.h"

#define LOWNIB(x)   P2OUT = (P2OUT & 0xF0) + (x & 0x0F)

void lcdInit() {
    delay_ms(100);
    // Wait for 100ms after power is applied.

    P2DIR = EN + RS + DATA;  // Make pins outputs
    P2OUT = 0x03;  // Start LCD (send 0x03)

    lcdTriggerEN(); // Send 0x03 3 times at 5ms then 100 us
    delay_ms(5);
    lcdTriggerEN();
    delay_ms(5);
    lcdTriggerEN();
    delay_ms(5);

    P2OUT = 0x02; // Switch to 4-bit mode
    lcdTriggerEN();
    delay_ms(5);

    lcdWriteCmd(0x28); // 4-bit, 2 line, 5x8
    lcdWriteCmd(0x08); // Instruction Flow
    lcdWriteCmd(0x01); // Clear LCD
    lcdWriteCmd(0x06); // Auto-Increment
    lcdWriteCmd(0x0C); // Display On, No blink
}

void lcdTriggerEN() {
    P2OUT |= EN;
    P2OUT &= ~EN;
}

void lcdWriteData(unsigned char data) {
    P2OUT |= RS; // Set RS to Data
    LOWNIB(data >> 4); // Upper nibble
    lcdTriggerEN();
    LOWNIB(data); // Lower nibble
    lcdTriggerEN();
    delay_us(50); // Delay > 47 us
}

void lcdWriteCmd(unsigned char cmd) {
    P2OUT &= ~RS; // Set RS to Data
    LOWNIB(cmd >> 4); // Upper nibble
    lcdTriggerEN();
    LOWNIB(cmd); // Lower nibble
    lcdTriggerEN();
    delay_ms(5); // Delay > 1.5ms
}

void lcdSetText(char* text, int x, int y) {
    int i;
    if (x < 16) {
        x |= 0x80; // Set LCD for first line write
        switch (y){
        case 1:
            x |= 0x40; // Set LCD for second line write
            break;
        case 2:
            x |= 0x60; // Set LCD for first line write reverse
            break;
        case 3:
            x |= 0x20; // Set LCD for second line write reverse
            break;
        }
        lcdWriteCmd(x);
    }
    i = 0;

    while (text[i] != '\0') {
        lcdWriteData(text[i]);
        i++;
    }
}

void lcdSetInt(int val, int x, int y){
    char number_string[16];
    sprintf(number_string, "%d", val); // Convert the integer to character string
    lcdSetText(number_string, x, y);
}

void lcdClear() {
    lcdWriteCmd(CLEAR);
}

#ifndef LCDLIB_H_
#define LCDLIB_H_

#include <msp430g2553.h>
#include <string.h>
#include <stdio.h>

// Delay Functions
#define delay_ms(x)     __delay_cycles((long) x* 1000)
#define delay_us(x)     __delay_cycles((long) x)

// Pins
#define EN      BIT4
#define RS      BIT5
#define DATA    0x0F

// Commands
#define CLEAR   0x01

// Functions
void lcdInit();                                 // Initialize LCD
void lcdTriggerEN();                            // Trigger Enable
void lcdWriteData(unsigned char data);          // Send Data (Characters)
void lcdWriteCmd(unsigned char cmd);            // Send Commands
void lcdClear();                                // Clear LCD
void lcdSetText(char * text, int x, int y);     // Write string
void lcdSetInt(int val, int x, int y);          // Write integer

#endif /* LCDLIB_H_ */

#include <msp430g2553.h>

#include <lcdLib.h>
unsigned short
int tic = 0, sec = 0, min = 0;
 //Example of Global variables.

int main (void)
{
    //Setup
    WDTCTL = WDTPW + WDTHOLD; //Stop Watchdog Timer

   lcdInit(); // Initialize LCD

lcdClear();
  //Setup Buttons

P1DIR &= ~0x07;

P1REN = 0; //tun off my resistor

P1OUT = 0; //turn off all my outputs

P1IE = 0x07;

P1IES &= ~0x07; // make 1.0 - 1.3 interrupts 0 so that we wait for a one

P1IFG = 0;
    //Setup Timer A0

TA0CTL = TASSEL_2 + MC_1; //Setup Timer/Start Timer

TA0CCTL0 = CCIE; //Enable interrupt on TA0.0

TA0CCR0 = 1001;//Period of 50ms or whatever interval you like.

_enable_interrupts(); //Enable General Interrupts. Best to do this last.

 while (1)
    {

    char text[16];


      sprintf(text,"Time: %d:%d:%d",min,sec, tic);

      lcdSetText(text,0,0);


       LPM1;

  }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{

if (P1IFG & BIT0)TA0CTL |= MC_1;

    if (P1IFG & BIT1)TA0CTL &= ~MC_1;

   if (P1IFG & BIT2){

   tic = 0;

        sec = 0;

       min = 0;

       lcdClear();

      char text[16];


       sprintf(text,"Time: %d:%d:%d",min,sec, tic);

       lcdSetText(text,0,0);
}
    P1IFG = 0;
}
// Timer 0 A0 Interrupt Service Routine

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR( void )
{

   LPM1_EXIT;

   tic++;

    if (tic == 1000){

tic = 0;

       sec++;

      if (sec == 60){

sec = 0;

        min++;

     }

 }
}
