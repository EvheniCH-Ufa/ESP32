#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc.h"
#include "soc/gpio_reg.h"
#include <stdbool.h>
#include <stdint.h>


/*
    Создайте новый проект в ESP-IDF. Назовите его, например, buttons.
    Скопируйте представленный листинг программы в файл main.c.
    Сохраните проект.
    Выполните компиляцию и загрузите прошивку в микроконтроллер.
    При нажатии кнопки, светодиод на плате, должен включаться. В последовательный порт выводятся сообщения. 

Задания

    Реализуйте в программе переключение величины задержки не по нажатию одной кнопки, а сразу двух или трех.
    - 0 кнопок- горит;
    - 1 кнопка - переключается раз в 4 секунды;
    - 2 кнопки - переключается раз в 3 секунды;
    - 3 кнопки - переключается раз в 2 секунды;
    - 4 кнопки - переключается раз в 1 секунду.

    Изменит схему и программу так, чтобы задействовать GPIO25, GPIO26, GPIO32 и GPIO33.0
    - вместо кнопок поставил переключатели - switch
    - 32 и 33 не работают в Picsimlab - заняты чем-то другим, наверное, подключил 4, 5, 25, 26 
*/


// Макрос для перевода номера GPIO в битовую маску
#define GPIO(gpio_num) (1ULL << (gpio_num))
#define LED GPIO_NUM_2

const uint32_t  BUTTON_PORT1 = 4;
const uint32_t  BUTTON_PORT2 = 5;
const uint32_t  BUTTON_PORT3 = 25;
const uint32_t  BUTTON_PORT4 = 26;

const uint32_t MAX_BUTTONS = 4;
const uint32_t TICKS_PER_SECOND = 100;

inline bool is_pin_set(uint64_t data, uint64_t pin)
{
    return (data & pin) != 0;
}

void app_main()
{
    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = GPIO(BUTTON_PORT1) | GPIO(BUTTON_PORT2) | GPIO(BUTTON_PORT3) | GPIO(BUTTON_PORT4),
        .pull_up_en = GPIO_PULLUP_DISABLE, // Установите GPIO_PULLUP_ENABLE, если НЕ используете внешние резисторы
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
    gpio_config(&io_config);

    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    bool led_state = false;
    uint32_t delay = 1000;
    uint32_t ticks = 0;

    while (1)
    {
        uint64_t io_data = REG_READ(GPIO_IN_REG);

        if (ticks % 10 == 0)
        {
            printf("GPIO_IN_REG: %llu\n",    io_data);
            printf("Number TiCKS is: %ld\n", ticks);
            printf("Is GPIO%ld set? %d\n",   BUTTON_PORT1, is_pin_set(io_data, GPIO(BUTTON_PORT1)));
            printf("Is GPIO%ld set? %d\n",   BUTTON_PORT2, is_pin_set(io_data, GPIO(BUTTON_PORT2)));
            printf("Is GPIO%ld set? %d\n",   BUTTON_PORT3, is_pin_set(io_data, GPIO(BUTTON_PORT3)));
            printf("Is GPIO%ld set? %d\r\n", BUTTON_PORT4, is_pin_set(io_data, GPIO(BUTTON_PORT4)));
        }

        uint32_t push_buttons_count = is_pin_set(io_data, GPIO(BUTTON_PORT1))
                                    + is_pin_set(io_data, GPIO(BUTTON_PORT2))
                                    + is_pin_set(io_data, GPIO(BUTTON_PORT3))
                                    + is_pin_set(io_data, GPIO(BUTTON_PORT4));
                              
        if (push_buttons_count == 0) // не нажато ничего
        {
            if (!led_state)
            {
                led_state = true;
                gpio_set_level(LED, led_state);
            }
        }
        else 
        {
            uint32_t chastota_miganija_per_second = (1 + MAX_BUTTONS - push_buttons_count)
                                                  * (1000/TICKS_PER_SECOND);
                                                  
            if (ticks  % chastota_miganija_per_second == 0)
            {
                led_state = !led_state;
                gpio_set_level(LED, led_state);
            }
        }
        vTaskDelay(TICKS_PER_SECOND / portTICK_PERIOD_MS);
        ++ticks;
    }
}