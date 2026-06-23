#include <stdio.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define LEDC_TIMER              LEDC_TIMER_0 // Используем таймер 0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_GPIO        GPIO_NUM_2 // Задаем GPIO - встроенный светодиод на плате
#define LEDC_CHANNEL            LEDC_CHANNEL_0 // Используем канал 0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Разрешение таймера для коэф. заполнения (13 бит)
#define LEDC_DUTY               4096 // Коэффициент заполнения 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY          4000 // Частота ШИМ в Гц (4 кГц)

void app_main(void)
{
    // Конфигурируем таймер
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Настраиваем канал ЩИМ LED-контролера
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_GPIO,
        .duty           = 0, // Коэффициент заполнения 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));


    uint32_t led_on = 1; // значение вкл/выкл светодиода
    while (1)

    {
        led_on = !led_on; // меняем значение вкл на выкл светодиода и наоборот
        if (led_on) 
        {
            printf("BKL LED!\n");
        }
        else
        {
            printf("BYKL LED!\n");
        }


        // Задаем коэффициент заполнения - 4096 - пол тона, 0 - не горит, поэтому умножаем на заполнение на вкл/выкл ЛЕД.
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY * led_on));
        // Применяем новые значение к выполнению
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));

        gpio_set_level(LEDC_OUTPUT_GPIO, led_on); // вкл или выкл светодиод относительно переменной
      
        vTaskDelay(1000 / portTICK_PERIOD_MS); // ждем секундочку
    }
}