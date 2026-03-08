#include "hx711.h"
#include "bsp_dwt.h"

GPIO_TypeDef* sck_port[3] = {SCK1_GPIO_Port, SCK2_GPIO_Port, SCK3_GPIO_Port};
uint16_t sck_pin[3] = {SCK1_Pin, SCK2_Pin, SCK3_Pin};
GPIO_TypeDef* dout_port[3] = {DOUT1_GPIO_Port, DOUT2_GPIO_Port, DOUT3_GPIO_Port};
uint16_t dout_pin[3] = {DOUT1_Pin, DOUT2_Pin, DOUT3_Pin};
#define DELAY_1ms DWT_Delay_us(1)

void hx711_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = SCK1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SCK1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SCK2_Pin;
    HAL_GPIO_Init(SCK2_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DOUT1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DOUT1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DOUT2_Pin;
    HAL_GPIO_Init(DOUT2_GPIO_Port, &GPIO_InitStruct);
}

void hx711_data_in(const uint8_t sensor)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = dout_pin[sensor];
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(dout_port[sensor], &GPIO_InitStruct);
}

void hx711_data_out(const uint8_t sensor)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = dout_pin[sensor];
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(dout_port[sensor], &GPIO_InitStruct);
}

uint32_t hx711_read(const uint8_t sensor)
{
    uint32_t value = 0;

    hx711_data_out(sensor);
    HAL_GPIO_WritePin(dout_port[sensor], dout_pin[sensor], GPIO_PIN_SET);
    DELAY_1ms;
    HAL_GPIO_WritePin(sck_port[sensor], sck_pin[sensor], GPIO_PIN_RESET);
    hx711_data_in(sensor);

    while (HAL_GPIO_ReadPin(dout_port[sensor], dout_pin[sensor]) == GPIO_PIN_SET)
        ;
    DELAY_1ms;

    for (int i = 0; i < 24; i++)
    {
        HAL_GPIO_WritePin(sck_port[sensor], sck_pin[sensor], GPIO_PIN_SET);
        DELAY_1ms;
        HAL_GPIO_WritePin(sck_port[sensor], sck_pin[sensor], GPIO_PIN_RESET);
        if (HAL_GPIO_ReadPin(dout_port[sensor], dout_pin[sensor]) == GPIO_PIN_RESET)
        {
            value = value << 1;
            value |= 0x00;
        }
        else
        {
            value = value << 1;;
            value |= 0x01;
        }
        DELAY_1ms;
    }
    HAL_GPIO_WritePin(sck_port[sensor], sck_pin[sensor], GPIO_PIN_SET);
    value = value ^ 0x800000;
    DELAY_1ms;
    HAL_GPIO_WritePin(sck_port[sensor], sck_pin[sensor], GPIO_PIN_RESET);
    DELAY_1ms;
    return value;
}
