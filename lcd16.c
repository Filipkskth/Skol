#include "gd32vf103.h"
#include "lcd16.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void RS_CMD(){
    delay_1ms(10);

    gpio_bit_reset(GPIOB, GPIO_PIN_2);

    delay_1ms(10);
}

void RS_CHAR(){
    delay_1us(1);

    gpio_bit_set(GPIOB, GPIO_PIN_2);

    delay_1us(1);
}

void EN_T(){
    delay_1us(1);

    gpio_bit_set(GPIOB, GPIO_PIN_1);

    delay_1us(1);

    gpio_bit_reset(GPIOB, GPIO_PIN_1);

    delay_1us(1);
}

void LCD_DEC_TO_BIN(int dec){

	for(int x = 7;x >= 0;x--){
		int temp = dec;
		temp = temp>>x;
		temp = temp&1;

		if(x == 7){
			if(temp>0){
				gpio_bit_set(GPIOA, GPIO_PIN_7);
			}else{
				gpio_bit_reset(GPIOA, GPIO_PIN_7);
			}
		}
		if(x == 6){
			if(temp>0){
				gpio_bit_set(GPIOA, GPIO_PIN_6);
			}else{
				gpio_bit_reset(GPIOA, GPIO_PIN_6);
			}

		}
		if(x == 5){
			if(temp>0){
				gpio_bit_set(GPIOA, GPIO_PIN_5);
			}else{
				gpio_bit_reset(GPIOA, GPIO_PIN_5);
			}
		}
		if(x == 4){
			if(temp>0){
				gpio_bit_set(GPIOA, GPIO_PIN_4);
			}else{
				gpio_bit_reset(GPIOA, GPIO_PIN_4);
			}
		}
		if(x == 3){
			if(temp>0){
				gpio_bit_set(GPIOA, GPIO_PIN_3);
			}else{
				gpio_bit_reset(GPIOA, GPIO_PIN_3);
			}
		}
		if(x == 2){
			if(temp>0){
				gpio_bit_set(GPIOA, GPIO_PIN_2);
			}else{
				gpio_bit_reset(GPIOA, GPIO_PIN_2);
			}
		}
		if(x == 1){
			if(temp>0){
				gpio_bit_set(GPIOA, GPIO_PIN_1);
			}else{
				gpio_bit_reset(GPIOA, GPIO_PIN_1);
			}
		}
		if(x == 0){
			if(temp>0){
				gpio_bit_set(GPIOA, GPIO_PIN_0);
			}else{
				gpio_bit_reset(GPIOA, GPIO_PIN_0);
			}
		}

	}

    EN_T();

}

void LCD_CLEAR(){
    RS_CMD();

    delay_1ms(10);

    LCD_DEC_TO_BIN(1);

    EN_T();

    delay_1ms(10);

}

void LCD_INIT(){

    delay_1ms(200);

    RS_CMD();

    for(int x = 0;x < 3;x++){

			delay_1ms(50);

			LCD_DEC_TO_BIN(56); //Init 1-3
	}

    delay_1ms(50);

	LCD_DEC_TO_BIN(14); // Init 4

    delay_1ms(50);

	LCD_DEC_TO_BIN(6); // Init 5

    delay_1ms(50);

	LCD_CLEAR();

}

void LCD_CHAR_TO_BIN(char ch){
    RS_CHAR();

    int chnum = ch;

    LCD_DEC_TO_BIN(chnum);
}

void LCD_STRING_TO_BIN(char chA[]){
    RS_CHAR();

    int num = strlen(chA);

    for(int x = 0;x < num;x++){
        LCD_CHAR_TO_BIN(chA[x]);
    }

}

void LCD_FLOAT_TO_STRING(float fchA){

    RS_CHAR();

    char chAF[10];

    gcvt(fchA, 6, chAF);

	LCD_STRING_TO_BIN(chAF);

}