/*
 * QUTMS_ShutdownBoard_Firmware.c
 *
 * Created: 18/09/2019 12:51:26 PM
 * Author : Zoe Goodward
 */

#define F_CPU 16000000UL /* CPU clock in Hertz */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "input.h"
#include "UART.h"
#include "spi.h"
#include "adc.h"
#include "MCP2517.h"
#include "MCP2517_reg.h"

uint8_t SHUTDOWN_BOARD_DATA[5] = {0};

/*============================================================================
Function:   shutdown_board_init()
------------------------------------------------------------------------------
Purpose :   consolidates all the functions required for the initialisation of
			the Shutdown board
Input   :   none
Returns :   void
Notes   :
============================================================================*/
void shutdown_board_init() 
{
	DDRD = 0b11000010; // LEDs as outputs
	DDRB = 0b10111000; // MOSI and SCK and CAN_CS as output, SS output

	CAN_CS_PORT |= (1<<CAN_CS); // CS high to turn off
	
	adc_init();
	uart0_init(9600);
	spi_init(0,0); // 1,0
	MCP2517_init();
	sei(); // Enable interrupts
}

int main(void)
{
    shutdown_board_init();
	
	SHUTDOWN_BOARD_DATA[0] = 0x00;
	SHUTDOWN_BOARD_DATA[1] = 0x01;
	SHUTDOWN_BOARD_DATA[2] = 0x02;
	SHUTDOWN_BOARD_DATA[3] = 0x03;
	SHUTDOWN_BOARD_DATA[4] = 0x04;
	
	uint8_t data[8] = {0};
	uint32_t receiveID;
	uint8_t numDataBytes;
	
    while(1) 
    {
		MCP2517_recieveMessage(&receiveID, &numDataBytes, data);
		if(receiveID == CAN_ID_PDM >> 18) {
			LED_A_ON;		
			MCP2517_transmitMessage(CAN_ID_WHEEL, 5, SHUTDOWN_BOARD_DATA);
			_delay_ms(100);
			LED_A_OFF;
		}
			
		_delay_ms(200);
    }
}

