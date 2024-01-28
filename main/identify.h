#pragma once

#include "led_indicator.h"

typedef enum {
    /// @brief The improv service is stopped.
    LED_IMPROV_STOPPED,
    /// @brief WiFi connection failed.
    LED_IMPROV_FAILED,
    /// @brief The improv service has successfully provisioned the WiFi credentials.
    LED_IMPROV_PROVISIONED,
    /// @brief Credentials are being verified and saved to the device.
    LED_IMPROV_PROVISIONING,
    /// @brief The identify command has been used by the client.
    LED_IMPROV_IDENTIFY,
    /// @brief The improv service is awaiting credentials.
    LED_IMPROV_WAIT_CREDENTIALS,
    /// @brief The improv service is active and waiting to be authorized.
    LED_IMPROV_WAIT_AUTHORIZATION,
    LED_PATTERNS_MAX,
} led_pattern_t;

void init_identify();

void identify(led_pattern_t pattern);

void identify_stop(led_pattern_t pattern);

const char* get_led_pattern_str(led_pattern_t pattern);
