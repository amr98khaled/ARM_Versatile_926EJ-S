typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#define LCDTiming0R 0x10120000
#define LCDTiming1R 0x10120004
#define LCDTiming2R 0x10120008
#define LCDControlR 0x10120018  /* it is 0x1012001C originally but 0x10120018 in QEMU */
#define LCDUPBASER  0x10120010  // LCD upper panel base address. This is the start address of the upper panel frame data in memory and is word aligned.
#define SYS_OSC1R   0x1000001c

#define RED   0
#define BLUE  1
#define GREEN 2
#define WHITE 3

extern char _binary_font_start;

int color;
u8 cursor;
int volatile *fb;
int row, col, scroll_row;
unsigned char *font;
int WIDTH = 640; // default to VGA mode for 640x480
char *tab = "0123456789ABCDEF";

int fbuf_init()
{
    int i;
    fb = (int *)(0x200000); // at 2 MB to 4 MB
    font= &_binary_font_start;
    //***************** for 640x480 VGA ****************/
    *(volatile unsigned int *)(SYS_OSC1R) = 0x2C77;    // LCD Frequency = 25MHz
    *(volatile unsigned int *)(LCDTiming0R) = 0x3F1F3F9C;
    *(volatile unsigned int *)(LCDTiming1R) = 0x090B61DF;
    *(volatile unsigned int *)(LCDTiming2R) = 0x067F1800;

    /***************** for 800X600 SVGA ****************
    *(volatile unsigned int *)(SYS_OSC1R) = 0x2CAC;    // LCD Frequency = 36MHz
    *(volatile unsigned int *)(LCDTiming0R) = 0x1313A4C4;
    *(volatile unsigned int *)(LCDTiming1R) = 0x0505F6F7;
    *(volatile unsigned int *)(LCDTiming2R) = 0x071F1800;
    }
    ****************************************************/
    *(volatile unsigned int *)(LCDUPBASER) = 0x200000; // fbuf
    *(volatile unsigned int *)(LCDControlR) = 0x82B;    
    /* LCD Power Enabled , little-endian pixel ordering within a byte , little-endian byte order , RGB normal output , single panel LCD is in use , mono LCD uses 4-bit interface , LCD is TFT, do not use gray scaler , 24 bpp , LCD controller enabled */
    
    cursor = 127; // cursor = row 127 in font bitmap
}


int clrpix(int x, int y)  // clear pixel at (x,y)
{
    int pix = y*640 + x;
    fb[pix] = 0x00000000;
}


int setpix(int x, int y) // set pixel at (x,y)
{
    int pix = y*640 + x;
    if(color==RED)
        fb[pix] = 0x000000FF;
    if(color==BLUE)
        fb[pix] = 0x00FF0000;
    if(color==GREEN)
        fb[pix] = 0x0000FF00;
}


int dchar(unsigned char c, int x, int y) // display char at (x,y)
{
    int r, bit;
    unsigned char *caddress, byte;
    caddress = font + c*16;
    for (r=0; r<16; r++)
    {
        byte = *(caddress + r);
        for (bit=0; bit<8; bit++)
        {
            if (byte & (1<<bit))
                setpix(x+bit, y+r);
        }
    }
}


int undchar(unsigned char c, int x, int y) // erase char at (x,y)
{
    int row, bit;
    unsigned char *caddress, byte;
    caddress = font + c*16;
    for (row=0; row<16; row++)
    {
        byte = *(caddress + row);
        for (bit=0; bit<8; bit++)
        {
            if (byte & (1<<bit))
                clrpix(x+bit, y+row);
        }
    }
}

int scroll() // scrow UP one line (the hard way)
{
    int i;
    for (i=64*640; i<640*480; i++)
    {
        fb[i] = fb[i + 640*16];
    }
}


int kpchar(char c, int ro, int co) // print char at (row, col)
{
    int x, y;
    x = co*8;
    y = ro*16;
    dchar(c, x, y);
}


int unkpchar(char c, int ro, int co) // erase char at (row, col)
{
    int x, y;
    x = co*8;
    y = ro*16;
    undchar(c, x, y);
}


int erasechar() // erase char at (row,col)
{
    int r, bit, x, y;
    unsigned char *caddress, byte;
    x = col*8;
    y = row*16;
    for (r=0; r<16; r++)
    {
        for (bit=0; bit<8; bit++)
        {
            clrpix(x+bit, y+r);
        }
    }
}


int clrcursor() // clear cursor at (row, col)
{
    unkpchar(127, row, col);
}


int putcursor(unsigned char c) // set cursor at (row, col)
{
    kpchar(c, row, col);
}


int kputc(char c) // print char at cursor position
{
    clrcursor();
    if(c=='\r') // return key
    {
        col=0;
        putcursor(cursor);
        return;
    }
    if(c=='\n') // new line key
    {
        row++;
        if(row>=25)
        {
            row = 24;
            scroll();
        }
        putcursor(cursor);
        return;
    }
    if(c=='\b')  // backspace key
    {
        if(col>0)
        {
            col--;
            erasechar();
            putcursor(cursor);
        }
        return;
    }
    // c is ordinary char case
    kpchar(c, row, col);
    col++;
    if (col>=80)
    {
        col = 0;
        row++;
        if (row >= 25)
        {
            row = 24;
            scroll();
        }
    }
    putcursor(cursor);
}


// The following implements kprintf() for formatted printing

int kprints(char *s)
{
    while(*s)
    {
        kputc(*s);
        s++;
    }
}

int krpx(int x)
{
    char c;
    if(x)
    {
        c = tab[x % 16];
        krpx(x / 16);
    }
    kputc(c);
}


int kprintx(int x)
{
    kputc('0'); kputc('x');
    if(x==0)
        kputc('0');
    else
        krpx(x);
    kputc(' ');
}


int krpu(int x)
{
    char c;
    if(x)
    {
        c = tab[x % 10];
        krpu(x / 10);
    }
    kputc(c);
}


int kprintu(int x)
{
    if(x==0)
        kputc('0');
    else
        krpu(x);
    kputc(' ');
}


int kprinti(int x)
{
    if(x<0)
    {
        kputc('-');
        x = -x;
    }
    kprintu(x);
}

int kprintf(char *fmt,...)      // variadic function , tell the compier that we don't know the number of arguments
{
    int *ip;       
    char *cp;
    cp = fmt;
    ip = (int *)&fmt + 1;       // ip points to first item in stack , the second argument
    while(*cp)
    {
        if (*cp != '%')
        {
            kputc(*cp);
            if(*cp=='\n')
                kputc('\r');
            cp++;
            continue;           // skip the rest of the code in the current loop block, and start the next iteration.
        }
        cp++;                   // cp points at a conversion symbol
        switch(*cp)
        {
            case 'c': kputc((char)*ip); break;
            case 's': kprints((char *)*ip); break;
            case 'd': kprinti(*ip); break;
            case 'u': kprintu(*ip); break;
            case 'x': kprintx(*ip); break;
        }
        cp++; ip++;
    }
}
