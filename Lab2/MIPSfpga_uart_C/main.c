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

//开关地址
#define IO_SW (IO_LEDR+8)

void delay();

//------------------
// main()
//------------------
int main() {
	volatile unsigned int pushbutton, count = 0xF;
	volatile unsigned int j = 1;
	volatile unsigned int k = 1;
	volatile unsigned int flag = 1;

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

	while(1) {

		for(flag = 0;flag<10;flag++){
			//回显键盘输入
			j = *READ_IO(UART_BASE + rbr);
			*WRITE_IO(UART_BASE + thr) = j;
			delay();
		}

/*
		flag = *READ_IO(UART_BASE + rbr);
		if(flag=='.'){//输入句号开始键盘读取
			//回显键盘输入
			j = *READ_IO(UART_BASE + rbr);
			*WRITE_IO(UART_BASE + thr) = j;
			delay();
		}
*/
		
		//回显开关输入
		k = *READ_IO(IO_SW);
		if(k<=0x09 && k>=0){//低四位开关16进制回显数字0-9
			*WRITE_IO(UART_BASE + thr) = k + 0x0030;
			delay();
		}
		else if(k>0xff && k<0x8000){//数值大于0xff，低八位显示A之后的字符，0x100显示A,以此类推
									//LED15作为控制输出位，大于0x8000暂停输出
			*WRITE_IO(UART_BASE + thr) = k - 0x100 + 0x41;
			delay();
		}
		//LED指示灯亮
		*WRITE_IO(IO_LEDR) = *READ_IO(IO_SW);
		

/*
		*WRITE_IO(UART_BASE + thr) = 0x0000000a; // Ascii \n in transmit
		delay();
		*WRITE_IO(UART_BASE + thr) = 0x0000000d; // Ascii \r in transmit
		delay();
		*WRITE_IO(UART_BASE + thr) = 0x0000004D; // Ascii M in transmit
		delay();
		*WRITE_IO(UART_BASE + thr) = 0x00000049; // Ascii I in transmit
		delay();
		*WRITE_IO(UART_BASE + thr) = 0x00000050; // Ascii P in transmit
		delay();
		*WRITE_IO(UART_BASE + thr) = 0x00000053; // Ascii S in transmit
		delay();
		*WRITE_IO(UART_BASE + thr) = 0x00000066; // Ascii f in transmit
		delay();
		*WRITE_IO(UART_BASE + thr) = 0x00000070; // Ascii p in transmit
		delay();
		*WRITE_IO(UART_BASE + thr) = 0x00000067; // Ascii g in transmit
		delay();
		*WRITE_IO(UART_BASE + thr) = 0x00000061; // Ascii a in transmit
		delay();
		*WRITE_IO(UART_BASE + thr) = 0x0000000a; // Ascii \n in transmit
		delay();
		*WRITE_IO(UART_BASE + thr) = 0x0000000d; // Ascii \r in transmit
		delay();
*/

/*
		//跑马灯
		*WRITE_IO(IO_LEDR) = count;

		if (j == 1) {
		    count = count << 1;
		    if (count == 0xf000)
		        j = 0;
		} else {
				count = count >> 1;
		    if (count == 0x000f)
		        j = 1;
		}
*/
		inline_assembly();	  
	}
	return 0;
}

void delay() {
	volatile unsigned int j;

	for (j = 0; j < (10000); j++) ;	// delay 
}

