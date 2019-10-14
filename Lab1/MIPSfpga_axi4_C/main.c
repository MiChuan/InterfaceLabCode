/*
 * main.c for microAptiv_UP MIPS core running on Nexys4DDR
 * Prints \n\rMIPSfpga\n\r out via UART.
 * Default baudrate 115200,8n1.
 * Also display a shifting 0xf on the LEDs
 */

#include "fpga.h"

#define inline_assembly()  asm("ori $0, $0, 0x1234")

#define CLK_FREQ 50000000


void delay();

//------------------
// main()
//------------------
int main() {
	volatile unsigned int pushbutton, count = 0xF000;

	delay();

	while(1) {
		count = *READ_IO(IO_LEDR+8);//读取开关数据

		switch (count)
		{
		case 0x0000:
			*WRITE_IO(IO_LEDR) = 0x0000;
			break;

		case 0x000F://SW[15:0]=0X000F,
			*WRITE_IO(IO_LEDR) = 0xFFFF;//LED[15:0]=FFFF
			break;
		
		case 0x00FF:
			*WRITE_IO(IO_LEDR) = 0xF0F0;
			break;

		case 0X0FFF:
			*WRITE_IO(IO_LEDR) = 0x0FF0;
			break;

		default:
			*WRITE_IO(IO_LEDR) = 0xF00F;
			break;
		}

		inline_assembly();	  

	}


	return 0;
}

void delay() {
	volatile unsigned int j;

	for (j = 0; j < (100000); j++) ;	// delay 
}

