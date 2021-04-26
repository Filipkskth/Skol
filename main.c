#include "gd32vf103.h"
#include "gd32vf103.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "lcd16.h"
#include "systick.h"

/* Connect: SCK to B13
            SDO to B14
            SDI to B15
            CS  to B0 */

uint16_t read_temp();
void init_temp_sense();
float temp_convert(uint16_t rtd);


/* Since the temperature sensor works with a lot slower SPI than the LCD this is used to temporarily slow the bus down */
#define FCLK_SLOW() { SPI_CTL0(SPI1) = (SPI_CTL0(SPI1) & ~0x38) | 0x28; }	/* Set SCLK = PCLK2 / 64 */
#define FCLK_FAST() { SPI_CTL0(SPI1) = (SPI_CTL0(SPI1) & ~0x38) | 0x00; }	/* Set SCLK = PCLK2 / 2 */

int main(){

    rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_2);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_1);

	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_0);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_1);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_2);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_3);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_4);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_5);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_6);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_7);

    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

	LCD_INIT();

    int32_t temp_read = 0;
    float temp_c = 0.0;

    init_temp_sense();

    while(1){
        temp_read = read_temp();
        temp_c = temp_convert(temp_read);

        if((temp_c >= 30.0) || (temp_c <= 25.0)){
            gpio_bit_set(GPIOB, GPIO_PIN_10);
        }else{
            gpio_bit_reset(GPIOB, GPIO_PIN_10);
        }

        LCD_FLOAT_TO_STRING(temp_c);

        delay_1ms(100);

        LCD_CLEAR();

        delay_1us(2);
    }

}





uint8_t read_write_spi ( uint8_t data){
    //Block if already sending
	while(spi_i2s_flag_get(SPI1, SPI_FLAG_TBE) != SET);
    
    spi_i2s_data_transmit(SPI1, data);

    //Block until receive
	while(spi_i2s_flag_get(SPI1, SPI_FLAG_RBNE) != SET);
    
    return(spi_i2s_data_receive(SPI1));
}



void temp_spi_config(void)
{
    rcu_periph_clock_enable(RCU_AF);
	rcu_periph_clock_enable(RCU_SPI1);
    rcu_periph_clock_enable(RCU_GPIOB);
	
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 |GPIO_PIN_14| GPIO_PIN_15);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_bit_set(GPIOB, GPIO_PIN_0);

    spi_parameter_struct spi_init_struct;
    /* deinitilize SPI and the parameters */
    spi_struct_para_init(&spi_init_struct);

    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_2;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spi_init_struct);

	spi_crc_polynomial_set(SPI1,7);
	spi_enable(SPI1);
}


void init_temp_sense(){

    FCLK_SLOW();

    uint8_t data, conf;
    
    //Enable CS-pin
    temp_spi_config();

    delay_1ms(100);


    //Read config register
    gpio_bit_reset(GPIOB, GPIO_PIN_0);

    data = 0x00;
    //Set register
    read_write_spi(data);
    //Read register
    data = read_write_spi(data);

    gpio_bit_set(GPIOB, GPIO_PIN_0);

    delay_1ms(100);

    //Write config register, 3-wire, auto updata
    gpio_bit_reset(GPIOB, GPIO_PIN_0);

    conf = data;
    conf = ~0x2C;

    data = 0x80;
    read_write_spi(data);
    data = read_write_spi(conf);

    gpio_bit_set(GPIOB, GPIO_PIN_0);

    FCLK_FAST();
    
}

#define RTD_A 0.00385
#define RTD_0 100.0
#define RTD_REF 430.0

float temp_convert(uint16_t rtd){
    float Rt, temp;

    Rt = rtd;
    Rt /= 32768;
    Rt *= RTD_REF;

    temp = ((Rt/RTD_0)-1.0)/RTD_A;
    return temp;
}




uint16_t read_temp(){

    uint8_t temperature[2];
    uint8_t data;

    FCLK_SLOW();

    gpio_bit_reset(GPIOB, GPIO_PIN_0);

    data = 0x01;
    read_write_spi(data);
    temperature[0] = read_write_spi(data);

    gpio_bit_set(GPIOB, GPIO_PIN_0);
    delay_1us(500);
    gpio_bit_reset(GPIOB, GPIO_PIN_0);

    data = 0x02;
    read_write_spi(data);
    temperature[1] = read_write_spi(data);

    gpio_bit_set(GPIOB, GPIO_PIN_0);

    uint16_t rtd_reg = ((0x7F & temperature[0]) << 7) | ((0xFE & temperature[1]) >> 1);

    uint32_t rtd_value = rtd_reg * 430;

    rtd_value = rtd_value >> 15;

    FCLK_FAST();

    return rtd_reg;
}
