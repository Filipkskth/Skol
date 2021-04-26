#include <stdio.h>
#include <stdlib.h>

//Initialize LCD
void LCD_INIT();

//A7-A0 / D7-D0 in Decimal to binary
void LCD_DEC_TO_BIN(int dec);

//Clear LCD
void LCD_CLEAR();

//Enable command mode
void RS_CMD();

//Enable character mode
void RS_CHAR();

//Trogger enable
void EN_T();

//Char to binary
void LCD_CHAR_TO_BIN(char ch);

//String to binary / Print string to LCD
void LCD_STRING_TO_BIN(char chA[]);

//Float to String to binary
void LCD_FLOAT_TO_STRING(float decch);
