#ifndef ARMOR_HX711_H
#define ARMOR_HX711_H
#include "typedef.h"

void hx711_init(void);
uint32_t hx711_read(uint8_t sensor);
unsigned long HX711_Read_01();

#endif //ARMOR_HX711_H