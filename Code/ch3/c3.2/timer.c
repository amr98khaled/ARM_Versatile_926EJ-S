// timer register u32 offsets from base address
#define TLOAD   0x0
#define TVALUE  0x1
#define TCNTL   0x2
#define TINTCLR 0x3
#define TRIS    0x4
#define TMIS    0x5
#define TBGLOAD 0x6

typedef volatile struct timer
{
    unsigned int *base;            // timer's base address; as u32 pointer
    int tick, hh, mm, ss; // per timer data area
    char clock[16];
}TIMER;

volatile TIMER timer[4]; //4 timers; 2 per unit; at 0x00 and 0x20

void timer_init()
{
    int i; TIMER *tp;
    printf("timer_init()\n");
    for (i=0; i<4; i++)
    {
        tp = &timer[i];
        if(i==0) tp->base = (u32 *)0x101E2000;
        if(i==1) tp->base = (u32 *)0x101E2020;
        if(i==2) tp->base = (u32 *)0x101E3000;
        if(i==3) tp->base = (u32 *)0x101E3020;
        *(tp->base+TLOAD) = 0x0;  // reset
        *(tp->base+TVALUE)= 0xFFFFFFFF;
        *(tp->base+TRIS)  =  0x0;
        *(tp->base+TMIS)  =  0x0;
        *(tp->base+TLOAD) = 0x100;
        // CntlReg=011-0110=|Disable|Periodic|InterruptEnable|-|scal=01 : clock / 1 |32bit|0=wrap|=0x62
        *(tp->base+TCNTL) = 0x62;
        *(tp->base+TBGLOAD) = 0xF4240; // timer counter value (1 sec) 
        /*
         TimerxLoad = [(Interval*ClkFreq)/(ClkDiv*Prescale)]  , Interval is period we want , clkFreq is by default 1Mhz , ClkDiv is by default 1 
         */
        
        tp->tick = tp->hh = tp->mm = tp->ss = 0; // initialize wall clock
        strcpy((char *)tp->clock, "00:00:00");
    }
}

int timer_clearInterrupt(int n);

void timer_handler(int n) 
{
    int i;
    TIMER *t = &timer[n];
    t->tick++;
    t->ss = t->tick;
    t->ss %=60;
        if(t->ss == 0)
        {
            t->mm++;
            if((t->mm %= 60)==0)
            {
                t->mm = 0; t->hh++; // no 24 hour roll around
            }
        }
        
    for (i=0; i<8; i++)
    {
      unkpchar(t->clock[i], n, 70+i);
    }
        
        t->clock[7]='0'+(t->ss%10); t->clock[6]='0'+(t->ss/10);
        t->clock[4]='0'+(t->mm%10); t->clock[3]='0'+(t->mm/10);
        t->clock[1]='0'+(t->hh%10); t->clock[0]='0'+(t->hh/10);
    
    color = n+1;
    for (i=0; i<8; i++)
    {
       kpchar(t->clock[i], n, 70+i);
    }
       timer_clearInterrupt(n); // clear timer interrupt
}

void timer_start(int n) // timer_start(0), 1, etc.
{
    TIMER *tp = &timer[n];
    kprintf("timer_start %d base=%x\n", n, tp->base);
    *(tp->base+TCNTL) |= 0x80;  // set enable bit 7
}

int timer_clearInterrupt(int n) 
{
    TIMER *tp = &timer[n];
    *(tp->base+TINTCLR) = 0xFFFFFFFF;       //Any write to this register, clears the interrupt output from the counter.
}
void timer_stop(int n)
{
    // stop a timer
    TIMER *tp = &timer[n];
    *(tp->base+TCNTL) &= 0x7F; // clear enable bit 7
}
