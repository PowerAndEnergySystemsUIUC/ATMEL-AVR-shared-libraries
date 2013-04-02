/*
 * lcd_power_management.h
 *
 * Created: 4/11/2011 10:32:56 AM
 *  Author: Josiah McClurg
 */ 


#ifndef LCD_POWER_MANAGEMENT_H_
#define LCD_POWER_MANAGEMENT_H_

#include <avr/io.h>
#include "lcd.h"

#define LCD_POWER_PORT PORTD
#define LCD_POWER_DDR  DDRD
#define LCD_POWER_PIN  4

#define lcd_power_on()  LCD_POWER_PORT          |= (1 << LCD_POWER_PIN); \
                        lcd_init(LCD_DISP_ON)
						
/*

LCD_DDR(LCD_DATA0_PORT) &= ~(1 << LCD_DATA0_PIN); \
						LCD_DDR(LCD_DATA1_PORT) &= ~(1 << LCD_DATA1_PIN); \
						LCD_DDR(LCD_DATA2_PORT) &= ~(1 << LCD_DATA2_PIN); \
						LCD_DDR(LCD_DATA3_PORT) &= ~(1 << LCD_DATA3_PIN); \
						LCD_DDR(LCD_RS_PORT)    &= ~(1 << LCD_RS_PIN);    \
						LCD_DDR(LCD_RW_PORT)    &= ~(1 << LCD_RW_PIN);    \
						LCD_DDR(LCD_E_PORT)     &= ~(1 << LCD_E_PIN);     \
*/
#define lcd_power_off() LCD_DATA0_PORT  &= ~(1 << LCD_DATA0_PIN); \
						LCD_DATA1_PORT  &= ~(1 << LCD_DATA1_PIN); \
						LCD_DATA2_PORT  &= ~(1 << LCD_DATA2_PIN); \
						LCD_DATA3_PORT  &= ~(1 << LCD_DATA3_PIN); \
						LCD_RS_PORT     &= ~(1 << LCD_RS_PIN);    \
						LCD_RW_PORT     &= ~(1 << LCD_RW_PIN);    \
						LCD_E_PORT      &= ~(1 << LCD_E_PIN);     \
						LCD_POWER_PORT  &= ~(1 << LCD_POWER_PIN)

#define init_lcd_power_management() LCD_POWER_DDR |= 1 << LCD_POWER_PIN; \
                                    lcd_power_off()

#endif /* LCD_POWER_MANAGEMENT_H_ */