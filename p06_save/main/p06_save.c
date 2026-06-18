#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc.h"
#include "soc/gpio_reg.h"
#include <stdbool.h>
#include <stdint.h>


/*
Задача

    Ознакомьтесь с требованиями Заказчика:
    Доступ на территорию фармацевтического склада ограничивает дверь с электронным замком.
    Этот замок должен открываться при введении корректного кода на четырехкнопочной клавиатуре.
    Замок снабжен визуальной индикацией. Пока дверь закрыта, горит светодиод.
    После ввода корректного кода, дверь открывается и остаётся открытой в течение 10 секунд,
    всё это время светодиод мигает. После чего дверь закрывается и светодиод опять светится непрерывно.
    
    Разработайте программное обеспечение для удовлетворения потребностей Заказчика.

Методические указания

    Считаем, что код открытия двери устанавливается при изготовлении замка и никогда не меняется.
    Замок может быть электромеханическим или электромагнитным (на выбор).
    Для имитации срабатывания открытия/закрытия замка достаточно воспользоваться реле.
    Подумайте о том, какой сигнал на катушке реле должен соответствовать выбранному типу замка.
    Для индикации срабатывания замка достаточно использовать встроенный на плате светодиод.

    Кнопки:
    - ГеймПад: 4, 5, 25, 26 (A B C D)
    - Лед: 2
    - Серво: Home 12, Open 13

    Логика:
    - ввод должен осуществляться в течении 2 секунд после нажатия последней кнопки, 
    - т.е. если после последней пока нет

*/


// Макрос для перевода номера GPIO в битовую маску
#define GPIO(gpio_num) (1ULL << (gpio_num))
#define LED GPIO_NUM_2

const uint32_t  BUTTON_PORT_A =  4;  // 1
const uint32_t  BUTTON_PORT_B =  5;  // 2
const uint32_t  BUTTON_PORT_C = 25;  // 3
const uint32_t  BUTTON_PORT_D = 26;  // 4

const uint32_t  DOOR_CLOSE_POS_PORT = 12;
const uint32_t  DOOR_OPEN_POS_PORT  = 13;

const uint32_t  PASSWORD = 1234;  // ABCD
const uint32_t  OPEN_DOOR_TIMEOUT = 10;  // seconds
const uint32_t TICKS_PER_CICLE = 10;


inline bool is_pin_set(uint64_t data, uint64_t pin)
{
    return (data & pin) != 0;
}

void app_main()
{
    uint32_t  current_password = 0;

    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = GPIO(BUTTON_PORT_A) | GPIO(BUTTON_PORT_B) | GPIO(BUTTON_PORT_C) | GPIO(BUTTON_PORT_D),
        .pull_up_en = GPIO_PULLUP_DISABLE, // Установите GPIO_PULLUP_ENABLE, если НЕ используете внешние резисторы
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
    gpio_config(&io_config);

    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(DOOR_CLOSE_POS_PORT, GPIO_MODE_OUTPUT);
    gpio_set_direction(DOOR_OPEN_POS_PORT, GPIO_MODE_OUTPUT);

    bool led_state = true;
    gpio_set_level(LED, led_state);

    bool door_open = false;
    gpio_set_level(DOOR_CLOSE_POS_PORT, door_open);
    gpio_set_level(DOOR_OPEN_POS_PORT, door_open);


  //  uint32_t delay = 1000;
    uint32_t ticks = 0;
    uint32_t num_ticks_door_open = 0;
    uint32_t interval_morganija_led = 100; // 

    while (1)
    {
        uint64_t io_data = REG_READ(GPIO_IN_REG);

        if (ticks % 10 == 0)
        {
            printf("GPIO_IN_REG: %llu\n",    io_data);
            printf("Number TiCKS is: %ld\n", ticks);
            printf("Is GPIO%ld set? %d\n",   BUTTON_PORT_A, is_pin_set(io_data, GPIO(BUTTON_PORT_A)));
            printf("Is GPIO%ld set? %d\n",   BUTTON_PORT_B, is_pin_set(io_data, GPIO(BUTTON_PORT_B)));
            printf("Is GPIO%ld set? %d\n",   BUTTON_PORT_C, is_pin_set(io_data, GPIO(BUTTON_PORT_C)));
            printf("Is GPIO%ld set? %d\r\n", BUTTON_PORT_D, is_pin_set(io_data, GPIO(BUTTON_PORT_D)));
        }

       /* uint32_t push_buttons_count = is_pin_set(io_data, GPIO(BUTTON_PORT_A))
                                    + is_pin_set(io_data, GPIO(BUTTON_PORT_B))
                                    + is_pin_set(io_data, GPIO(BUTTON_PORT_C))
                                    + is_pin_set(io_data, GPIO(BUTTON_PORT_D));*/


        if (door_open)
        {

            continue;



        } 
        else 
        {                                
            if (is_pin_set(io_data, GPIO(BUTTON_PORT_A)))
            {
                current_password = current_password * 10 + 1;
                gpio_set_level(BUTTON_PORT_A, false);

            }
            else if (is_pin_set(io_data, GPIO(BUTTON_PORT_B)))
            {
                current_password = current_password * 10 + 2;
                gpio_set_level(BUTTON_PORT_B, false);
            }
            else if (is_pin_set(io_data, GPIO(BUTTON_PORT_C)))
            {
                current_password = current_password * 10 + 2;
                gpio_set_level(BUTTON_PORT_C, false);
            }
            else if (is_pin_set(io_data, GPIO(BUTTON_PORT_D)))
            {
                current_password = current_password * 10 + 2;
                gpio_set_level(BUTTON_PORT_D, false);
            }
            
            if (current_password == PASSWORD)
            {
                door_open = true;
                gpio_set_level(DOOR_CLOSE_POS_PORT, false);
                gpio_set_level(DOOR_OPEN_POS_PORT, true);

                current_password = 0;
                led_state = false;
            }
        }
        
        
        
        
        
        
        vTaskDelay(TICKS_PER_CICLE / portTICK_PERIOD_MS);
        ++ticks;
    }
}