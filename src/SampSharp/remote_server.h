// SampSharp
// Copyright 2018 Tim Potze
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

#include "server.h"
#include <inttypes.h>
#include <string>
#include <map>
#include <mutex>
#include <sampgdk/sampgdk.h>
#include <time.h>
#include "message_queue.h"
#include "callbacks_map.h"
#include "natives_map.h"
#include "commsvr.h"
#include "intermission.h"

#define LEN_NETBUF          (1024 * 32)

/** a remote running game mode server */
class remote_server : public server {
public:
    remote_server(plugin *plg, commsvr *communication, bool debug_check);
    ~remote_server();
    void tick() override;
    void public_call(AMX *amx, const char *name, cell *params, cell *retval)
        override;
    /** terminates the game mode server in an error state */
    void terminate(const char *context);

private: /* fields */
    /** statuses */
    enum status {
        status_none                 = 0,
        status_client_connected     = (1 << 0), /* connected to client */
        status_client_started       = (1 << 1), /* has sent a CMD_START */
        status_client_received_init = (1 << 2), /* received a GMI */
        status_client_reconnecting  = (1 << 3), /* is reconnecting */
        status_server_received_init = (1 << 4), /* received GMI */
        status_client_disconnecting = (1 << 5), /* is disconnecting */
    };

    /** status flags of the server */
    status status_;
    /** map of registered callbacks */
    callbacks_map callbacks_;
    /** map of registred native functions */
    natives_map natives_;
    /** buffer */
    uint8_t buf_[LEN_NETBUF];
    /** buffer tx */
    uint8_t buftx_[LEN_NETBUF];
    /** comms */
    commsvr *communication_;
    /** lock for callbacks/ticks */
    std::recursive_mutex mutex_;
    /** intermission manager */
    intermission intermission_;
    /** should check for attached paused debuggers */
    bool debug_check_;
    /** time of last sign of life of client */
    time_t sol_;
    /** time of last tick */
    time_t tick_;
    /** expecting client to paused by debugger */
    bool is_debug_ = false;
    /** number of ticks skipped while paused by debugger */
    int ticks_skipped_ = 0;

private: /* methods */
    /* update status for new connection */
    bool connect();
    /** sends the server annoucement to the client */
    void cmd_send_announce();
    /** a value indicating whether the client is connected */
    bool is_client_connected();
    /** receives commands until an unhandled command appears */
    bool cmd_receive_unhandled(uint8_t **response, uint32_t *len);
    /** receive an unhandled command */
    cmd_status cmd_receive_one(uint8_t **response, uint32_t *len);
    /** processes a command */
    cmd_status cmd_process(uint8_t cmd, uint8_t *buf, uint32_t buflen,
        uint8_t **resp, uint32_t *resplen);
    /** store current time as last interaction time */
    void store_time();
    /** a guessed value whether the client is paused by a debugger */
    bool is_debugging(bool is_tick);
    /** clears local buffers and disconnect client */
    void disconnect(const char *context, bool expected = false);

private: /* commands */
#define CMD_DEFINE(name) void remote_server::name(uint8_t *buf, uint32_t buflen)
#define CMD_DECLARE(name) void name(uint8_t *buf, uint32_t buflen)
    CMD_DECLARE(cmd_ping);
    CMD_DECLARE(cmd_print);
    CMD_DECLARE(cmd_register_call);
    CMD_DECLARE(cmd_find_native);
    CMD_DECLARE(cmd_invoke_native);
    CMD_DECLARE(cmd_reconnect);
    CMD_DECLARE(cmd_start);
    CMD_DECLARE(cmd_disconnect);
    CMD_DECLARE(cmd_alive);
#undef CMD_DECLARE
};
