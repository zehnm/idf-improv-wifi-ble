// Stripped down, plain C version from https://github.com/improv-wifi/sdk-cpp
#pragma once

#include <stdlib.h>
#include "esp_system.h"

typedef enum Error {
    ERROR_NONE = 0x00,
    ERROR_INVALID_RPC = 0x01,
    ERROR_UNKNOWN_RPC = 0x02,
    ERROR_UNABLE_TO_CONNECT = 0x03,
    ERROR_NOT_AUTHORIZED = 0x04,
    ERROR_UNKNOWN = 0xFF,
} improv_error_t;

typedef enum State {
    STATE_STOPPED = 0x00,
    STATE_AWAITING_AUTHORIZATION = 0x01,
    STATE_AUTHORIZED = 0x02,
    STATE_PROVISIONING = 0x03,
    STATE_PROVISIONED = 0x04,
} improv_state_t;

typedef enum Command {
    UNKNOWN = 0x00,
    WIFI_SETTINGS = 0x01,
    IDENTIFY = 0x02,
    GET_CURRENT_STATE = 0x02,
    GET_DEVICE_INFO = 0x03,
    GET_WIFI_NETWORKS = 0x04,
    BAD_CHECKSUM = 0xFF,
} improv_command_t;

static const uint8_t CAPABILITY_IDENTIFY = 0x01;
static const uint8_t IMPROV_SERIAL_VERSION = 1;

typedef enum ImprovSerialType {
    TYPE_CURRENT_STATE = 0x01,
    TYPE_ERROR_STATE = 0x02,
    TYPE_RPC = 0x03,
    TYPE_RPC_RESPONSE = 0x04
} improv_serial_type_t;

struct ImprovCommand {
    enum Command command;
    uint8_t ssid[32];                         /**< SSID of target AP. */
    uint8_t password[64];                     /**< Password of target AP. */
};

struct ImprovCommand parse_improv_data(const uint8_t *data, size_t length, bool check_checksum);
uint8_t* build_rpc_response(improv_command_t command, const char * datum[], uint8_t num_datum, bool add_checksum, uint16_t *buf_length);

const char* get_state_str(improv_state_t state);
const char* get_error_str(improv_error_t error);
