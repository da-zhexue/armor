#include "sk6812.h"
#include "tim.h"
#include "bsp_dwt.h"

/*public variables*/
uint8_t color = RED;
uint8_t light_on = 1;

/*private variables*/
SK6812_Buffer_s led_buf;
uint8_t blinked = 0;

/*开启DMA和PWM传输*/
void Data_Transmit(const uint32_t* buf, const uint32_t num)
{
    while(HAL_TIM_PWM_Start_DMA(&htim4,TIM_CHANNEL_1,buf,num) != HAL_OK);
    while(HAL_TIM_PWM_Start_DMA(&htim3,TIM_CHANNEL_4,buf,num) != HAL_OK);
}

/*将颜色参数写入数组*/
void Set_SINGLE_LED_RGB(const uint16_t n , const uint8_t red, const uint8_t green, const uint8_t blue)
{
    if(n < SK6812_LEDS_NUM)
    {
        for(uint8_t i = 0; i < RGB; i++)
        {
            //输入顺序为G,R,B,且高位先写入
            led_buf.LEDS_Buffer[n][i] = SK6812_RGB_BIT(i,red,green,blue) ? SK6812_HIGH : SK6812_LOW;
        }
    }
}

/*打开灯*/
void SK6812_LEDS_Set(void)
{
    for(uint8_t i=0;i<SK6812_LEDS_NUM;i++)
        Set_SINGLE_LED_RGB(i,COLOR_RGB(color,0),COLOR_RGB(color,1),COLOR_RGB(color,2));
    for(uint8_t i=0;i<RESET_TIME;i++)
        led_buf.RESET_Buffer[i] = 0;
    Data_Transmit((uint32_t*)&led_buf,SK6812_BUFFER_SIZE);
}

/*关闭灯*/
void SK6812_LEDS_Reset(void)
{
    for(uint8_t i=0;i<SK6812_LEDS_NUM;i++)
        Set_SINGLE_LED_RGB(i,0,0,0);
    for(uint8_t i=0;i<RESET_TIME;i++)
        led_buf.RESET_Buffer[i] = 0;
    Data_Transmit((uint32_t*)&led_buf,SK6812_BUFFER_SIZE);
}

void SK6812_LEDS_Blink(void)
{
    SK6812_LEDS_Set();
    for(uint8_t i=0;i<2;i++)
    {
        SK6812_LEDS_Reset();
        DWT_Delay_ms(35);
        SK6812_LEDS_Set();
        DWT_Delay_ms(35);
    }
}

void SK6812_Init(void)
{
    SK6812_LEDS_Reset();
    DWT_Delay_ms(1000);
}

void SK6812_LEDS_Task(const uint8_t hit)
{
    if(light_on)SK6812_LEDS_Set();
    else SK6812_LEDS_Reset();

    // 被击打闪烁
    const uint8_t state = hit;
    if(state == 1 && blinked == 0)
    {
        SK6812_LEDS_Blink();
        blinked = 1;
    }
    else if(state == 0)blinked = 0;
    DWT_Delay_ms(12);
}

