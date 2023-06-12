#include <arduino_homekit_server.h>
#include "log.h"

//==============================
// HomeKit setup and loop
//==============================

// access your HomeKit characteristics defined in my_accessory.c
extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t cha_desk_up;

// homekit_storage_reset(); // to remove the previous HomeKit pairing storage when you first run this new HomeKit example

void homekit_setup()
{
    arduino_homekit_setup(&config);
}

void homekit_loop(bool should_log)
{
    arduino_homekit_loop();
    if (should_log)
    {
        // LOG_D("Free heap: %d, HomeKit clients: %d",
        //       ESP.getFreeHeap(), arduino_homekit_connected_clients_count());
    }
}
