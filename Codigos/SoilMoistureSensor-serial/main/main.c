#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

static const char *TAG = "SoilMoistureSensor-serial";

void app_main(void) {
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_0,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_6, &config));

    while (1) {
        int raw = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &raw));

        float voltage = (float)raw / 4095.0 * 3.3;

        ESP_LOGI(TAG, "Raw: %d, Voltage: %.2f V", raw, voltage);
        printf("Raw: %d, Voltage: %.2f V\n", raw, voltage);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
}
