#include "defines.h"        // LCD, TIMER and UART addresses
#include "string.c"         // strcmp, strlen, etc
#include "vid.c"            // LCD driver file
#include "timer.c"          // timer handler file
#include "kbd.c"
#include "exceptions.c"     // other exception handlers
#include "uart.c"

void copy_vectors(void)
{
    // copy vector table in ts.s to 0x0
    extern u32 vectors_start, vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;
    while (vectors_src < &vectors_end)
        *vectors_dst++ = *vectors_src++;
}


void timer_handler();
TIMER *tp[4];           // 4 TIMER structure pointers

void IRQ_handler()    // IRQ interrupt handler in C
{
    // read VIC status registers to find out which interrupt
    int vicstatus = VIC_STATUS;
    int sicstatus = SIC_STATUS;
    // VIC status BITs: timer0,1=4,uart0=12,uart1=13
    if(vicstatus & (1<<5))
    {
        // bit5:timer2,3
        if (*(tp[2]->base+TVALUE)==0) // timer 2
            timer_handler(2);
        if (*(tp[3]->base+TVALUE)==0) // timer 3
            timer_handler(3);
    }
    if(vicstatus & (1<<4))
    {
        // bit4=1:timer0,1
        if (*(tp[0]->base+TVALUE)==0) // timer 0
            timer_handler(0);
        if (*(tp[1]->base+TVALUE)==0) // timer 1
            timer_handler(1);
    }

    if(vicstatus & (1<<12))            // bit12=1: uart0
        uart_handler(&uart[0]);

    if(vicstatus & (1<<13))           // bit13=1: uart1
        uart_handler(&uart[1]);
    
    if (vicstatus & (1<<31))
    {
        
        // PIC.bit31= SIC interrupts
        if(sicstatus & (1<<3))
        { // SIC.bit3 = KBD interrupt
            kbd_handler();
        }
    }
}


int main()
{
    char line[128];
    UART *up;
    KBD  *kp;
    fbuf_init();   // initialize LCD display
    printf("C3.3 start: test KBD TIMER UART drivers\n");
    /* enable timer0,1, uart0,1 SIC interrupts */
    VIC_INTENABLE = 0;
    VIC_INTENABLE |= (1<<4); // timer0,1 at bit4
    VIC_INTENABLE |= (1<<5); // timer2,3 at bit5
    VIC_INTENABLE |= (1<<12); // UART0 at bit12
    VIC_INTENABLE |= (1<<13); // UART1 at bit13
    VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31
    SIC_INTENABLE = 0;
    SIC_INTENABLE |= (1<<3);   // KBD int=bit3 on SIC
    kbd_init(); // initialize keyboard
    uart_init(); // initialize UARTs
    up = &uart[0]; // test UART0 I/O
    kp = &kbd;
    timer_init();
    timer_start(0);
    while(1){
    // timer0 only
    kprintf("Enter a line from KBD\n");
    kgets(line);
    kprintf("Enter a line from UART0\n");
    uprints(up,"Enter a line from UARTS\n\r");
    ugets(up, line);
    uprints(up, line);
    }
}
