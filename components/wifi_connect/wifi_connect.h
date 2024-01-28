// Inspired by https://github.com/Mair/esp32-course

#pragma once

/// @brief WiFi connection events
typedef enum {
    /// @brief Station connected to AP and got an IP
    WIFI_STA_CONNECTED,
    /// @brief Station disconnected from AP
    WIFI_STA_DISCONNECTED
} wifi_connect_event_t;

/// @brief Function callback for WiFi events.
typedef void (*wifi_connect_event_handler)(wifi_connect_event_t event);

/// @brief Initialize networking and wifi stacks.
/// @param connect_handler Callback handler for wifi_connect_event_t events.
void wifi_connect_init(wifi_connect_event_handler connect_handler);

/// @brief Establish a WiFi STA connection.
/// @param ssid Network name
/// @param password Password
/// @param timeout Timeout in seconds to try establishing a connection.
/// @return ESP_OK if successful
esp_err_t wifi_connect_sta(uint8_t ssid[32], uint8_t password[64], int timeout);

/// @brief Setup a WiFi access point.
/// @param ssid Network name
/// @param password Password
/// @return ESP_OK if successful
esp_err_t wifi_connect_ap(uint8_t ssid[32], uint8_t password[64]);

/// @brief Stop WiFi stack and destroy allocated resources.
void wifi_disconnect(void);
