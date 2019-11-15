/*
 * main.c for microAptiv_UP MIPS core running on Nexys4DDR
 * Prints \n\rMIPSfpga\n\r out via UART.
 * Default baudrate 115200,8n1.
 * Also display a shifting 0xf on the LEDs
 */

#include "fpga.h"

#define inline_assembly()  asm("ori $0, $0, 0x1234")

#define CLK_FREQ 50000000

#define UART_BASE 0xB0401000	//With 1000 offset that axi_uart16550 has
#define rbr		0*4
#define ier		1*4
#define fcr		2*4
#define lcr		3*4
#define mcr		4*4
#define lsr		5*4
#define msr		6*4
#define scr		7*4

#define thr		rbr
#define iir		fcr
#define dll		rbr
#define dlm		ier

#define PWM_BASE 0xB0C00000
#define F_DATA_BASE 0xB0F00000

void delay();
void uart_outbyte(char c);
char uart_inbyte(void);
void uart_print(const char *ptr);

extern char *reverse(char *s);
extern char *my_itoa(int n);

extern void delay_ms(unsigned int ms_count);
void LED_Display();
void Seg_Display_Temp(short int data);

// The following is for ADT7420
#include "i2c.h"
#include "ADT7420.h"

/*****************************************************************************/
/********************** Variable Definitions *********************************/
/*****************************************************************************/
volatile int rxData = 0;


//------------------
// main()
//------------------
int main() {
	volatile unsigned int pushbutton, count = 0xF;
	volatile unsigned int j = 1;

	*WRITE_IO(UART_BASE + lcr) = 0x00000080; // LCR[7]  is 1
	delay();
	*WRITE_IO(UART_BASE + dll) = 27; // DLL msb. 115200 at 50MHz. Formula is Clk/16/baudrate. From axi_uart manual.
	delay();
	*WRITE_IO(UART_BASE + dlm) = 0x00000000; // DLL lsb.
	delay();
	*WRITE_IO(UART_BASE + lcr) = 0x00000003; // LCR register. 8n1 parity disabled
	delay();
	*WRITE_IO(UART_BASE + ier) = 0x00000000; // IER register. disable interrupts
	delay();
		
	*WRITE_IO(UART_BASE + ier) = 0x00000001; // IER register. Enables Receiver Line Status and Received Data Interrupts
	delay();
	
	// Begin ADT7420 test
	// Initialize ADT7420 Device
	ADT7420_Init();
    
	// Display Main Menu on UART
	ADT7420_DisplayMainMenu();
	
	while(rxData != 'q')
    {
    	//uart_print("rxData = ");
		//uart_print(my_itoa(rxData));
		//uart_print("\n\r");
		/*开关控制LED*/
		LED_Display();
		
		/*数码管显示温度*/
		Seg_Display_Temp(ADT7420_ReadTemp());

		/*温度显示*/
		Display_Temp(ADT7420_ReadTemp());
		
		switch(rxData)
    	{
    	case 't':
    		Display_Temp(ADT7420_ReadTemp());
    		break;
    	case 'r':
    		ADT7420_SetResolution();
    		break;
    	case 'h':
    		ADT7420_DisplaySetTHighMenu();
    		break;
    	case 'l':
    		ADT7420_DisplaySetTLowMenu();
    		break;
    	case 'c':
    		ADT7420_DisplaySetTCritMenu();
    		break;
        case 'y':
        	ADT7420_DisplaySetTHystMenu();
    		break;
    	case 'f':
    		ADT7420_DisplaySetFaultQueueMenu();
    		break;
    	case 's':
			ADT7420_DisplaySettings();
			break;
    	case 'm':
    		ADT7420_DisplayMenu();
    		break;
    	case 0:
    		break;
    	default:
    		uart_print("\n\rWrong option! Please select one of the options below.");
    		ADT7420_DisplayMenu();
    		break;
    	}
    }
	
	uart_print("Exiting ADT7420 test application!\n\r");
			
	return 0;
}

void delay() {
	volatile unsigned int j;

	for (j = 0; j < (10000); j++) ;	// delay 
}

void uart_outbyte(char c) {
	*WRITE_IO(UART_BASE + thr) = (unsigned int) c;
	delay( );
}

char uart_inbyte(void) {
	unsigned int RecievedByte;
	
	while(!((*READ_IO(UART_BASE + lsr) & 0x00000001)==0x00000001));

	RecievedByte = *READ_IO(UART_BASE + rbr);

	return (char)RecievedByte;
}

void uart_print(const char *ptr)
{
	while (*ptr) {
		uart_outbyte (*ptr);
		ptr++;
	}
}

void _mips_handle_irq(void* ctx, int reason)
{	
	*WRITE_IO(IO_LEDR) = 0xF00F;  // Display 0xFFFF on LEDs to indicate receive data from uart
	
	if((*READ_IO(UART_BASE + lsr) & 0x00000001)==0x00000001)
		rxData = *READ_IO(UART_BASE + rbr);
	
	*WRITE_IO(IO_LEDR) = 0x0;
	
	return;
}

void LED_Display(){
	volatile unsigned int count;
	count = *READ_IO(IO_LEDR+8);//读取开关数据,控制三色灯
	switch (count)
	{
	case 0x0000:
		*WRITE_IO(IO_LEDR) = 0x0000;
		*WRITE_IO(PWM_BASE) = 0 * 110000;
		break;

	case 0x000F://SW[15:0]=0X000F,
		*WRITE_IO(IO_LEDR) = 0xFFFF;//LED[15:0]=FFFF
		*WRITE_IO(PWM_BASE) = 2 * 110000;
		break;
		
	case 0x00FF:
		*WRITE_IO(IO_LEDR) = 0xF0F0;
		*WRITE_IO(PWM_BASE) = 4 * 110000;
		break;

	case 0X0FFF:
		*WRITE_IO(IO_LEDR) = 0x0FF0;
		*WRITE_IO(PWM_BASE) = 8 * 110000;
		break;

	default:
		*WRITE_IO(IO_LEDR) = 0x000F;
		*WRITE_IO(PWM_BASE) = 0 * 110000;
		break;
	}
	return;
}

void Seg_Display_Temp(short int data)
{
	int     value     = 0;

	// converting data for display
	if(ADT7420_GetResolution(0) == 0)
	{
		if(data&0x1000)
		{
			data = data	| 0xffffe000;
		}
		value = data / 16;
	}
	else
	{
		value = data / 128;
	}

	if(value >= 0)
	{
		*WRITE_IO(F_DATA_BASE) = (volatile unsigned int)value;
	}
	return;
}