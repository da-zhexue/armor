#include "sk6812.h"
#include "tim.h"
#include "bsp_dwt.h"

/*public variables*/
uint8_t color = BLUE;
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

void SK6812_LEDS_Flow(void);
void SK6812_LEDS_Task(const uint8_t hit)
{
    //if(light_on) SK6812_LEDS_Set();
    if(light_on) SK6812_LEDS_Flow();
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

#define RGB_FLOW_COLOR_CHANGE_TIME  500
#define RGB_FLOW_COLOR_LENGTH   6
//blue-> green(dark)-> red -> blue(dark) -> green(dark) -> red(dark) -> blue

uint32_t RGB_flow_color[RGB_FLOW_COLOR_LENGTH + 1] = {0xFF0000FF, 0x0000FF00, 0xFFFF0000, 0x000000FF, 0xFF00FF00, 0x00FF0000, 0xFF0000FF};
void SK6812_LEDS_Flow(void)
{
    for(int i = 0; i < RGB_FLOW_COLOR_LENGTH; i++)
    {
        fp32 alpha = (fp32)((RGB_flow_color[i] & 0xFF000000) >> 24);
        fp32 red = (fp32)((RGB_flow_color[i] & 0x00FF0000) >> 16);
        fp32 green = (fp32)((RGB_flow_color[i] & 0x0000FF00) >> 8);
        fp32 blue = (fp32)((RGB_flow_color[i] & 0x000000FF) >> 0);

        fp32 delta_alpha = (fp32)((RGB_flow_color[i + 1] & 0xFF000000) >> 24) - (fp32)((RGB_flow_color[i] & 0xFF000000) >>
            24);
        fp32 delta_red = (fp32)((RGB_flow_color[i + 1] & 0x00FF0000) >> 16) - (fp32)((RGB_flow_color[i] & 0x00FF0000) >>
            16);
        fp32 delta_green = (fp32)((RGB_flow_color[i + 1] & 0x0000FF00) >> 8) - (fp32)((RGB_flow_color[i] & 0x0000FF00) >>
            8);
        fp32 delta_blue = (fp32)((RGB_flow_color[i + 1] & 0x000000FF) >> 0) - (fp32)((RGB_flow_color[i] & 0x000000FF) >> 0);

        delta_alpha /= RGB_FLOW_COLOR_CHANGE_TIME;
        delta_red /= RGB_FLOW_COLOR_CHANGE_TIME;
        delta_green /= RGB_FLOW_COLOR_CHANGE_TIME;
        delta_blue /= RGB_FLOW_COLOR_CHANGE_TIME;
        for(int j = 0; j < RGB_FLOW_COLOR_CHANGE_TIME; j++)
        {
            alpha += delta_alpha;
            red += delta_red;
            green += delta_green;
            blue += delta_blue;

            for(uint8_t n = 0; n < SK6812_LEDS_NUM; n++)
            {
                // 如果需要带透明度亮度调节可以使用 (uint8_t)(red * (alpha/255.0))，此处直接使用计算出的RGB
                Set_SINGLE_LED_RGB(n, (uint8_t)red, (uint8_t)green, (uint8_t)blue);
            }
            for(uint8_t n = 0; n < RESET_TIME; n++)
            {
                led_buf.RESET_Buffer[n] = 0;
            }
            Data_Transmit((uint32_t*)&led_buf, SK6812_BUFFER_SIZE);

            DWT_Delay_ms(3);
        }
    }
}
