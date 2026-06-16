#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc.h"
#include "soc/gpio_reg.h"
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

    Изменит схему и программу так, чтобы задействовать GPIO25, GPIO26, GPIO32 и GPIO33.
    - будет


то есть если 0, то горит всегда 
суммируем количество кнопок

тики 100 мс - высокая чуствительность 
но можно менять на 4000, 3000, 2000, 10000 - очень низкая чуствительность

(5 - кол-во кнопок)*10.

еали 
если количество тиков % ХХХ == 0, то переключаем


*/


// Макрос для перевода номера GPIO в битовую маску
#define GPIO(gpio_num) (1ULL << (gpio_num))

#define LED GPIO_NUM_2

inline bool is_pin_set(uint64_t data, uint64_t pin)
{
    return (data & pin) != 0;
}

void app_main()
{
    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = GPIO(25) | GPIO(26) | GPIO(32) | GPIO(33),
        .pull_up_en = GPIO_PULLUP_DISABLE, // Установите GPIO_PULLUP_ENABLE, если НЕ используете внешние резисторы
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
    gpio_config(&io_config);

    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    bool led_state = false;
    uint32_t delay = 1000;

    while (1)
    {
        uint64_t io_data = REG_READ(GPIO_IN_REG);
        printf("GPIO_IN_REG: %llu\r\n", io_data);

        printf("Is GPIO14 set? %d\r\n", is_pin_set(io_data, GPIO(25)));
        printf("Is GPIO25 set? %d\r\n", is_pin_set(io_data, GPIO(26)));
        printf("Is GPIO26 set? %d\r\n", is_pin_set(io_data, GPIO(32)));
        printf("Is GPIO27 set? %d\r\n", is_pin_set(io_data, GPIO(33)));

        uint32_t button_count = is_pin_set(io_data, GPIO(25)
                              + is_pin_set(io_data, GPIO(26))
                              + is_pin_set(io_data, GPIO(32))
                              + is_pin_set(io_data, GPIO(33));
                              
        if (button_count)
        {
zxcvzcv
        }
        else 
        {
sdvzv
        }
        

        gpio_set_level(LED, led_state);
        led_state = !led_state;
        vTaskDelay(delay / portTICK_PERIOD_MS);
    }
}