#pragma once

/// @brief GATT register callback function for logging resource registrations (service, characteristic, or descriptor).
void log_gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);

void log_ble_gap_event(struct ble_gap_event *event, void *arg);
