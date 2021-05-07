#include "defines.h"
#include "keymap" 
#include "keymap2"

#define LSHIFT 0x12
#define RSHIFT 0x59
#define ENTER  0x5A
#define LCTRL  0x14
#define RCTRL  0xE014

#define ESC    0x76
#define F1     0x05
#define F2     0x06
#define F3     0x04
#define F4     0x0C

/******** KBD register byte offsets; for char *base *****/
#define KCNTL 0x00 // 7-6_ 5(0=AT)4=RxIntEn 3=TxIntEn
#define KSTAT 0x04 // 7_ 6=TxE 5=TxBusy 4=RXFull 3=RxBusy
#define KDATA 0x08 // data register;
#define KCLK  0x0C // clock divisor register;   (not used)
#define KISTA 0x10 // interrupt status register;(not used)

typedef volatile struct kbd{ 
    // base = 0x10006000
    char *base; // base address of KBD, as char *
    char buf[128]; // input buffer
    int head, tail, data, room; // control variables
    }KBD;
volatile KBD kbd;  // KBD data structure
int shifted = 0;
int release = 0;
int control = 0;

volatile int keyset;

int kbd_init()
{
    char scode;
    keyset = 1; // default to scan code set-1
  
    KBD *kp = &kbd;
    kp->base = (char *)0x10006000;
    *(kp->base+KCNTL) = 0x14; // 00010100=INTreceiverEnable, Enable
    *(kp->base+KCLK) = 8;     // PL051 manual says a value 0 to 15
    kp->data = 0; kp->room = 128; // counters
    kp->head = kp->tail = 0;   // index to buffer
    
    shifted = 0;
    release = 0;
    control = 0;
    printf("Detect KBD scan code: press the ENTER key : ");
    while( (*(kp->base + KSTAT) & 0x10) == 0);      //wait while receive register is full to read 
    scode = *(kp->base + KDATA);
    printf("scode=%x ", scode);
    if (scode==0x5A)
        keyset=2;
    printf("keyset=%d\n", keyset);
}


// keys don't work correctly in kbd_handler1
void kbd_handler1()          // KBD interrupt handler in C
{
    u8 scode, c;
    int i;
    KBD *kp = &kbd;
    color = RED;
    scode = *(kp->base+KDATA); // read scan code in data register
    if(scode & 0x80)  // ignore key releases
        return;
    c = unsh[scode];   // map scan code to ASCII
    if (c >= 'a' && c <= 'z')
     printf("kbd interrupt: c=%x %c\n", c, c);
    kp->buf[kp->head++] = c;
    kp->head %= 128; // enter key into CIRCULAR buf[ ]
    kp->data++; kp->room--; // update counters
}


// kbd_handelr2() for scan code set 2
void kbd_handler2()
{
  u8 scode, c;
  KBD *kp = &kbd;
  color = YELLOW;
  scode = *(kp->base + KDATA);
  //  printf("scanCode = %x  ", scode);

  if (scode == 0xF0){ // key release 
    release = 1;
    return;
  }
  
  if (release && scode != 0x12){ // ordinay key release
    release = 0;
    return;
  }

  if (release && scode == 0x12){ // Left shift key release
    release = 0;
    shifted = 0;
    return;
  }

  if (!release && scode == 0x12){// left key press and HOLD
    release = 0;
    shifted = 1;
    return;
  }

  if (!release && scode == LCTRL){// left Control key press and HOLD
    release = 0;
    control = 1;
    return;
  }

  if (release && scode == LCTRL){ // Left Control key release
    release = 0;
    control = 0;
    return;
  }

  /********* catch Control-C ****************/
  if (control && scode == 0x21){ // Control-C
    // send signal 2 tp processes on KBD
    printf("Control-C: %d\n", control);
    control = 0;
    return;
  }   

  
  if (!shifted)
     c = ltab[scode];
  else
     c = utab[scode];
  
  //if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c==' ' || c=='\b'){
     printf("kbd interrupt: c=%x %c\n", c, c);
     // }

  if (control && scode == 0x23){ // Control-D
    c = 0x04;
    printf("Control-D: c = %x\n", c);
  }   

  kp->buf[kp->head++] = c;
  kp->head %= 128;
  kp->data++; kp->room--;
}

void kbd_handler()
{
  if (keyset == 1)
    kbd_handler1();
  else
    kbd_handler2();
}

int kgetc() // main program calls kgetc() to return a char
{
    char c;
    KBD *kp = &kbd;
    unlock();
    // enable IRQ interrupts
    while(kp->data <= 0); // wait for data; READONLY
    lock(); // disable IRQ interrupts
    c = kp->buf[kp->tail++];// get a c and update tail index
    kp->tail %= 128;
    kp->data--; kp->room++; // update with interrupts OFF
    unlock();
    // enable IRQ interrupts
    return c;
}

int kgets(char s[ ])        /* get a string from KBD */
{
    char c;
    while( (c = kgetc()) != '\r')
    {
        if (c=='\b')
        {
            s--;
            continue;
        }
        *s++ = c;
    }
    *s = 0;
    return strlen(s);
}
