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

#define IS_UART_INTR (1<<15)//uart和时钟中断共用HW5端口
#define IS_PWM_INTR (1<<14)//pwm使用HW4端口
#define IS_TIMER_INTR (1<<30)

void delay();
void uart_outbyte(char c);
char uart_inbyte(void);
void uart_print(const char *ptr);

char *reverse(char *s);
char *my_itoa(int n);

volatile unsigned int rxData = 0x0;
volatile unsigned int data_received = 0x0;

unsigned char *promt = "Select a Brightness between 0 and 9\n\r";

//------------------
// main()
//------------------
int main() {
	volatile unsigned int count = 0xF;
	volatile unsigned int j;
	volatile unsigned int period;

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
	
	/* Prompt the user to select a brightness value ranging from  0 to 9. */
	//uart_print("Select a Brightness between 0 and 9\n\r");

	while(1) {
		// LEDs display
		*WRITE_IO(IO_LEDR) = count;
		
		count = count + 1;
		
		for(j=0; j<1000; j++)
			delay();

		if(data_received){
			period = rxData - 0x30;
			*WRITE_IO(PWM_BASE) = period * 110000;//大于990000触发pwm产生中断信号
			data_received = 0x0;
			
			delay();
		}
	}
	
	return 0;
}

void delay() {
	volatile unsigned int j;
	//for (j = 0; j < (100); j++) ;	// delay
	for (j = 0; j < (50); j++) ;	// delay
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

//反转字符串  
char *reverse(char *s)  
{  
    char temp;  
    char *p = s;    //p指向s的头部  
    char *q = s;    //q指向s的尾部  
    while(*q)  
        ++q;  
    q--;  
  
    //交换移动指针，直到p和q交叉  
    while(q > p)  
    {  
        temp = *p;  
        *p++ = *q;  
        *q-- = temp;  
    }  
    return s;  
}  
  
/* 
 * 功能：整数转换为字符串 
 * char s[] 的作用是存储整数的每一位 
 */  
char *my_itoa(int n)  
{  
    int i = 0,isNegative = 0;  
    static char s[100];      //必须为static变量，或者是全局变量  
    if((isNegative = n) < 0) //如果是负数，先转为正数  
    {  
        n = -n;  
    }  
    do      //从各位开始变为字符，直到最高位，最后应该反转  
    {  
        s[i++] = n%10 + '0';  
        n = n/10;  
    }while(n > 0);  
  
    if(isNegative < 0)   //如果是负数，补上负号  
    {  
        s[i++] = '-';  
    }  
    s[i] = '\0';    //最后加上字符串结束符  
	
    return reverse(s);    
}  


void _mips_handle_irq(void* ctx, int reason) {
	volatile unsigned int period;
	//volatile unsigned int rxData;
	
	*WRITE_IO(IO_LEDR) = 0xF00F;  // Display 0xF00F on LEDs to indicate enter the interrupt
	data_received = 0x0;

	if(reason & IS_TIMER_INTR){
		asm("mtc0 $0, $11");
		
		asm("li $9, 0x1");
		asm("mtc0 $9, $9");
		return;
	}
	else if(reason & IS_UART_INTR) {
		rxData = *READ_IO(UART_BASE + rbr);
		data_received = 0x1;
		return;
	}

	else if(reason & IS_PWM_INTR){
		*WRITE_IO(PWM_BASE) = 0;
		return;
	}

	else {
		uart_print("Other interrupts occurred!\n\r");
	}
    
	return;
}


/*
void _mips_handle_irq(void* ctx, int reason) {
	volatile unsigned int period;
	//volatile unsigned int rxData;
	
	*WRITE_IO(IO_LEDR) = 0xF00F;  // Display 0xF00F on LEDs to indicate enter the interrupt
	data_received = 0x0;
	uart_print("Here is mips interrupt handle\r\n");//debug

	//asm("mtc0 $0, $11");

	if(reason & IS_TIMER_INTR){//时钟中断，HW5,IP7,bit15，(1<<15)
		asm("mtc0 $0, $11");//compare register 置0
		
		asm("li $9, 0x1");
		asm("mtc0 $9, $9");//count register 置1

		uart_print("Here is in Timer interupt\n\r");//debug
	}

	else if(reason & IS_UART_INTR) {//UART中断，HW5,IP7,bit15，(1<<15)
		// Read an input value from the console. 
		rxData = *READ_IO(UART_BASE + rbr);
		data_received = 0x1;

		uart_print("Here is in UART interupt,rxData is ");//debug
		*WRITE_IO(UART_BASE + thr) = rxData;//debug
		uart_print("\n\r");//debug

		asm("mtc0 $0, $11");
		
		asm("li $9, 0x1");
		asm("mtc0 $9, $9");

	}

	else if(reason & IS_PWM_INTR){//PWM中断，HW4,IP6,bit14，(1<<14)
		*WRITE_IO(PWM_BASE) = 0 * 110000;
		data_received = 0x0;//pwm
		uart_print("Here is in PWM interupt\n\r");//debug
	}
		
	else{
		*WRITE_IO(IO_LEDR) = 0x0FF0;
		uart_print("Other interrupts occurred!\n\r");
		uart_print(my_itoa(reason));
		uart_print("\r\n");
	}
    
	return;
}
*/