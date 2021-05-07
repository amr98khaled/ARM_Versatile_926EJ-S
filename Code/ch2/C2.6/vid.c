#define LCDTiming0R 0x10120000
#define LCDTiming1R 0x10120004
#define LCDTiming2R 0x10120008
#define LCDControlR 0x10120018  /* it is 0x1012001C originally but 0x10120018 in QEMU */
#define LCDUPBASER  0x10120010  // LCD upper panel base address. This is the start address of the upper panel frame data in memory and is word aligned.
#define SYS_OSC1R   0x1000001c

int volatile *fb;
int WIDTH = 640; // default to VGA mode for 640x480

int fbuf_init()
{
    fb = (int *)(0x200000); // at 2 MB to 4 MB
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
}
