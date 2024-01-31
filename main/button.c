#include "esp_log.h"
#include "button.h"
#include "sdkconfig.h"

#ifdef CONFIG_IMPROV_WIFI_AUTHENTICATION_BUTTON

#include "iot_button.h"

static const char *const TAG = "BUTTON";

static void button_single_click_cb(void *button_handle, void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_SINGLE_CLICK");
    on_improv_authorized();
}

static void button_double_click_cb(void *button_handle, void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_DOUBLE_CLICK");
}

void init_button()
{
    // create gpio button
    button_config_t gpio_btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .long_press_time = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
        .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
        .gpio_button_config = {
            .gpio_num = CONFIG_IMPROV_WIFI_BUTTON_GPIO,
            .active_level = 0,
        },
    };
    button_handle_t gpio_btn = iot_button_create(&gpio_btn_cfg);
    if (NULL == gpio_btn) {
        ESP_LOGE(TAG, "Button create failed");
    }

    iot_button_register_cb(gpio_btn, BUTTON_SINGLE_CLICK, button_single_click_cb, NULL);
    iot_button_register_cb(gpio_btn, BUTTON_DOUBLE_CLICK, button_double_click_cb, NULL);
}

#else  // CONFIG_IMPROV_WIFI_AUTHENTICATION_BUTTON
void init_button() {}
#endif