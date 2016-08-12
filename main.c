/* 
 * File:   main.c
 * Author: Jordon
 *
 * Created on June 19, 2016, 10:51 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#pragma config FOSC = INTOSC    //  (INTOSC oscillator; I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOREN = OFF    // Low Power Brown-out Reset enable bit (LPBOR is disabled)
#pragma config LVP = OFF         // Low-Voltage Programming Enable (Low-voltage programming enabled)

#define _XTAL_FREQ 8000000      //FREQUENCY OF EXTERNAL EXTERNAL OSCILLATOR
#define Nop() {_asm nop _endasm}        //Can use for delay without using CPU processing
/*
 * VARIABLES USED THROUGHOUT PROGRAM
*/
int data = 0x00; //Data shifted in from HC-06
bit poll = 0; //Boolean used to read in the PORT A
int mask = 0xFF; //byte used for bit masking
int y, t;       //Variables used in for loops
int temp_data;      //Byte used when shifting in 8 bits from HC-06
int counter = 0;        //Counter used for creating a "dimming" effect

//For each button that is pressed assign to it a number used later for send RGBA data to embedded IC chip
int charging = 0x35;
int red = 0x33;
int blue = 0x32;
int green = 0x31;
int white = 0x30;

//For the byte received using USART
bit bit1;
bit bit2;
bit bit3;
bit bit4;
bit bit5;
bit bit6;
bit bit7;
bit bit8;

//
// METHOD FOR SENDING BITS FOR THE NEOPIXEL
//

void zeroBit(void){     //Logical "0" bit based off the data sheet for the neopixel
    LATAbits.LATA1 = 1;     //
    LATAbits.LATA1 = 1;     //
    //InterData Bit
    LATAbits.LATA1 = 0;
    LATAbits.LATA1 = 0;
    LATAbits.LATA1 = 0;
    LATAbits.LATA1 = 0;
}

void oneBit(void){      //Logical "1" bit based off the data sheet for the neopixel
    LATAbits.LATA1 = 1;
    LATAbits.LATA1 = 1;
    LATAbits.LATA1 = 1;
    LATAbits.LATA1 = 1;
    LATAbits.LATA1 = 1;
    //InterData Bit
    LATAbits.LATA1 = 0;
    LATAbits.LATA1 = 0;
    LATAbits.LATA1 = 0;
    LATAbits.LATA1 = 0;
}

//Latching is used after sending the data through the LEDs and is basically a delay of sending data
void latchData(){
    __delay_ms(6);
}

//Function sends the bits using a mask to get each bit value
void sendData(int data){
	for(t = 0;t < 8;t++){
		if (data & 0x80) {
			oneBit();
		} else {
			zeroBit();
		}
		 data <<= 1;
	}
}

/*
 Main Method
 */
int main() {
    
    TRISAbits.TRISA2 = 1; //Set PIN2 as input for bluetooth receive
    
    TRISAbits.TRISA1 = 0; //Used to display what was retrieved from HC-06
    
    OSCCON = 120; //112v 120=112/(2)
    //OSCCONbits.SCS1 = 1; // internal clock
    
    //OPTION_REG = 201 // Configures for the clock and timer initialization
    //INTCONbits.T0IE = 1 // Enable interrupt on TMR0 overflow
    INTCONbits.INTF = 0; //Reset external interrupt
    
    INTCONbits.INTE = 1; // enable the external interrupt
    
    INTCONbits.GIE = 1; // Global interrupt enable
    
    OPTION_REGbits.INTEDG = 0; //Trigger on falling edge
    
    ADCON1 = 0x00; //Set ports as digital I/O, not analog input
    
    ANSELA = 0x00; //Shut off A/D Converter
    
    CMOUT = 0x00; //Shut off the comparator
    
    VREGCON = 0x00; //Shut off voltage reference
    
    LATAbits.LATA1 = 0;
    
     while(1){
           //This creates a dimming effect seen by the viewer
         if(temp_data == charging){
             counter++;
             sendData(0);
             sendData(0);
             sendData(0);
             sendData(counter);
             latchData();       //Latches the serial data
             __delay_ms(50);
             if(counter == 255) counter = 0;
         }
     }
}
 
void interrupt fallingEdge()        // interrupt function 
 {
        if(INTCONbits.INTF) 
        {                                     // if timer flag is set & interrupt enabled
            __delay_us(200);
            __delay_us(100);
            bit1 = PORTAbits.RA2;       //Read the bit on PORTA
            __delay_us(200);
            bit2 = PORTAbits.RA2;       //Read the bit on PORTA
            __delay_us(200);
            bit3 = PORTAbits.RA2;       //Read the bit on PORTA
            __delay_us(200);
            bit4 = PORTAbits.RA2;       //Read the bit on PORTA
            __delay_us(200);
            bit5 = PORTAbits.RA2;       //Read the bit on PORTA
            __delay_us(200);
            bit6 = PORTAbits.RA2;       //Read the bit on PORTA
            __delay_us(200);
            bit7 = PORTAbits.RA2;       //Read the bit on PORTA
            __delay_us(200);
            bit8 = PORTAbits.RA2;       //Read the bit on PORTA
            temp_data = bit8<<7|bit7<<6|bit6<<5|bit5<<4|bit4<<3|bit3<<2|bit2<<1|bit1;
            
            //Values are sent RBGA not RGBA, odd
            if(temp_data == red){       //If temp_data represents all Red
                sendData(0);
                sendData(255);
                sendData(0);
                sendData(0);
                latchData();
            }
            
            if(temp_data == blue){      //If temp_data represents all Blue
                sendData(0);
                sendData(0);
                sendData(255);
                sendData(0);
                latchData();
            }
            
            if(temp_data == green){     //If temp_data represents all Green
                sendData(255);
                sendData(0);
                sendData(0);
                sendData(0);
                latchData();
            }
            
            if(temp_data == white){     //If temp_data represents all White
                sendData(0);
                sendData(0);
                sendData(0);
                sendData(255);
                latchData();
            }
            
            INTCONbits.INTF = 0; 
        }
}
 

