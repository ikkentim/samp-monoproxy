// SampSharp
// Copyright 2017 Tim Potze
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <inttypes.h>
#include <string>
#include <map>

#include <sampgdk/sampgdk.h>
#include "message_queue.h"
#include "callbacks_map.h"
#include "natives_map.h"

#define MAX_PIPE_NAME_LEN       (256)

class server {
public:
    /** initializes and allocates required memory for the server instance */
    server();
    /** frees memory allocated by this instance */
    ~server();
    /** starts the pipe server */
    void start(const char *pipe_name);
    /** called when a server tick occurs */
    void tick();
    /** called when a public call is send from the server */
    void public_call(AMX *amx, const char *name, cell *params, cell *retval);
    /** prints text to the output */
    void print(const char *format, ...);
    /** log an error */
    void log_error(const char *format, ...);
    /** log a debug */
    void log_debug(const char *format, ...);
    /** log info */
    void log_info(const char *format, ...);
    /** log a message */
    void vlog(const char* prefix, const char *format, va_list args);

private: /* fields */
    /** statuses of received commands */
    enum cmd_status {
        conn_dead, handled, unhandled, no_cmd
    };
    /** statuses */
    enum status {
        status_none                 = 0,
        status_client_connected     = (1 << 0), /* connected to pipe */
        status_client_started       = (1 << 1), /* has sent a CMD_START */
        status_client_received_init = (1 << 2), /* received a GMI */
        status_client_reconnecting  = (1 << 3), /* is reconnecting to pipe */
        status_server_received_init = (1 << 4), /* received GMI */
    };

    /** status flags of the server */
    status status_;
    /** queue for messages to be handled by the game mode thread (send by the client) */
    message_queue queue_messages_;
    /** map of registered callbacks */
    callbacks_map callbacks_;
    /** map of registred native functions */
    natives_map natives_;
    /** common buffer for reading pipe */
    uint8_t *rxbuf_;
    /** common buffer for writing pipe */
    uint8_t *txbuf_;
    /** pipe handle */
    void *pipe_;
    /** name of the pipe to use */
    char pipe_name_[MAX_PIPE_NAME_LEN];

private: /* methods */
    /** create the pipe */
    bool pipe_create();
    /** connect to the pipe */
    bool pipe_connect();
    /* update status for new connection */
    void pipe_handle_new_connection();
    /** disconnects from pipe */
    void pipe_disconnect(const char *context, bool expected = false);
    /** sends the server annoucement to the client */
    void cmd_send_announce();
    /** a value indicating whether the pipe is connected */
    bool is_pipe_connected();
    /** sends the specified command with the specified buffer as arguments */
    bool cmd_send(uint8_t cmd, uint32_t len, uint8_t *buf);
    /** receives a single command if available */
    cmd_status cmd_receive_one(uint8_t **response, uint32_t *len);
    /** receives commands until an unhandled command appears */
    bool cmd_receive_unhandled(uint8_t **response, uint32_t *len);
    /** processes a command */
    cmd_status cmd_process(uint8_t command, uint8_t *buffer, 
        uint32_t command_len, uint8_t **response, uint32_t *len);

private: /* commands */
#define CMD_DEFINE(name) void server::name(uint8_t *buf, uint32_t buflen)
#define CMD_DECLARE(name) void name(uint8_t *buf, uint32_t buflen)
    CMD_DECLARE(cmd_ping);
    CMD_DECLARE(cmd_print);
    CMD_DECLARE(cmd_register_call);
    CMD_DECLARE(cmd_find_native);
    CMD_DECLARE(cmd_invoke_native);
    CMD_DECLARE(cmd_reconnect);
    CMD_DECLARE(cmd_start);
#undef CMD_DECLARE
};
