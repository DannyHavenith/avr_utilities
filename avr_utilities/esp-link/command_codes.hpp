//
//  Copyright (C) 2017 Danny Havenith
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef ESP_LINK_COMMAND_CODES_HPP_
#define ESP_LINK_COMMAND_CODES_HPP_
#include <stdint.h>
namespace esp_link {
      namespace commands {
      constexpr uint16_t CMD_SYNC           = 1; /**< Synchronize; starts the protocol */
      constexpr uint16_t CMD_RESP_V         = 2; /**< Response with a value */
      constexpr uint16_t CMD_RESP_CB        = 3; /**< Response with a callback */
      constexpr uint16_t CMD_WIFI_STATUS    = 4; /**< Get the wifi status */
      constexpr uint16_t CMD_CB_ADD         = 5; /**< Add a custom callback */
      constexpr uint16_t CMD_CB_EVENTS      = 6;

      constexpr uint16_t CMD_GET_TIME       = 7; /**< Get current time in seconds since the unix epoch */

      constexpr uint16_t CMD_MQTT_SETUP     = 10; /**< Register callback functions */
      constexpr uint16_t CMD_MQTT_PUBLISH   = 11; /**< Publish MQTT topic */
      constexpr uint16_t CMD_MQTT_SUBSCRIBE = 12; /**< Subscribe to MQTT topic */
      constexpr uint16_t CMD_MQTT_LWT       = 13; /**< Define MQTT last will */

      constexpr uint16_t CMD_REST_SETUP     = 20; /**< Setup REST connection */
      constexpr uint16_t CMD_REST_REQUEST   = 21; /**< Make request to REST server */
      constexpr uint16_t CMD_REST_SETHEADER = 22; /**< Define HTML header */

      constexpr uint16_t CMD_WEB_SETUP      = 30; /**< web-server setup */
      constexpr uint16_t CMD_WEB_DATA       = 31; /**< used for publishing web-server data */

      constexpr uint16_t CMD_SOCKET_SETUP   = 40; /**< Setup socket connection */
      constexpr uint16_t CMD_SOCKET_SEND    = 41; /**< Send socket packet */
}; /**< Enumeration of commands supported by esp-link; this needs to match the definition in esp-link! */
}
#endif /* ESP_LINK_COMMAND_CODES_HPP_ */
