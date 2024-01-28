// Stripped down, plain C version from https://github.com/improv-wifi/sdk-cpp

#include <string.h>
#include <sys/param.h>  // For MIN/MAX(a, b)
#include "esp_log.h"

#include "improv.h"

struct ImprovCommand parse_improv_data(const uint8_t *data, size_t length, bool check_checksum)
{
    struct ImprovCommand improv_command;
    enum Command command = (enum Command) data[0];
    uint8_t data_length = data[1];
    size_t buffer_length = length - 2 - check_checksum;

    if (data_length != buffer_length) {
        ESP_LOGE("IMPROV", "Data length field value %d doesn't match received buffer length: %d", data_length, buffer_length);
        improv_command.command = UNKNOWN;
        return improv_command;
    }

    if (check_checksum) {
        uint8_t checksum = data[length - 1];

        uint32_t calculated_checksum = 0;
        for (uint8_t i = 0; i < length - 1; i++) {
            calculated_checksum += data[i];
        }

        if ((uint8_t) calculated_checksum != checksum) {
            improv_command.command = BAD_CHECKSUM;
            return improv_command;
        }
    }

    if (command == WIFI_SETTINGS) {
        uint8_t ssid_length = data[2];
        if (ssid_length > 32) {
            ESP_LOGE("IMPROV", "Invalid SSID length in WIFI_SETTINGS command: %d", ssid_length);
            improv_command.command = UNKNOWN;
            return improv_command;
        }

        uint8_t ssid_start = 3;
        size_t ssid_end = ssid_start + ssid_length;

        uint8_t pass_length = data[ssid_end];
        if (pass_length > 64) {
            ESP_LOGE("IMPROV", "Invalid password length in WIFI_SETTINGS command: %d", pass_length);
            improv_command.command = UNKNOWN;
            return improv_command;
        }
        size_t pass_start = ssid_end + 1;

        struct ImprovCommand improv_cmd =  {.command = command};
        // FIXME ssid & password may contain null characters! These are NOT strings, but byte buffers! See WiFi specification.
        // Using strncpy is still good enough for > 99.9% of all use cases. Most APs only allow proper UTF-8 strings :-)
        strncpy((char *)improv_cmd.ssid, (const char*)&data[ssid_start], MIN(ssid_length, sizeof(improv_cmd.ssid) - 1));
        strncpy((char *)improv_cmd.password, (const char*)&data[pass_start], MIN(pass_length, sizeof(improv_cmd.password) - 1));

        return improv_cmd;
    }

    improv_command.command = command;
    return improv_command;
}

uint8_t* build_rpc_response(improv_command_t command, const char *datum[], uint8_t num_datum, bool add_checksum, uint16_t *buf_length)
{
    uint8_t* out;
    uint16_t out_length = 3; // command + data length + checksum
    uint16_t length = 0;

    ESP_LOGI("IMPROV", "build_rpc_response, urls: %d", num_datum);

    for (uint8_t i = 0; i < num_datum; i++) {
        const char* str = datum[i];
        uint8_t len = strlen(str);
        out_length += len + 1; // length of string + datum (not zero terminated!)
    }

    ESP_LOGI("IMPROV", "build_rpc_response, out_length: %d", out_length);

    out = (uint8_t*) malloc(out_length);
    if (!out) {
        *buf_length = 0;
        return out;
    }
    *buf_length = out_length;

    out[0] = command;
    out[1] = length;

    uint16_t pos = 2;
    for (uint8_t i = 0; i < num_datum; i++) {
        const char* str = datum[i];
        uint8_t len = strlen(str);
        out[pos] = len;
        memcpy(&out[pos + 1], str, len);
        pos += len + 1;
    }

    if (add_checksum) {
        uint32_t calculated_checksum = 0;

        for (uint8_t i = 0; i < pos; i++) {
            calculated_checksum += out[pos];
        }
        out[pos] = (uint8_t)calculated_checksum;
    }
    return out;
}

const char* get_state_str(improv_state_t state)
{
    switch (state) {
    case STATE_STOPPED: return "STOPPED";
    case STATE_AWAITING_AUTHORIZATION: return "AWAITING_AUTHORIZATION";
    case STATE_AUTHORIZED: return "AUTHORIZED";
    case STATE_PROVISIONING: return "PROVISIONING";
    case STATE_PROVISIONED: return "PROVISIONED";
    default: return "UNKNOWN";
    }
}

const char* get_error_str(improv_error_t error)
{
    switch (error) {
    case ERROR_NONE: return "NONE";
    case ERROR_INVALID_RPC: return "INVALID_RPC";
    case ERROR_UNKNOWN_RPC: return "UNKNOWN_RPC";
    case ERROR_UNABLE_TO_CONNECT: return "UNABLE_TO_CONNECT";
    case ERROR_NOT_AUTHORIZED: return "NOT_AUTHORIZED";
    default: return "UNKNOWN";
    }
}