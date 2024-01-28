#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "identify.h"
#include "sdkconfig.h"

#ifdef CONFIG_IMPROV_WIFI_CAPABILITY_IDENTIFY

static const char *const TAG = "IDENTIFY";

static led_indicator_handle_t led_handle = NULL;

/**
 * @brief LED off: The improv service is stopped
 */
static const blink_step_t improv_stopped[] = {
    {LED_BLINK_HOLD, LED_STATE_OFF, 1000},
    {LED_BLINK_STOP, 0, 0},
};

/**
 * @brief slow blinking for 3 times: The improv service failed to provision the received credentials.
 */
static const blink_step_t improv_failed[] = {
    {LED_BLINK_HOLD, LED_STATE_ON, 1000},
    {LED_BLINK_HOLD, LED_STATE_OFF, 1000},
    {LED_BLINK_HOLD, LED_STATE_ON, 1000},
    {LED_BLINK_HOLD, LED_STATE_OFF, 1000},
    {LED_BLINK_HOLD, LED_STATE_ON, 1000},
    {LED_BLINK_HOLD, LED_STATE_OFF, 1000},
    {LED_BLINK_STOP, 0, 0},
};

/**
 * @brief blinking 5 times per second: Credentials are being verified and saved to the device.
 */
static const blink_step_t improv_provisioning[] = {
    {LED_BLINK_HOLD, LED_STATE_ON, 100},
    {LED_BLINK_HOLD, LED_STATE_OFF, 100},
    {LED_BLINK_LOOP, 0, 0},
};

/**
 * @brief blinking 3 times per second with a break in between for 3 seconds: The identify command has been used by the client.
 */
static const blink_step_t improv_identify[] = {
    {LED_BLINK_HOLD, LED_STATE_ON, 166},
    {LED_BLINK_HOLD, LED_STATE_OFF, 166},
    {LED_BLINK_HOLD, LED_STATE_ON, 166},
    {LED_BLINK_HOLD, LED_STATE_OFF, 166},
    {LED_BLINK_HOLD, LED_STATE_ON, 166},
    {LED_BLINK_HOLD, LED_STATE_OFF, 1000},
    {LED_BLINK_HOLD, LED_STATE_ON, 166},
    {LED_BLINK_HOLD, LED_STATE_OFF, 166},
    {LED_BLINK_HOLD, LED_STATE_ON, 166},
    {LED_BLINK_HOLD, LED_STATE_OFF, 166},
    {LED_BLINK_HOLD, LED_STATE_ON, 166},
    {LED_BLINK_HOLD, LED_STATE_OFF, 1000},
    {LED_BLINK_HOLD, LED_STATE_ON, 166},
    {LED_BLINK_HOLD, LED_STATE_OFF, 166},
    {LED_BLINK_HOLD, LED_STATE_ON, 166},
    {LED_BLINK_HOLD, LED_STATE_OFF, 166},
    {LED_BLINK_HOLD, LED_STATE_ON, 166},
    {LED_BLINK_HOLD, LED_STATE_OFF, 1000},
    {LED_BLINK_STOP, 0, 0},
};

/**
 * @brief blinking once per second: The improv service is awaiting credentials.
 */
static const blink_step_t improv_wait_credentials[] = {
    {LED_BLINK_HOLD, LED_STATE_ON, 200},
    {LED_BLINK_HOLD, LED_STATE_OFF, 800},
    {LED_BLINK_LOOP, 0, 0},
};

/**
 * @brief solid: The improv service is active and waiting to be authorized.
 */
static const blink_step_t improv_wait_authorization[] = {
    {LED_BLINK_HOLD, LED_STATE_ON, 1000},
    {LED_BLINK_LOOP, 0, 0},
};

/**
 * @brief provision done
 */
static const blink_step_t improv_provisioned[] = {
    {LED_BLINK_HOLD, LED_STATE_OFF, 1000},
    {LED_BLINK_STOP, 0, 0},
};

blink_step_t const *led_mode[] = {
    [LED_IMPROV_STOPPED] = improv_stopped,
    [LED_IMPROV_FAILED] = improv_failed,
    [LED_IMPROV_PROVISIONED] = improv_provisioned,
    [LED_IMPROV_PROVISIONING] = improv_provisioning,
    [LED_IMPROV_IDENTIFY] = improv_identify,
    [LED_IMPROV_WAIT_CREDENTIALS] = improv_wait_credentials,
    [LED_IMPROV_WAIT_AUTHORIZATION] = improv_wait_authorization,
    [LED_PATTERNS_MAX] = NULL,
};
#endif

#ifdef CONFIG_IMPROV_WIFI_IDENTIFY_LED_STRIP

void init_identify()
{
    led_strip_config_t strip_config = {
        .strip_gpio_num = CONFIG_IMPROV_WIFI_IDENTIFY_LED_STRIP_GPIO,              // The GPIO that connected to the LED strip's data line
        .max_leds = CONFIG_IMPROV_WIFI_IDENTIFY_LED_STRIP_NUMBER,                  // The number of LEDs in the strip,
        .led_pixel_format = CONFIG_IMPROV_WIFI_IDENTIFY_PIXEL_FORMAT,       // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,                  // LED strip model
        .flags.invert_out =                             // whether to invert the output signal
#ifdef CONFIG_IMPROV_WIFI_IDENTIFY_LED_STRIP_INVERT
        true
#else
        false
#endif
    };

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
        .rmt_channel = 0,
#else
        .clk_src = RMT_CLK_SRC_DEFAULT,        // different clock source can lead to different power consumption
        .resolution_hz = CONFIG_IMPROV_WIFI_IDENTIFY_LED_STRIP_RESOLUTION, // RMT counter clock frequency
        .flags.with_dma =                      // DMA feature is available on ESP target like ESP32-S3
#ifdef CONFIG_IMPROV_WIFI_IDENTIFY_LED_STRIP_DMA
        true
#else
        false
#endif
#endif
    };

    led_indicator_strips_config_t strips_config = {
        .led_strip_cfg = strip_config,
        .led_strip_driver = LED_STRIP_RMT,
        .led_strip_rmt_cfg = rmt_config,
    };

    const led_indicator_config_t config = {
        .mode = LED_STRIPS_MODE,
        .led_indicator_strips_config = &strips_config,
        .blink_lists = led_mode,
        .blink_list_num = LED_PATTERNS_MAX,
    };

    led_handle = led_indicator_create(&config);
    assert(led_handle != NULL);

    ESP_LOGI(TAG, "Created LED strip object with RMT backend");
}

#endif  //CONFIG_IMPROV_WIFI_IDENTIFY_LED_STRIP

#ifdef CONFIG_IMPROV_WIFI_IDENTIFY_LED

static const char *const TAG = "IDENTIFY";

void init_identify()
{
    ESP_LOGW(TAG, "GPIO LED identify option not yet implemented");

    led_indicator_gpio_config_t gpio_config = {
        .gpio_num = CONFIG_IMPROV_WIFI_IDENTIFY_LED_GPIO,
        .is_active_level_high = CONFIG_IMPROV_WIFI_IDENTIFY_LED_ACTIVE_LEVEL,
    };

    const led_indicator_config_t config = {
        .mode = LED_GPIO_MODE,
        .led_indicator_gpio_config = &gpio_config,
        .blink_lists = led_mode,
        .blink_list_num = LED_PATTERNS_MAX,
    };

    led_handle = led_indicator_create(&config);
    assert(led_handle != NULL);
}

#endif  // CONFIG_IMPROV_WIFI_IDENTIFY_LED

// TODO not yet implemented
#if defined CONFIG_IMPROV_WIFI_IDENTIFY_LED_PWN || defined CONFIG_IMPROV_WIFI_IDENTIFY_LED_RGB
void init_identify(led_pattern_t pattern) {}
#endif

#ifdef CONFIG_IMPROV_WIFI_CAPABILITY_IDENTIFY

void identify(led_pattern_t pattern)
{
    ESP_LOGI(TAG, "Starting LED pattern: %s", get_led_pattern_str(pattern));
    led_indicator_start(led_handle, pattern);
}

void identify_stop(led_pattern_t pattern)
{
    ESP_LOGI(TAG, "Stopping LED pattern: %s", get_led_pattern_str(pattern));
    led_indicator_stop(led_handle, pattern);
}

#else
void init_identify(led_pattern_t pattern) {}
void identify(led_pattern_t pattern) {}
void identify_stop(led_pattern_t pattern) {}
#endif

const char* get_led_pattern_str(led_pattern_t pattern)
{
    switch (pattern) {
    case LED_IMPROV_STOPPED: return "IMPROV_STOPPED";
    case LED_IMPROV_FAILED: return "IMPROV_FAILED";
    case LED_IMPROV_PROVISIONED: return "IMPROV_PROVISIONED";
    case LED_IMPROV_PROVISIONING: return "IMPROV_PROVISIONING";
    case LED_IMPROV_IDENTIFY: return "IMPROV_IDENTIFY";
    case LED_IMPROV_WAIT_CREDENTIALS: return "IMPROV_WAIT_CREDENTIALS";
    case LED_IMPROV_WAIT_AUTHORIZATION: return "IMPROV_WAIT_AUTHORIZATION";
    default: return "UNKNOWN";
    }
}