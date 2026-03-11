#ifndef ARMOR_HX711_H
#define ARMOR_HX711_H
#include "typedef.h"

#define HX711_DOUT(__ID__) \
((__ID__ == 0) ? HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==GPIO_PIN_SET :\
(__ID__ == 1) ? HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==GPIO_PIN_SET :\
(__ID__ == 2) ? HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15)==GPIO_PIN_SET :\
HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)==GPIO_PIN_SET)

#define HX711_DOUT_WRITE(__ID__,__STATE__) \
((__ID__ == 0) ? HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,__STATE__) :\
(__ID__ == 1) ? HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,__STATE__) :\
(__ID__ == 2) ? HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,__STATE__) :\
HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,__STATE__))

#define HX711_SCK(__ID__,__STATE__) \
((__ID__ == 0) ? HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,__STATE__) :\
(__ID__ == 1) ? HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,__STATE__) :\
(__ID__ == 2) ? HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,__STATE__) :\
HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,__STATE__))

#define HX711_SCK_HIGH(__ID__) HX711_SCK(__ID__,GPIO_PIN_SET)
#define HX711_SCK_LOW(__ID__) HX711_SCK(__ID__,GPIO_PIN_RESET)
#define HX711_DOUT_HIGH(__ID__) HX711_DOUT_WRITE(__ID__,GPIO_PIN_SET)
#define HX711_DOUT_LOW(__ID__) HX711_DOUT_WRITE(__ID__,GPIO_PIN_RESET)

void hx711_init(void);
uint32_t hx711_read(uint8_t id);
uint8_t get_hit();
void printf_hx711_data();
#define HIT_THRESHOLD_0 0
#define HIT_THRESHOLD_1 7200000
#define HIT_THRESHOLD_2 8180000
#define HIT_THRESHOLD_3 8250000

#endif //ARMOR_HX711_H
