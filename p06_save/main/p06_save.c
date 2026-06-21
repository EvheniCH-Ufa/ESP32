#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
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



    секунд открыто 10
    тиков в цикле 10
    100 циклов на секунду

    
    то есть тайм = 10 * 1000 / 10

*/


// Макрос для перевода номера GPIO в битовую маску
#define GPIO(gpio_num) (1ULL << (gpio_num))

#define LED GPIO_NUM_2

const uint32_t  BUTTON_PORT_A =  4;  // 1
const uint32_t  BUTTON_PORT_B =  5;  // 2
const uint32_t  BUTTON_PORT_C = 25;  // 3
const uint32_t  BUTTON_PORT_D = 26;  // 4

const uint32_t  BUTTON_PORT_CLEAR = 27;  // switcher

const uint32_t  DOOR_CLOSE_POS_PORT          = 12;
const uint32_t  DOOR_CLOSE_TO_OPEN_POS_PORT  = 13;
const uint32_t  DOOR_OPEN_POS_PORT           = 14;
const uint32_t  DOOR_OPEN_TO_CLOSE_POS_PORT  = 15;
const uint32_t  DOOR_HOME_POS_PORT           = 16;



const uint32_t PASSWORD = 1234;  // ABCD
const uint32_t OPEN_DOOR_TIMEOUT = 10;  // seconds
const double LED_CHANGE_TIMEOUT  = .5;  // seconds
const double TICKS_PER_CICLE = 10;


// для перевода секунд в циклы
inline uint32_t SecToCicle(double sec_count)
{
    return  (1 * sec_count * 1000 / TICKS_PER_CICLE);
} 


inline bool is_pin_set(uint64_t data, uint64_t pin)
{
    return (data & pin) != 0;
}


void Rotate180(uint32_t x)
{
        //0
        gpio_set_level(DOOR_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_CLOSE_TO_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_TO_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_HOME_POS_PORT, false);

        gpio_set_level(DOOR_HOME_POS_PORT, true);

        for (uint32_t i = 0; i < x; ++i)
    {
        //4
        gpio_set_level(DOOR_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_CLOSE_TO_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_TO_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_HOME_POS_PORT, false);

        gpio_set_level(DOOR_OPEN_POS_PORT, true);

        //1
        gpio_set_level(DOOR_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_CLOSE_TO_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_TO_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_HOME_POS_PORT, false);

        gpio_set_level(DOOR_OPEN_TO_CLOSE_POS_PORT, true);

        //2
        gpio_set_level(DOOR_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_CLOSE_TO_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_TO_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_HOME_POS_PORT, false);

        gpio_set_level(DOOR_CLOSE_POS_PORT, true);

        //3
        gpio_set_level(DOOR_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_CLOSE_TO_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_TO_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_HOME_POS_PORT, false);

        gpio_set_level(DOOR_CLOSE_TO_OPEN_POS_PORT, true);

        printf("%ld%%\n", (i+1) * 100/x);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}


void Rotate_180(uint32_t x)
{
    for (uint32_t i = 0; i < x; ++i)
    {
        //2
        gpio_set_level(DOOR_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_CLOSE_TO_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_TO_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_HOME_POS_PORT, false);

        gpio_set_level(DOOR_CLOSE_POS_PORT, true);

        //1
        gpio_set_level(DOOR_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_CLOSE_TO_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_TO_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_HOME_POS_PORT, false);

        gpio_set_level(DOOR_OPEN_TO_CLOSE_POS_PORT, true);

        //4
        gpio_set_level(DOOR_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_CLOSE_TO_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_TO_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_HOME_POS_PORT, false);

        gpio_set_level(DOOR_OPEN_POS_PORT, true);


        //3
        gpio_set_level(DOOR_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_CLOSE_TO_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_POS_PORT, false);
        gpio_set_level(DOOR_OPEN_TO_CLOSE_POS_PORT, false);
        gpio_set_level(DOOR_HOME_POS_PORT, false);

        gpio_set_level(DOOR_CLOSE_TO_OPEN_POS_PORT, true);

        printf("%ld%%\n", (i+1) * 100/x);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void DoorClose()
{
    Rotate180(25);
    
    gpio_set_level(LED, true);
    printf("Door is close\n");
}


void DoorOpen()
{
   
    Rotate180(25);

    printf("Door is open\n");

    uint32_t ticks = 0;
    uint32_t target_ticks = SecToCicle(OPEN_DOOR_TIMEOUT);

    bool led_state = false;

    uint32_t door_open_timeout = OPEN_DOOR_TIMEOUT;

    while (ticks <= target_ticks)
    {
        if (ticks % SecToCicle(1) == 0)
        {
            printf("Door is open: %ld seconds\n", door_open_timeout);
            --door_open_timeout;
            led_state = !led_state;
            gpio_set_level(LED, led_state);
        }

        if (ticks % SecToCicle(LED_CHANGE_TIMEOUT) == 0)
        {
            led_state = !led_state;
            gpio_set_level(LED, led_state);
        }

        vTaskDelay(TICKS_PER_CICLE / portTICK_PERIOD_MS);
        ++ticks;
    }
    DoorClose();
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
    gpio_set_direction(BUTTON_PORT_CLEAR, GPIO_MODE_INPUT);

    gpio_set_direction(DOOR_CLOSE_POS_PORT, GPIO_MODE_OUTPUT);
    gpio_set_direction(DOOR_CLOSE_TO_OPEN_POS_PORT, GPIO_MODE_OUTPUT);
    gpio_set_direction(DOOR_OPEN_POS_PORT, GPIO_MODE_OUTPUT);
    gpio_set_direction(DOOR_OPEN_TO_CLOSE_POS_PORT, GPIO_MODE_OUTPUT);
    gpio_set_direction(DOOR_HOME_POS_PORT, GPIO_MODE_OUTPUT);


    bool led_state = true;
    gpio_set_level(LED, led_state);

    bool door_open = false;
    gpio_set_level(DOOR_CLOSE_POS_PORT, door_open);
    gpio_set_level(DOOR_OPEN_POS_PORT, door_open);


    uint32_t ticks = 0;
     
    // выравнивание мотора
    Rotate_180(1);
    Rotate180(2);


    while (1)
    {
        uint64_t io_data = REG_READ(GPIO_IN_REG);

        if (!is_pin_set(io_data, GPIO(BUTTON_PORT_A)))
        {
            current_password = current_password * 10 + 1;
            vTaskDelay(10 * TICKS_PER_CICLE / portTICK_PERIOD_MS);
            printf("Currenn Password = %ld\n, Target Password = %ld\n", current_password, PASSWORD);
        }
        else if (!is_pin_set(io_data, GPIO(BUTTON_PORT_B)))
        {
            current_password = current_password * 10 + 2;
            vTaskDelay(10 * TICKS_PER_CICLE / portTICK_PERIOD_MS);
            printf("Currenn Password = %ld\n, Target Password = %ld\n", current_password, PASSWORD);
        }
        else if (!is_pin_set(io_data, GPIO(BUTTON_PORT_C)))
        {
            current_password = current_password * 10 + 3;
            vTaskDelay(10 * TICKS_PER_CICLE / portTICK_PERIOD_MS);
            printf("Currenn Password = %ld\n, Target Password = %ld\n", current_password, PASSWORD);
        }
        else if (!is_pin_set(io_data, GPIO(BUTTON_PORT_D)))
        {
            current_password = current_password * 10 + 4;
            vTaskDelay(10 * TICKS_PER_CICLE / portTICK_PERIOD_MS);
            printf("Currenn Password = %ld\n, Target Password = %ld\n", current_password, PASSWORD);
        }

        if (!is_pin_set(io_data, GPIO(BUTTON_PORT_CLEAR)))
        {
            printf("Current password has clear\r\n");
            current_password = 0;
            vTaskDelay(10 * TICKS_PER_CICLE / portTICK_PERIOD_MS);
        }


        if (ticks % 50 == 0)
        {
            printf("Number TiCKS is: %ld\n", ticks);
            printf("Currenn Password = %ld\nTarget Password = %ld\n", current_password, PASSWORD);
        }
        

        if (current_password == PASSWORD)
        {
            printf("Password is true!\n");
            current_password = 0;
            DoorOpen();
        }

        vTaskDelay(TICKS_PER_CICLE / portTICK_PERIOD_MS);
        ++ticks;
    }
}