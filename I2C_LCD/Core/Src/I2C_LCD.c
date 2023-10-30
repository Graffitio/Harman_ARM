/*
 * I2C_LCD.c
 *
 *  Created on: Oct 23, 2023
 *      Author: Wooseong Jung
 */

#include "I2C_LCD.h"

extern I2C_HandleTypeDef hi2c1; // hi2c1이 외부에 선언되었다는 뜻

#define SLAVE_ADDRES_LCD	0x27<<1

void lcd_init()
{
	// cmd init
	HAL_Delay(50);
	lcd_send_cmd(0x30);
	HAL_Delay(5);
	lcd_send_cmd(0x30);
	HAL_Delay(1);
	lcd_send_cmd(0x30);
	HAL_Delay(10);
	lcd_send_cmd(0x20); // 4bit mode
	HAL_Delay(10);

	// display init
	lcd_send_cmd(0x28); // function set
	HAL_Delay(1);
	lcd_send_cmd(0x08); // display off
	HAL_Delay(1);
	lcd_send_cmd(0x01); // clear display
	HAL_Delay(1);
	lcd_send_cmd(0x06); // entry mode
	HAL_Delay(1);
	lcd_send_cmd(0x0C); // display on
}

void lcd_send_cmd(char cmd)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd & 0xf0); // msb쪽 4bit만 남기고 나머지 다 버림.
	data_l = ((cmd << 4) & 0xf0); // lsb쪽 4bit만 님기고 나머지 다 버림.
	data_t[0] = data_u | 0x0C;	// en = 1, rs = 0
	data_t[1] = data_u | 0x08;	// en = 0, rs = 0
	data_t[2] = data_l | 0x0C;	// en = 1, rs = 0
	data_t[3] = data_l | 0x08;	// en = 0, rs = 0

	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRES_LCD, (uint8_t *)data_t, 4, 100);
}

void lcd_send_data(char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data & 0xf0); // msb쪽 4bit만 남기고 나머지 다 버림.
	data_l = ((data << 4) & 0xf0); // lsb쪽 4bit만 님기고 나머지 다 버림.
	data_t[0] = data_u | 0x0D;	// en = 1, rs = 0
	data_t[1] = data_u | 0x09;	// en = 0, rs = 0
	data_t[2] = data_l | 0x0D;	// en = 1, rs = 0
	data_t[3] = data_l | 0x09;	// en = 0, rs = 0

	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRES_LCD, (uint8_t *)data_t, 4, 100);
}

void lcd_send_string(char *str)
{
	while(*str) lcd_send_data(*str++); // *str이 참이면, lcd_send_data(*str++) 실행
}

void lcd_put_cur(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }

    lcd_send_cmd(col);
}

void lcd_clear()
{
	lcd_send_cmd (0x80);
	for (int i=0 ; i<70 ; i++)
	{
		lcd_send_data(' ');
	}
}
