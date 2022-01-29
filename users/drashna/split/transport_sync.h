// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "drashna.h"

typedef union {
    uint32_t raw;
    struct {
        bool audio_enable         :1;
        bool audio_clicky_enable  :1;
        bool tap_toggling         :1;
        bool unicode_mode         :1;
        bool swap_hands           :1;
        bool host_driver_disabled :1;
    };
} user_runtime_config_t;

extern user_runtime_config_t user_state;

void keyboard_post_init_transport_sync(void);
