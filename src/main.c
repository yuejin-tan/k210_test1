#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#include "fpioa.h"
#include "gpio.h"
#include "gpiohs.h"
#include "sysctl.h"
#include "dmac.h"
#include "fpioa.h"
#include "i2s.h"
#include "plic.h"
#include "uarths.h"
#include "uart.h"

#include "bsp.h"


#include "lcd.h"
#include "st7789.h"
#include "ov2640.h"
#include "dvp.h"

// LED
#define PIN_LED_R             (13)
#define PIN_LED_G             (12)
#define PIN_LED_B             (14)

// LCD
#define PIN_LCD_CS             (36)
#define PIN_LCD_RST            (37)
#define PIN_LCD_RS             (38)
#define PIN_LCD_WR             (39)

#define LCD_RST_GPIOHSNUM        (FUNC_GPIOHS0)
#define LCD_RS_GPIOHSNUM         (FUNC_GPIOHS1)

// camera
#define PIN_DVP_PCLK           (47)
#define PIN_DVP_XCLK           (46)
#define PIN_DVP_HSYNC          (45)
#define PIN_DVP_PWDN           (44)
#define PIN_DVP_VSYNC          (43)
#define PIN_DVP_RST            (42)
#define PIN_DVP_SCL            (41)
#define PIN_DVP_SDA            (40)

int main()
{
    gpio_init();                             //初始化GPIO

    // LED
    fpioa_set_function(PIN_LED_R, FUNC_GPIO0); //管脚映射
    gpio_set_drive_mode(0, GPIO_DM_OUTPUT);    //设置GPIO驱动模式
    fpioa_set_function(PIN_LED_G, FUNC_GPIO1);        //管脚映射
    gpio_set_drive_mode(1, GPIO_DM_OUTPUT);    //设置GPIO驱动模式
    fpioa_set_function(PIN_LED_B, FUNC_GPIO2);        //管脚映射
    gpio_set_drive_mode(2, GPIO_DM_OUTPUT);    //设置GPIO驱动模式

    gpio_set_pin(0, GPIO_PV_LOW);
    gpio_set_pin(1, GPIO_PV_LOW);
    gpio_set_pin(2, GPIO_PV_LOW);

    /* lcd */
    fpioa_set_function(PIN_LCD_CS, FUNC_SPI0_SS0);
    fpioa_set_function(PIN_LCD_RST, LCD_RST_GPIOHSNUM);
    fpioa_set_function(PIN_LCD_RS, LCD_RS_GPIOHSNUM);
    fpioa_set_function(PIN_LCD_WR, FUNC_SPI0_SCLK);
    sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);

    /* DVP camera */
    fpioa_set_function(PIN_DVP_RST, FUNC_CMOS_RST);
    fpioa_set_function(PIN_DVP_PWDN, FUNC_CMOS_PWDN);
    fpioa_set_function(PIN_DVP_XCLK, FUNC_CMOS_XCLK);
    fpioa_set_function(PIN_DVP_VSYNC, FUNC_CMOS_VSYNC);
    fpioa_set_function(PIN_DVP_HSYNC, FUNC_CMOS_HREF);
    fpioa_set_function(PIN_DVP_PCLK, FUNC_CMOS_PCLK);
    fpioa_set_function(PIN_DVP_SCL, FUNC_SCCB_SCLK);
    fpioa_set_function(PIN_DVP_SDA, FUNC_SCCB_SDA);
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);

    /* 使能SPI0和DVP */
    sysctl_set_spi0_dvp_data(1);


    /* 设置系统时钟和DVP时钟 */
    sysctl_pll_set_freq(SYSCTL_PLL0, 800000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL1, 300000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);

    /* 系统中断初始化*/
    plic_init();

    /* 初始化LCD */
    lcd_init();

    /* LCD显示图片 */
    lcd_draw_string(16, 40, "Hello Yahboom!", RED);
    lcd_draw_string(16, 60, "Nice to meet you!", BLUE);
    sleep(5);

    ov2640_init();
    sysctl_enable_irq();

    while (1)
    {
        /* 等待摄像头采集结束，然后清除结束标志 */
        while (g_dvp_finish_flag == 0)
        {
            // wait
        }
        g_dvp_finish_flag = 0;

        /* 显示画面 */
        lcd_draw_picture(0, 0, 320, 240, display_buf);
    }

    unsigned int cnt = 0;

    while (1)
    {
        cnt = (cnt + 1) & 0x7u;

        if (cnt & 1)
        {
            gpio_set_pin(0, GPIO_PV_HIGH);
        }
        else
        {
            gpio_set_pin(0, GPIO_PV_LOW);
        }

        if (cnt & 2)
        {
            gpio_set_pin(1, GPIO_PV_HIGH);
        }
        else
        {
            gpio_set_pin(1, GPIO_PV_LOW);
        }

        if (cnt & 4)
        {
            gpio_set_pin(2, GPIO_PV_HIGH);
        }
        else
        {
            gpio_set_pin(2, GPIO_PV_LOW);
        }

        msleep(200);
    }

    return 1;
}
