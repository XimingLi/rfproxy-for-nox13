/* Copyright 2008 (C) Nicira, Inc.
 * Copyright 2009 (C) Stanford University.
 *
 * This file is part of NOX.
 *
 * NOX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NOX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NOX.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef rfproxy_HH
#define rfproxy_HH

#include "component.hh"
#include "config.h"

#include "IPC.h"
#include "RFProtocol.h"
#include "RFProtocolFactory.h"

#include "IPAddress.h"
#include "MACAddress.h"
#include "Match.hh"
#include "Action.hh"
#include "Option.hh"

#include "openflow-default.hh"
#include "fnv_hash.hh"
#include "hash_set.hh"
#include "ofp-msg-event.hh"
#include "flowmod.hh"
#include "flow.hh"

#include <vector>
#include <boost/shared_array.hpp>

#ifdef LOG4CXX_ENABLED
#include <boost/format.hpp>
#include "log4cxx/logger.h"
#else
#include "vlog.hh"
#endif

namespace vigil {
using namespace std;
using namespace vigil::container;

// Map message struct
struct eth_data {
	uint8_t eth_dst[6]; /* Destination MAC address. */
	uint8_t eth_src[6]; /* Source MAC address. */
	uint16_t eth_type; /* Packet type. */
	uint64_t vm_id; /* Number which identifies a Virtual Machine .*/
	uint8_t vm_port; /* Number of the Virtual Machine port */
}__attribute__((packed));

// Association table
typedef pair<uint64_t, uint32_t> PORT;
// We can do this because there can't be a 0xff... datapath ID or port
PORT NONE = PORT(-1, -1);
class Table {
    public:
        void update_dp_port(uint64_t dp_id, uint32_t dp_port,
                            uint64_t vs_id, uint32_t vs_port) {
            map<PORT, PORT>::iterator it;
            it = dp_to_vs.find(PORT(dp_id, dp_port));
            if (it != dp_to_vs.end()) {
                PORT old_vs_port = dp_to_vs[PORT(dp_id, dp_port)];
                vs_to_dp.erase(old_vs_port);
            }

            dp_to_vs[PORT(dp_id, dp_port)] = PORT(vs_id, vs_port);
            vs_to_dp[PORT(vs_id, vs_port)] = PORT(dp_id, dp_port);
        }

        PORT dp_port_to_vs_port(uint64_t dp_id, uint32_t dp_port) {
            map<PORT, PORT>::iterator it;
            it = dp_to_vs.find(PORT(dp_id, dp_port));
            if (it == dp_to_vs.end())
                return NONE;

            return dp_to_vs[PORT(dp_id, dp_port)];
        }

        PORT vs_port_to_dp_port(uint64_t vs_id, uint32_t vs_port) {
            map<PORT, PORT>::iterator it;
            it = vs_to_dp.find(PORT(vs_id, vs_port));
            if (it == vs_to_dp.end())
                return NONE;

            return vs_to_dp[PORT(vs_id, vs_port)];
        }

        void delete_dp(uint64_t dp_id) {
            map<PORT, PORT>::iterator it = dp_to_vs.begin();
            while (it != dp_to_vs.end()) {
                if ((*it).first.first == dp_id)
                    dp_to_vs.erase(it++);
                else
                    ++it;
            }

            it = vs_to_dp.begin();
            while (it != vs_to_dp.end()) {
                if ((*it).second.first == dp_id)
                    vs_to_dp.erase(it++);
                else
                    ++it;
            }
        }

    private:
        map<PORT, PORT> dp_to_vs;
        map<PORT, PORT> vs_to_dp;
};

class rfproxy : public Component, private IPCMessageProcessor
{
    private:
        IPCMessageService* ipc;
        IPCMessageProcessor *processor;
        RFProtocolFactory *factory;
        Table table;

        // Base methods
        //bool send_of_msg(uint64_t dp_id, uint8_t* msg);
        //bool send_of_msg(uint64_t dp_id, FlowMod *fmod);
        //bool send_packet_out(uint64_t dp_id, uint32_t port, Buffer& data);
	//bool send_packet_out(uint64_t dp_id, uint32_t port, uint32_t buffer_id);
		     
        // Event handlers
        Disposition on_datapath_up(const Event& e);
        Disposition on_datapath_down(const Event& e);
        Disposition on_packet_in(const Event& e);

        // IPC message processing
        bool process(const string &from, const string &to, const string &channel, IPCMessage& msg);

    public:
        // Initialization
        rfproxy(const Context* c, const json_object* node) : Component(c) {}
        void configure(const Configuration* c);
        void install();
        static void getInstance(const container::Context* c, rfproxy*& component);

};
}
#endif
