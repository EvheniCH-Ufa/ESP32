#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED GPIO_NUM_2   // заменил на 2-й, т.к. 32 не работает в Пиксимлабе
#define RELAY GPIO_NUM_4 // заменил на 4-й, т.к. 33 не работает в Пиксимлабе


// Напишите программу для данной схемы, чтобы реле переключалось 1 раз в 10 секунд,
// а светодиод переключался с частотой 1 Гц пока реле обесточено (раз в секунду)
// и 5 Гц, когда реле включено (5 раз в секунду).

void app_main(void)
{
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(RELAY, GPIO_MODE_INPUT_OUTPUT);


    uint32_t on_relay = 0; 
    uint32_t on_led = 0; 
    uint32_t ticks = 0;
    printf("ticks %lu: Start Cicle\n", ticks);
    while (true)
    {
        if (ticks % 20 == 0)  // норм - раз в 10 сек
            {
                //  так понимаю, что цель считывать реле, потому и считываем его постоянно 
                gpio_set_level(RELAY, !gpio_get_level(RELAY));
                on_relay = gpio_get_level(RELAY);   // current status of relay
                printf("ticks %lu: Check RELAY to%lu\n", ticks, on_relay);
            }
        
        if (on_relay)
        {
            on_led = !on_led;
            gpio_set_level(LED, on_led);
            printf("ticks %lu: Check LED to%lu\n", ticks, on_led);
        }    
        else
        {
            if (ticks % 5 == 0)
            {
                on_led = !on_led;
                gpio_set_level(LED, on_led);
                printf("ticks %lu: Check LED to%lu\n", ticks, on_led);
            }    
        }
        ticks++;
        vTaskDelay(200 / portTICK_PERIOD_MS); // 5 раз в секунду. на 10 сек - 50 раз
    }
}