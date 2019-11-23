;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
           .cdecls C,LIST,"msp430.h"       ; Include device header file

;-------------------------------------------------------------------------------
           .def    RESET                   ; Export program entry-point to
                                      ; make it known to linker.
COUNT   .equ  125
   .data
  .bss  cnt,1

;-------------------------------------------------------------------------------
           .text                           ; Assemble into program memory.
           .retain                         ; Override ELF conditional linking
                                           ; and retain current section.
           .retainrefs                     ; And retain any sections that have
                                           ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDT_MDLY_8,&WDTCTL  ; Stop watchdog timer

sec   .equ R4
min   .equ R5
hr   .equ R6
change  .equ R7
;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------

  mov  #7,hr
  mov  #30,min
  mov  #0,sec
;   clr  sec
;   clr  min
;   clr  hr
  mov  #5,change

  bis.b #0xFF,&P1DIR
  clr.b &P1OUT

  mov.b #0x07,&P2IE
  clr.b &P1IES
  clr.b &P1IFG

  mov.b  #WDTIE,&IE1
  mov  #COUNT,&cnt

loop:
  cmp  #5,change
  jeq  showhour

  cmp  #3,change
  jge  showmin

showsec: mov.b sec,&P1OUT
  bis.b #BIT6,&P1OUT
  jmp  skip

showmin:
  mov  min,&P1OUT
  jmp  skip

showhour:
  mov  hr,&P1OUT
  bis  #BIT7,&P1OUT


skip:  cmp  #60,sec
  jne  minutes
  mov  #0,sec
  inc  min

minutes:
  cmp  #60,min
  jne  hours
  mov  #0,min
  inc  hr

hours:
  cmp  #24,hr
  jne  sleep
  mov  #0,hr

sleep:  bis  #LPM1+GIE,SR
  jmp  loop


; Watchdog Timer interrupt service routine
WDT_ISR:
  bic.w  #LPM1,0(SP)
  dec.w &cnt
   jne  WDT_exit
  mov.w  #COUNT,&cnt
  inc  sec
  dec  change
  cmp  #-1,change
  jne  WDT_exit
  mov  #5,change
WDT_exit:  reti

P2_ISR:
  bit.b #0x04,&P2IFG
  jnz  addhour

  bit.b #0x02,&P2IFG
  jnz  addminute

  bit.b #0x01,&P2IFG
  jnz  add10minutes



addhour:
  inc  hr
  clr.b &P2IFG
  reti
addminute:
  inc  min
  clr.b &P2IFG
  reti
add10minutes:
  add  #10,min
  clr.b &P2IFG
  reti
;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
           .global __STACK_END
           .sect   .stack

;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
  .sect PORT2_VECTOR
  .word P2_ISR
  .sect  ".int10"
  .word  WDT_ISR
           .sect   ".reset"                ; MSP430 RESET Vector
           .short  RESET
  .end
