#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

// Выбираем АЦП
#define ADC_UNIT                    ADC_UNIT_1
#define ADC_CHANNEL                 ADC_CHANNEL_4
#define ADC_ATTENUATION             ADC_ATTEN_DB_12

const double K = 3.3 / 4096;

void app_main(void)
{
    static int adc_raw;

    // Инициализация АЦП
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    // Настройка АЦП
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTENUATION,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &config));
    
    while (1)
    {
        // Чтение "сырых" данных
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_raw));
        printf("ADC%d Channel[%d] Raw Data: %d\n", ADC_UNIT + 1, ADC_CHANNEL, adc_raw);

        int adc_tmp_int = (adc_raw + 1) * 1000 * K;
        int precision = 2;
        printf("ADC%d Channel[%d] Volt:     %.*f\n", ADC_UNIT + 1, ADC_CHANNEL, precision, 0.001* adc_tmp_int);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Никогда не будет выполнено, но удаляем всё, что создали
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
}