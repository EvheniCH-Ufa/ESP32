#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


// по факту это просто именованная константа = 2
// мы ее переобзываем, чтобы сказать что там у нас светодиод
#define LED GPIO_NUM_2 

void app_main(void)
{
    // устанавливаем эту ножку/порт на вывод (выход 0 или 3.3 вольта)
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    gpio_set_direction(5, GPIO_MODE_OUTPUT);

    gpio_set_direction(4, GPIO_MODE_OUTPUT);


    uint32_t led_on = 0; // значение вкл/выкл светодиода
    while (true)

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

        gpio_set_level(LED, led_on); // вкл или выкл светодиод относительно переменной
        gpio_set_level(4, led_on); // вкл или выкл светодиод относительно переменной

        uint32_t led_on5 = ! led_on;
        gpio_set_level(5, led_on5); // вкл или выкл светодиод относительно переменной

        vTaskDelay(1000 / portTICK_PERIOD_MS); // ждем секундочку
    }
}