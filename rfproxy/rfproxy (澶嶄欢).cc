#include <stdint.h>
//#include <map>
#include <linux/if_ether.h>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

//#include <mongo/client/dbclient.h>

#include <boost/bind.hpp>

#include "rfproxy.hh"
#include "assert.hh"
#include "component.hh"
#include "vlog.hh"
#include "datapath-join.hh"
#include "datapath-leave.hh"
#include "buffer.hh"
#include "flow.hh"
#include "netinet++/ethernet.hh"
//#include "packets.h"

//the following .h are copyed from component SWITCH
#include "openflow-default.hh"
//#include "fnv_hash.hh"
//#include "hash_set.hh"
#include "ofp-msg-event.hh"
#include "flowmod.hh"

#include "IPC.h"
#include "MongoIPC.h"
#include "RFProtocol.h"
#include "RFProtocolFactory.h"
#include "converter.h"
#include "defs.h"
//#include "openflow/openflow.h"
//the followings are maybe needed
#include "../../../oflib/ofl-actions.h"
#include "../../../oflib/ofl-messages.h"
//#include "../../../oflib/ofl.h"
//#include "../../../oflib/ofl-packets.h"
//#include "../../../oflib/ofl-structs.h"
//#include "../../../oflib/ofl-utils.h"
//#include "../../../oflib/oxm-match.h"
#include <map>

#define FAILURE 0
#define SUCCESS 1

#define OFP_BUFFER_NONE 0xffffffff

// TODO: proper support for ID
#define ID 0

namespace vigil {

static Vlog_module lg("rfproxy");

//boost::mutex sendPacketMutex;


// Event handlers
Disposition rfproxy::on_datapath_up(const Event& e) {

    return CONTINUE;
}

Disposition rfproxy::on_datapath_down(const Event& e) {

    return CONTINUE;
}

Disposition rfproxy::on_packet_in(const Event& e) {
 
    return CONTINUE;
}

// IPC message processing
bool rfproxy::process(const string &from, const string &to,
                      const string &channel, IPCMessage& msg) {
    return true;
}

// Initialization
void rfproxy::configure(const Configuration* c) {
    lg.dbg("Configure called");
}

void rfproxy::install() {
    lg.dbg("!!!!!!!!!!!!!1#");
    ipc = new MongoIPCMessageService(MONGO_ADDRESS, MONGO_DB_NAME, to_string<uint64_t>(ID));
//new MongoIPCMessageService(MONGO_ADDRESS, MONGO_DB_NAME, to_string<uint64_t>(ID));
    factory = new RFProtocolFactory();
    //ipc->listen(RFSERVER_RFPROXY_CHANNEL, factory, this, false);

    register_handler(Datapath_join_event::static_get_name(), boost::bind(&rfproxy::on_datapath_up, this, _1));
	register_handler(Datapath_leave_event::static_get_name(), boost::bind(&rfproxy::on_datapath_down, this, _1));
	register_handler(Ofp_msg_event::get_name(OFPT_PACKET_IN), boost::bind(&rfproxy::on_packet_in, this, _1));
}

void rfproxy::getInstance(const Context* c, rfproxy*& component) {
    component = dynamic_cast<rfproxy*>
        (c->get_by_interface(container::Interface_description(
            typeid(rfproxy).name())));
}

REGISTER_COMPONENT(Simple_component_factory<rfproxy>, rfproxy);
}
