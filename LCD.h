#include  <msp430xG46x.h>

typedef unsigned char UInt8;

#define LCD_NUM_DIGITS  7                   // Number of digits on LCD
#define LCD_MEM_OFFSET  2                   // Offset from LCDMEM[0]
#define LCD_MEM_LOC     11                  // Num of LCDMEM[] locations used

#define LCD_A    BIT0
#define LCD_B    BIT1
#define LCD_C    BIT2
#define LCD_D    BIT3
#define LCD_E    BIT6
#define LCD_F    BIT4
#define LCD_G    BIT5
#define LCD_H    BIT7


#define LCD_SYM_DOL      0x01
#define LCD_SYM_ERR      0x02
#define LCD_SYM_MINUS    0x04
#define LCD_SYM_MEM      0x08
#define LCD_SYM_ENV      0x10
#define LCD_SYM_TX       0x20
#define LCD_SYM_RX       0x40
#define LCD_SYM_OFF      0x80

extern void dispSymbol(UInt8 mask);
extern void initLCD_A(void);
extern void clrLCD(void);
extern void dispChar(UInt8 pos, UInt8 index);
extern void displayPass(void);
extern void displayFail(void);
extern void displayWait(void);
extern const UInt8 LCD_MAX_CHARS;

