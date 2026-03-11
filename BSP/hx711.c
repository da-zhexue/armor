#include "hx711.h"

#include "bsp_dwt.h"
#include "gpio.h"
#include "usart.h"

#define DELAY_1ms DWT_Delay_us(1)

uint32_t hx711_read(const uint8_t id)
{
    uint32_t value = 0;

    HX711_DOUT_HIGH(id);
    HX711_SCK_LOW(id);

    while (HX711_DOUT(id))
        ;

    for (int i = 0; i < 24; i++)
    {
        HX711_SCK_HIGH(id);
        HX711_SCK_LOW(id);
        value = value << 1;
        if (HX711_DOUT(id))
            value |= 0x01;
    }
    HX711_SCK_HIGH(id);
    value = value ^ 0x800000;
    HX711_SCK_LOW(id);
    return value;
}

const uint8_t hx711_work[4] = {0, 1, 1, 1};
const uint32_t hx711_hit_threshold[4] = {HIT_THRESHOLD_0, HIT_THRESHOLD_1, HIT_THRESHOLD_2, HIT_THRESHOLD_3};
uint8_t get_hit()
{
    uint8_t hit = 0;
    for (int i = 0; i < 4; i++)
        if (hx711_work[i])
            if (hx711_read(i) < hx711_hit_threshold[i])
                hit = 1;
    return hit;
}

void packed_uint32(const uint32_t in, uint8_t* out)
{
    out[0] = in & 0xff;
    out[1] = (in >> 8) & 0xff;
    out[2] = (in >> 16) & 0xff;
    out[3] = (in >> 24) & 0xff;
}

void printf_hx711_data()
{
    uint8_t data[]={0x00, 0x00, 0x00, 0x00, 0x00};
    for (int i = 0; i < 4; i++)
    {
        if (hx711_work[i])
        {
            data[0] = i;
            const uint32_t value = hx711_read(i);
            packed_uint32(value, &data[1]);
            HAL_UART_Transmit(&huart1, data, sizeof(data), 100);
        }
    }
}
