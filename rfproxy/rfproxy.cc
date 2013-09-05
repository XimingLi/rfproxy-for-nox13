#include <stdint.h>
#include <map>
#include <string>
#include <linux/if_ether.h>

#include <boost/thread/mutex.hpp>
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
#include "fnv_hash.hh"
#include "hash_set.hh"
#include "ofp-msg-event.hh"
#include "flowmod.hh"


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
#include <mongo/client/dbclient.h>

#define FAILURE 0
#define SUCCESS 1

#define OFP_BUFFER_NONE 0xffffffff

// TODO: proper support for ID
#define ID 0

int route_mod_count = 0;
int ovs_pkt_count = 0;
int vir_count = 0;

uint8_t mac_tmp[6];
uint8_t mac_tmp2[6];

uint8_t mac_tmp3[6];
uint8_t mac_tmp4[6];

namespace vigil {

//static Vlog_module lg("rfproxy");

boost::mutex sendPacketMutex;

//move all from OFInterface.cc to here below

/**
 * Convert the RouteFlow Match to an OpenFlow match
 *
 * Returns -1 on failure.
 */
int add_match(Flow *f, const Match& match) {
    int error = 0;
    lg.dbg("In add_match, head");
    switch (match.getType()) {
        case RFMT_IPV4: {
            lg.dbg("Type is IPV4");
            const in_addr* address = static_cast<const in_addr*>
                                     (match.getIPAddress());
			/*
			const in_addr* mask = static_cast<const in_addr*>
                                  (match.getIPMask());

            		if (address == NULL || mask == NULL) {
                		error = -1;
                		break;
            		}
            		*/
            const in_addr* mask = static_cast<const in_addr*>
                                  (match.getIPMask());
            lg.dbg("mask is ------------0x%x",mask->s_addr);


            if(address == NULL) {

				error = -1;
				break;
            }

            //int32_t ofp_mask = ofp_get_mask(*mask, OFPFW_NW_DST_SHIFT);

            //ofm_match_dl(ofm, OFPFW_DL_TYPE, ETHERTYPE_IP, 0, 0);
/******6.27*****/	    
            //f->Add_Field("eth_type", ethernet::IP);
            
            //ofm_match_nw(ofm, ofp_mask, 0, 0, 0, address->s_addr);
            uint16_t _dl_type;
	    f->get_Field<uint16_t>("eth_type",&_dl_type);
		lg.dbg("_dl_type  is ------0x%x-----\n",_dl_type);
		lg.dbg("_dl_type  is ------0x%x-----\n",_dl_type);
		lg.dbg("_dl_type  is ------0x%x-----\n",_dl_type);
		lg.dbg("_dl_type  is ------0x%x-----\n",_dl_type);
		lg.dbg("_dl_type  is ------0x%x-----\n",_dl_type);



	    if( _dl_type == 0xffff)
{	
	    	f->Add_Field("eth_type", 2048);
 }         

	       //f->Add_Field("ipv4_dst", address->s_addr);//未添加子网掩码
		
		
		lg.dbg("ipv4  is ------0x%x-----\n",address->s_addr);
           /*
         	int a=address->s_addr;
		uint8_t *tmp;
		tmp = (uint8_t *)&a;
		int b=mask->s_addr;
		uint8_t *tmp2;
		tmp2 = (uint8_t *)&b;

                lg.dbg("ipv4  is ------0x%x-----\n",a);
		mac_tmp3[0] = *tmp;//0xc0;
		mac_tmp3[1] = *(tmp+1);//0xa4;
		mac_tmp3[2] = *(tmp+2);//0x0a;
		mac_tmp3[3] = *(tmp+3);//0x00;*/
/*		
		mac_tmp3[4] = *tmp2;
		mac_tmp3[5] = *(tmp2+1);

               
		mac_tmp4[0] = *(tmp2+2);
		mac_tmp4[1] = *(tmp2+3);
		mac_tmp4[2] = 0x00;
		mac_tmp4[3] = 0x00;
                mac_tmp4[4] = 0x00;
		mac_tmp4[5] = 0x00;
*//*
		mac_tmp4[0] = *tmp2;
		mac_tmp4[1] = *(tmp2+1);
		mac_tmp4[2] = *(tmp2+2);
		mac_tmp4[3] = *(tmp2+3);*/

                //f->Add_Field("ipv4_dst",mac_tmp3,mac_tmp4);
               // f->Add_Field("ipv4_dst",0x000aa4c0,0x00ffffff);
                 //f->Add_Field("ipv4_dst",0x090000e0);//090000e0已经是反序
		//  f->Add_Field("ipv4_dst", address->s_addr);

		f->Add_Field("ipv4_dst", address->s_addr,mask->s_addr);
		//f->Add_Field("ipv4_dst", address->s_addr);
            break;
        }
        case RFMT_ETHERNET:
	    lg.dbg("Type is ETHERNET");
            //ofm_match_dl(ofm, OFPFW_DL_DST, 0, 0, match.getValue());
	    uint8_t eth_dst[6];
	    lg.dbg("ETHERNET  length is ------0x%x-----\n",match.getLength());
	    lg.dbg("ETHERNET  is ------0x%x-----\n",*match.getValue());
	    lg.dbg("ETHERNET  is ------0x%x-----\n",*(match.getValue()+1));
	    lg.dbg("ETHERNET  is ------0x%x-----\n",*(match.getValue()+2));
	    lg.dbg("ETHERNET  is ------0x%x-----\n",*(match.getValue()+3));
	    lg.dbg("ETHERNET  is ------0x%x-----\n",*(match.getValue()+4));
	    lg.dbg("ETHERNET  is ------0x%x-----\n",*(match.getValue()+5));

	    lg.dbg("ETHERNET  is ------0x%x-----\n",match.getUint32());
		eth_dst[0] = *match.getValue();
		eth_dst[1] = *(match.getValue()+1);
		eth_dst[2] = *(match.getValue()+2);
		eth_dst[3] = *(match.getValue()+3);
		eth_dst[4] = *(match.getValue()+4);
		eth_dst[5] = *(match.getValue()+5);

            f->Add_Field("eth_dst", eth_dst);
            break;
        case RFMT_ETHERTYPE:
	    lg.dbg("Type is ETHERTYPE");
            //ofm_match_dl(ofm, OFPFW_DL_TYPE, match.getUint16(), 0, 0);
	    f->Add_Field("eth_type", match.getUint16());
            break;
        case RFMT_NW_PROTO:
 	    lg.dbg("Type is NW_PROTO");
            //ofm_match_nw(ofm, OFPFW_NW_PROTO, match.getUint8(), 0, 0, 0);
            f->Add_Field("ip_proto", match.getUint8());
            break;
        case RFMT_TP_SRC:
	    lg.dbg("Type is TP_SRC");
            //ofm_match_tp(ofm, OFPFW_TP_SRC, match.getUint16(), 0);
	    f->Add_Field("tcp_src", match.getUint16());
            break;
        case RFMT_TP_DST:
	    lg.dbg("Type is TP_DST");
            //ofm_match_tp(ofm, OFPFW_TP_DST, 0, match.getUint16());
	    f->Add_Field("tcp_dst", match.getUint16());
            break;
        case RFMT_IN_PORT:
  	    lg.dbg("Type is IN_PORT");
	    //lg.dbg("IN_PORT  is ------0x%x-----\n",match.getUint16());
            //ofm_match_in(ofm, static_cast<uint16_t>(match.getUint32()));
	    //f->Add_Field("in_port", static_cast<uint16_t>(match.getUint32()));
            f->Add_Field("in_port", match.getUint32());
	    break;
        case RFMT_IPV6://T.B.D.
        case RFMT_MPLS://T.B.D.
            // Not implemented in OpenFlow 1.0.
        default:
	    lg.dbg("Type is default");
            error = -1;
            break;
    }
    lg.dbg("In add_match, end");

    return error;
}

/**
 * Convert the RouteFlow Action to an OpenFlow Action
 *
 * Returns -1 on failure.
 */
int add_action(Actions *acts, const Action& action) {
    lg.dbg("In add_action, head");
    int error = 0;
    //ofp_action_header *oah = reinterpret_cast<ofp_action_header*>(buf);
    switch (action.getType()) {
        case RFAT_OUTPUT:
            //ofm_set_action(oah, OFPAT_OUTPUT, action.getUint16(), 0);
	    //port should be 32??????????????????????????? in nox13
		lg.dbg("action---getUnit32----is----------0x%x", action.getUint32());
		if(action.getUint32() == OFPP_CONTROLLER) {
			//acts->CreateOutput(action.getUint16(), OFPCML_NO_BUFFER);
			acts->CreateOutput(OFPP_CONTROLLER, OFPCML_NO_BUFFER);
			lg.dbg("OUTPUT eq OFPP_CONTROLLER");	
		}
		else {
			//acts->CreateOutput(action.getUint16());
			acts->CreateOutput(action.getUint32());
			lg.dbg("OUTPUT neq OFPP_CONTROLLER");
		}	
        	break;
        case RFAT_SET_ETH_SRC:
        	//ofm_set_action(oah, OFPAT_SET_DL_SRC, 0, action.getValue());
		//uint8_t eth_src[6];
	lg.dbg("SET_ETH_SRC  is ------0x%x-----\n",*action.getValue());
	lg.dbg("SET_ETH_SRC  is ------0x%x-----\n",*(action.getValue()+1));
	lg.dbg("SET_ETH_SRC  is ------0x%x-----\n",*(action.getValue()+2));
	lg.dbg("SET_ETH_SRC  is ------0x%x-----\n",*(action.getValue()+3));
	lg.dbg("SET_ETH_SRC  is ------0x%x-----\n",*(action.getValue()+4));
	lg.dbg("SET_ETH_SRC  is ------0x%x-----\n",*(action.getValue()+5));
		mac_tmp[0] = *action.getValue();
		mac_tmp[1] = *(action.getValue()+1);
		mac_tmp[2] = *(action.getValue()+2);
		mac_tmp[3] = *(action.getValue()+3);
		mac_tmp[4] = *(action.getValue()+4);
		mac_tmp[5] = *(action.getValue()+5);

		acts->CreateSetField("eth_src", mac_tmp);
        	break;
        case RFAT_SET_ETH_DST:
        	//ofm_set_action(oah, OFPAT_SET_DL_DST, 0, action.getValue());
		//uint8_t eth_dst[6];
		lg.dbg("SET_ETH_DST  is ------0x%x-----\n",*action.getValue());
		lg.dbg("SET_ETH_DST  is ------0x%x-----\n",*(action.getValue()+1));
		lg.dbg("SET_ETH_DST  is ------0x%x-----\n",*(action.getValue()+2));
		lg.dbg("SET_ETH_DST  is ------0x%x-----\n",*(action.getValue()+3));
		lg.dbg("SET_ETH_DST  is ------0x%x-----\n",*(action.getValue()+4));
		lg.dbg("SET_ETH_DST  is ------0x%x-----\n",*(action.getValue()+5));
		mac_tmp2[0] = *action.getValue();
		mac_tmp2[1] = *(action.getValue()+1);
		mac_tmp2[2] = *(action.getValue()+2);
		mac_tmp2[3] = *(action.getValue()+3);
		mac_tmp2[4] = *(action.getValue()+4);
		mac_tmp2[5] = *(action.getValue()+5);

		acts->CreateSetField("eth_dst", mac_tmp2);
		break;
		// the following are T.B.D.
        case RFAT_PUSH_MPLS:
        case RFAT_POP_MPLS:
        case RFAT_SWAP_MPLS:
            // Not implemented in OpenFlow 1.0. 
        default:
            error = -1;
            break;
    }

    lg.dbg("In add_action, end");
    return error;
}

/**
 * Convert the RouteFlow Option to an OpenFlow field
 *
 * Returns -1 on failure.
 */
 //In this function is htons needed??????????????????????????????????
int add_option(FlowMod *fmod, const Option& option) {
    int error = 0;
    lg.dbg("In add_option, head");
    lg.dbg("In add_action, type is %d, 1 - RFOT_PRIORITY, 2 - RFOT_IDLE_TIMEOUT, 3 - RFOT_HARD_TIMEOUT, 255 - RFOT_CT_ID", option.getType());
    switch (option.getType()) {
        case RFOT_PRIORITY:
            //ofm->priority = htons(option.getUint16());
			fmod->fm_msg.priority = option.getUint16();
            break;
        case RFOT_IDLE_TIMEOUT:
            //ofm->idle_timeout = htons(option.getUint16());
			fmod->fm_msg.idle_timeout = option.getUint16();
            break;
        case RFOT_HARD_TIMEOUT:
            //ofm->hard_timeout = htons(option.getUint16());
			fmod->fm_msg.idle_timeout = option.getUint16();
            break;
        case RFOT_CT_ID:
            /* Ignore. */
            break;
        default:
            /* Unsupported Option. */
            error = -1;
            break;
    }

    lg.dbg("In add_option, end");
    return error;
}

void log_error(std::string type, bool optional) {
    if (optional) {
        VLOG_DBG(lg, "Dropping unsupported TLV (type: %s)", type.c_str());
    } else {
        VLOG_ERR(lg, "Failed to serialise TLV (type: %s)", type.c_str());
    }
}

FlowMod *create_flow_mod(uint8_t mod,
            std::vector<Match> matches, std::vector<Action> actions,
            std::vector<Option> options) {
    int error = 0;
    lg.dbg("In create_flow_mod, head");
/*
	ofp_flow_mod *ofm;
    size_t size = sizeof *ofm + ofp_match_len(matches)
                  + ofp_action_len(actions);

    boost::shared_array<uint8_t> raw_of(new uint8_t[size]);
    ofm = reinterpret_cast<ofp_flow_mod*>(raw_of.get());
    ofm_init(ofm, size);

    uint8_t* oah = reinterpret_cast<uint8_t*>(ofm->actions);
*/
	FlowMod *fmod = new FlowMod();
	Flow *f = new Flow();
	Actions *acts = new Actions();
	Instruction *inst =  new Instruction();

	//FlowMod init
	//cookie, cookie_mask, table_id, out_group, flags are left
	fmod->fm_msg.cookie = 0x00ULL;
	fmod->fm_msg.cookie_mask = 0x00ULL;
	fmod->fm_msg.table_id = 0;
	fmod->fm_msg.out_group = OFPP_ANY;
	fmod->fm_msg.flags = ofd_flow_mod_flags();
	fmod->fm_msg.priority = OFP_DEFAULT_PRIORITY;
	//Whether idle_timeout and hard_timeout should be init here?
	
    int count = 0;
    std::vector<Match>::iterator iter_mat;
    for (iter_mat = matches.begin(); iter_mat != matches.end(); ++iter_mat, ++count) {
        if (add_match(f, *iter_mat) != 0) {
            log_error(iter_mat->type_to_string(), iter_mat->optional());
            if (!iter_mat->optional()) {
                error = -1;
                break;
            }
        }
    }
    lg.dbg("In create_flow_mod, there %d Matches added", count);

    count = 0;
    std::vector<Action>::iterator iter_act;
    for (iter_act = actions.begin(); iter_act != actions.end(); ++iter_act, ++count) {
        if (add_action(acts, *iter_act) != 0) {
            log_error(iter_act->type_to_string(), iter_act->optional());
            if (!iter_act->optional()) {
                error = -1;
                break;
            }
        }
        //oah += ofp_len(*iter_act);
    }
    lg.dbg("In create_flow_mod, there %d Actions added", count);

	inst->CreateApply(acts);
	//set act and match fields.
	fmod->AddInstructions(inst);
	fmod->AddMatch(&f->match);

    count = 0;
	//the following set the priority, idle_timeout and hard_timeout fields
    std::vector<Option>::iterator iter_opt;
    for (iter_opt = options.begin(); iter_opt != options.end(); ++iter_opt, ++count) {
        if (add_option(fmod, *iter_opt) != 0) {
            log_error(iter_opt->type_to_string(), iter_opt->optional());
            if (!iter_opt->optional()) {
                error = -1;
                break;
            }
        }
    }
    lg.dbg("In create_flow_mod, there %d Options added", count);

	//we then should assign for other fields of a FlowMod
	//a complete FlowMod maybe like this:
	//mod = new FlowMod(0x00ULL,0x00ULL, 0,OFPFC_ADD, OFP_FLOW_PERMANENT, OFP_FLOW_PERMANENT, 0, 0, OFPP_ANY, OFPG_ANY, ofd_flow_mod_flags());
    lg.dbg("In create_flow_mod, the input mod is 0x%x", mod);
    switch (mod) {
        case RMT_ADD:
			/*while(1)
        {
              lg.dbg("modmodmdomdomdodmodmodmdom!!!!!!!!!!!!!!!!!!!!!!");
        }*/
            //ofm_set_command(ofm, OFPFC_ADD);
			fmod->fm_msg.command = OFPFC_ADD;
			fmod->fm_msg.buffer_id = OFP_BUFFER_NONE;
			fmod->fm_msg.idle_timeout = OFP_FLOW_PERMANENT;
			fmod->fm_msg.hard_timeout = OFP_FLOW_PERMANENT;
			fmod->fm_msg.out_port = OFPP_ANY; //this is different with the former version, in which it is OFPP_NONE 
            break;
        case RMT_DELETE:
            //ofm_set_command(ofm, OFPFC_DELETE_STRICT);
           //while(1)
             // sleep(1000000000000000);
              //lg.dbg("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

			fmod->fm_msg.command = OFPFC_DELETE;  //OFPFC_DELETE_STRICT
			fmod->fm_msg.buffer_id = OFP_BUFFER_NONE;
			fmod->fm_msg.idle_timeout = OFP_FLOW_PERMANENT;
			fmod->fm_msg.hard_timeout = OFP_FLOW_PERMANENT;
			fmod->fm_msg.out_port = OFPP_ANY; //this is different with the former version, in which it is OFPP_NONE 
            break;
        default:
            VLOG_ERR(lg, "Unrecognised RouteModType (type: %d)", mod);
            error = -1;
            break;
    }

	//ofm_init(ofp_flow_mod * ofm,size_t size);
	
    if (error != 0) {
        return NULL;
    }
    
    lg.dbg("In create_flow_mod, end");
    return fmod;
}


// Base functions
bool rfproxy::send_of_msg(uint64_t dp_id, FlowMod *fmod) {
    lg.dbg("In send_of_msg, head");
    boost::lock_guard<boost::mutex> lock(sendPacketMutex);
	//Modified by XML
	//if (send_openflow_command(datapathid::from_host(dp_id), (ofp_header*) msg, true))
    if ((struct ofl_msg_header *)&(fmod->fm_msg) == NULL) 
        lg.dbg("(struct ofl_msg_header *)&(fmod->fm_msg) is fucking NULL");
    lg.dbg("Dare u be here??");
 /*
Flow *f = new Flow();
f->Add_Field("in_port",0);
Actions *acts = new Actions();
acts->CreateOutput(1);
Instruction *inst = new Instruction();
inst->CreateApply(acts);
FlowMod *mod = new FlowMod(0x00ULL,0x00ULL, 0,OFPFC_ADD, OFP_FLOW_PERMANENT, OFP_FLOW_PERMANENT, 0, 0, 
OFPP_ANY, OFPG_ANY, ofd_flow_mod_flags());
mod->AddMatch(&f->match);
mod->AddInstructions(inst);
lg.dbg("Dare u be here 2??");
//send_openflow_msg(dpj.dpid, (struct ofl_msg_header *)&mod->fm_msg, 0, true);
lg.dbg("datapathid net = %ld", datapathid::from_host(dp_id).as_net());
lg.dbg("datapathid host = %ld", datapathid::from_host(dp_id).as_host());
if (send_openflow_msg(datapathid::from_host(dp_id), (struct ofl_msg_header *)&mod->fm_msg, 0, true)) {


*/
    if (send_openflow_msg(datapathid::from_host(dp_id), (struct ofl_msg_header *)&(fmod->fm_msg), 0/*xid*/, true/*block*/)) {
	lg.dbg("In send_of_msg, end, FAILURE");        
	return FAILURE;
    }
    else {
	lg.dbg("In send_of_msg, end, SUCCESS");
        return SUCCESS;
    }
	lg.dbg("In send_of_msg, end");
}

//should be Buffer * ?????????????????????????????????????????
bool rfproxy::send_packet_out(uint64_t dp_id, uint32_t port, Buffer& data) {
    boost::lock_guard<boost::mutex> lock(sendPacketMutex);
    //if (send_openflow_pkt(datapathid::from_host(dp_id), data, port, OFPP_NONE, true))
	if(send_openflow_pkt(datapathid::from_host(dp_id), data, OFPP_ANY,
							port, true))
        return FAILURE;
    else
        return SUCCESS;
}
/*
bool rfproxy::send_packet_out(uint64_t dp_id, uint32_t port, uint32_t buffer_id) {
    boost::lock_guard<boost::mutex> lock(sendPacketMutex);
    //if (send_openflow_pkt(datapathid::from_host(dp_id), data, port, OFPP_NONE, true))
	if(send_openflow_pkt(datapathid::from_host(dp_id), buffer_id, OFPP_ANY, port, true))//datapathid::from_host(dp_id) right????????????????????
        return FAILURE;
    else
        return SUCCESS;
}
*/
// Event handlers
Disposition rfproxy::on_datapath_up(const Event& e) {
/*
    const Datapath_join_event& dj = assert_cast<const Datapath_join_event&> (e);

    for (int i = 0; i < dj.ports.size(); i++) {
        if (dj.ports[i].port_no <= OFPP_MAX) {
            DatapathPortRegister msg(ID, dj.dpid.as_host(), dj.ports[i].port_no);
            ipc->send(RFSERVER_RFPROXY_CHANNEL, RFSERVER_ID, msg);

            VLOG_INFO(lg,
                      "Registering datapath port (dp_id=%0#"PRIx64", dp_port=%d)",
                      dj.dpid.as_host(),
                      dj.ports[i].port_no);
        }
    }
*/
	lg.dbg("In rfproxy, received a Datapath_join_event\n");
	const Datapath_join_event& dpj = assert_cast<const Datapath_join_event&>(e);
 /*lxm 13.07.8*/       
        if(IS_RFVS(dpj.dpid.as_host())) {
	
	        Flow f;
		Actions acts;
		acts.CreateOutput(OFPP_CONTROLLER, OFPCML_NO_BUFFER);
		Instruction inst;
		inst.CreateApply(&acts);
		FlowMod fmod(0x00ULL,0x00ULL, 0,OFPFC_ADD, OFP_FLOW_PERMANENT, OFP_FLOW_PERMANENT, 0, 0,OFPP_ANY, OFPG_ANY,ofd_flow_mod_flags());


		fmod.AddMatch(&(f.match));
		fmod.AddInstructions(&inst);


		send_openflow_msg(dpj.dpid, (struct ofl_msg_header *)&fmod.fm_msg, 0/*xid*/, true/*block*/);
	}

	//if (dpj.dpid.as_host() > 0x0000ffffffffffffULL)
	//	lg.warn("Joined DPID has no 0x0000 in upper 16-bits, so rfproxy will not work correctly");
	else {

	//Request the port infomation from the joined Dp.
	VLOG_DBG(lg,"Request the port information from the dpid= 0x%"PRIx64"\n", dpj.dpid.as_host());
	struct ofl_msg_multipart_request_port ommrpt;
	ommrpt.header.header.type = OFPT_MULTIPART_REQUEST;
	ommrpt.header.type = OFPMP_PORT_DESC;
	ommrpt.header.flags = OFPMPF_REQ_MORE;
	ommrpt.port_no = OFPP_ANY;
	// need lock here?
	
	send_openflow_msg(dpj.dpid, (struct ofl_msg_header *)&ommrpt, 0/*xid*/, true/*block*/);
	}    
	return CONTINUE;
}

Disposition rfproxy::on_port_status_in(const Event& e) {

	lg.dbg("In rfproxy, received a port_status event\n");
	const Ofp_msg_event& ome = assert_cast<const Ofp_msg_event&>(e);
	struct ofl_msg_port_status *ps = (struct ofl_msg_port_status *)**ome.msg;
	/*if (ps->reason == 0)
		lg.dbg("The dpid = %0#"PRIx64" has added a port, whose port num is %d\n", ome.dpid.as_host(), (*(ps->desc)).port_no);
	if (ps->reason == 1)
		lg.dbg("The dpid = %0#"PRIx64" has deleted a port, whose port num is %d\n", ome.dpid.as_host(), (*(ps->desc)).port_no);
	if (ps->reason == 2)
		lg.dbg("The dpid = %0#"PRIx64" has changed a port, whose port num is %d\n", ome.dpid.as_host(), (*(ps->desc)).port_no);
	*/
	uint64_t vs_id = ome.dpid.as_host();
	if (IS_RFVS(vs_id)) {

		uint32_t vs_port = (*(ps->desc)).port_no;
		PORT dp_port;
		switch (ps->reason) {

			case OFPPR_ADD:
			  	lg.dbg("The dpid = %0#"PRIx64" has added a port, whose port num is %d\n", vs_id, vs_port);
				break;
			case OFPPR_DELETE:
				lg.dbg("The dpid = %0#"PRIx64" has deleted a port, whose port num is %d\n", vs_id, vs_port);
		
				dp_port = table.vs_port_to_dp_port(vs_id, vs_port);
				if (dp_port != NONE) {
					lg.dbg("A vm is down, the corresponding switch is dpid = %0#"PRIx64", and will delete its flow tables, except for its default flow tables", dp_port.first);
					//now will delete the flow tables, without the default ones
					DatapathPortRegister msg(ID, dp_port.first, 10000);
					ipc->send(RFSERVER_RFPROXY_CHANNEL, RFSERVER_ID, msg);
         				//table delete vs port or not
					table.delete_vs_port(vs_id, vs_port);
					
				}
        			else {
					lg.dbg("The corresponding switch is not up, so, no need to delete its flow tables!\n");
				}
				break;
			case OFPPR_MODIFY:
				lg.dbg("The dpid = %0#"PRIx64" has changed a port, whose port num is %d\n", vs_id, vs_port);
				break;

			default:
			    	lg.dbg("Received a wrong port status reason number!\n");
				break;
		}
	}
	return CONTINUE;
}

Disposition rfproxy::on_port_description_in(const Event& e) {

	lg.dbg("In rfproxy, received a port_description event\n");
	const Ofp_msg_event& ome = assert_cast<const Ofp_msg_event&>(e);
	struct ofl_msg_multipart_reply_port_desc *rpd = (struct ofl_msg_multipart_reply_port_desc *)**ome.msg;
	lg.dbg("The detaild port info of dpid = %0#"PRIx64" are as follows\n", ome.dpid.as_host());
	for (int i = 0; i < rpd->stats_num; i++) {

		struct ofl_port *port_desc = *(rpd->stats+i);
		lg.dbg("Num %d, port num is %x", i, port_desc->port_no);
		lg.dbg("OFPP_MAX is %x", OFPP_MAX);
		if (port_desc->port_no <= OFPP_MAX) {
			//Is ID OK here????????????????
			DatapathPortRegister msg(ID, ome.dpid.as_host(), port_desc->port_no);//as_host() right??????????????
            		//port_add(Port(ome.dpid, port_desc->port_no));
			ipc->send(RFSERVER_RFPROXY_CHANNEL, RFSERVER_ID, msg);
        	}
	}
	
	return CONTINUE;
}

Disposition rfproxy::on_datapath_down(const Event& e) {

	lg.dbg("In rfproxy, received a Datapath_leave_event\n");
	const Datapath_leave_event& dl = assert_cast<const Datapath_leave_event&> (e);
	uint64_t dp_id = dl.datapath_id.as_host();//as_host() right??????????????

	VLOG_INFO(lg,
        "Datapath is down (dp_id=%0#"PRIx64")",
        dp_id);

	// Delete internal entry
	table.delete_dp(dp_id);

	// Notify RFServer
	DatapathDown dd(ID, dp_id);
	ipc->send(RFSERVER_RFPROXY_CHANNEL, RFSERVER_ID, dd);

	return CONTINUE;
}

Disposition rfproxy::on_packet_in(const Event& e) {
    	
	lg.dbg("In rfproxy, received a Packet_in event\n");

	const Ofp_msg_event& ome = assert_cast<const Ofp_msg_event&>(e);
	
	struct ofl_msg_packet_in *in = (struct ofl_msg_packet_in *)**ome.msg;

	Flow *flow = new Flow((struct ofl_match*) in->match);




	uint8_t * byteptr = (uint8_t *)in;

	uint8_t * byteptr2 = (uint8_t *)in->data;

	

  //      lg.dbg("sizesize = 0x%d", sizeof(*(in->data)));
	
//	for(int i = 0; i < 50; i++)
//	{
//		lg.dbg("byte %d = 0x%x", i, *(byteptr2+i));
//	}

	uint16_t _dl_type;
	flow->get_Field<uint16_t>("eth_type",&_dl_type);
	uint32_t _in_port;
	flow->get_Field<uint32_t>("in_port", &_in_port);
	uint8_t _nw_proto;
	flow->get_Field<uint8_t>("ip_proto", &_nw_proto);
	uint16_t _tcp_src;
	flow->get_Field<uint16_t>("tcp_src", &_tcp_src);
	uint32_t _ipv4_dst;
	flow->get_Field<uint32_t>("ipv4_dst", &_ipv4_dst);
	uint32_t _ipv4_src;
	flow->get_Field<uint32_t>("ipv4_src", &_ipv4_src);
	uint8_t eth_src[6];
	flow->get_Field("eth_src", eth_src);	
	uint8_t eth_dst[6];
	flow->get_Field("eth_dst", eth_dst);
	
		
	uint64_t dp_id = ome.dpid.as_host();
	
	uint32_t in_port = _in_port;
	uint64_t vs_id = ome.dpid.as_host();
	uint32_t vs_port = _in_port;
	
	
	if (IS_RFVS(dp_id) && _dl_type != 0xffff)
		while(1)
			lg.dbg("~");

	lg.dbg("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	lg.dbg("_dl_type  is ------0x%x-----\n",_dl_type);
	lg.dbg("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");


	
    // Drop all LLDP packets
    if (_dl_type == ethernet::LLDP) {
	lg.dbg("It is a LLDP pack, we drop it");
        return CONTINUE;
    }

    // If we have a mapping packet, inform RFServer through a Map message

	//_dl_type=0x0A0A;

    //if (_dl_type == htons(RF_ETH_PROTO)) {
	if (_dl_type == 0xffff && *(in->data + 12) == 0x0A && *(in->data + 13) == 0x0A) {
	//while(1)
		//lg.dbg("SS");              
	lg.dbg("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

        //const eth_data* data = buf->try_pull<eth_data> ();
        //right?????????????????????????????????????????????????????????????????????????????????
	const eth_data* data = (struct eth_data *)in->data;
		VLOG_INFO(lg,
            "Received mapping packet (vm_id=%0#"PRIx64", vm_port=%d, vs_id=%0#"PRIx64", vs_port=%d)",
            data->vm_id,
            data->vm_port,
            vs_id,
            vs_port);

              lg.dbg("data->vm_id = %x", data->vm_id);

        // Create a map message and send it
        VirtualPlaneMap mapmsg;
        mapmsg.set_vm_id(data->vm_id);
        mapmsg.set_vm_port(data->vm_port);
        mapmsg.set_vs_id(vs_id);
        mapmsg.set_vs_port(vs_port);
        lg.dbg("Has created a VirtualPlaneMap msg, contents are: %s", mapmsg.str().c_str());
        ipc->send(RFSERVER_RFPROXY_CHANNEL, RFSERVER_ID, mapmsg);

	return CONTINUE;

	}
	
	Nonowning_buffer buf(in->data, in->data_length);


	

    	// If the packet came from RFVS, redirect it to the right switch port
    	if (IS_RFVS(dp_id)) {
    		lg.dbg("ovs_pkt_count = %d", ++ovs_pkt_count);
		lg.dbg("This pack is from OvS");
        	PORT dp_port = table.vs_port_to_dp_port(dp_id, in_port);
        	if (dp_port != NONE) {
			lg.dbg("And the pack will be sent to dp_id=%0#"PRIx64", dp_port=%x", dp_port.first, dp_port.second);
         		send_packet_out(dp_port.first, dp_port.second, buf);//buf is ok?
		}
        	else {

			table.get_dp_to_vs_map_num();
			table.get_vs_to_dp_map_num();
			VLOG_DBG(lg, "Unmapped RFVS port (vs_id=%0#"PRIx64", vs_port=%x)",
                     		dp_id, in_port);
		}
    	}
   	// If the packet came from a switch, redirect it to the right RFVS port
    	else {
		
		lg.dbg("This pack is from a Switch");
       		PORT vs_port = table.dp_port_to_vs_port(dp_id, in_port);
        	if (vs_port != NONE) {
			lg.dbg("And the pack will be sent to vs_id=%0#"PRIx64", vs_port=%x", vs_port.first, vs_port.second);
			
            		if(send_packet_out(vs_port.first, vs_port.second, buf))//buf is ok?
			{
				lg.dbg("!!!!!!!!!!!!!!!!***************send_packet_out success***************!!!!!!1");
			}
                        else
			{
				lg.dbg("!!!!!!!!!!!!!!!!***************send_packet_out fail***************!!!!!!1");
			}
		}
        	else {
			table.get_dp_to_vs_map_num();
			table.get_vs_to_dp_map_num();
            		VLOG_DBG(lg, "Unmapped datapath port (dp_id=%0#"PRIx64", dp_port=%x)",
                     	dp_id, in_port);
		}
    	}

    	return CONTINUE;
}

// IPC message processing
bool rfproxy::process(const string &from, const string &to,
                      const string &channel, IPCMessage& msg) {
	lg.dbg("In rfproxy.cc, Process called");

	int type = msg.get_type();
		lg.dbg("In process(), the type of the received msg is %d, 6 is ROUTE_MOD and 5 is DATA_PLANE_MAP", type);

	
        //lg.dbg("msg-------------------------------are: \n%s", msg.str().c_str());
	//lg.dbg("htons-------------------------------are: 0x%x", htons(0x30));
        
	if (type == ROUTE_MOD) {
		RouteMod* rmmsg = static_cast<RouteMod*>(&msg);
		
        	//boost::shared_array<uint8_t> ofmsg = create_flow_mod(rmmsg->get_mod(),
                //                    rmmsg->get_matches(),
                 //                   rmmsg->get_actions(),
                 //                   rmmsg->get_options());
        	



		lg.dbg("In process(), has received a RouteMod msg, contents are: %s", rmmsg->str().c_str());
		lg.dbg("In process, before calling create_flow_mod");


		lg.dbg("In create_flow_mod, the input mod is %c",rmmsg->get_mod());

		FlowMod *fmod = create_flow_mod(rmmsg->get_mod(),
                                    rmmsg->get_matches(),
                                    rmmsg->get_actions(),
                                    rmmsg->get_options());
		lg.dbg("In process, after calling create_flow_mod");

		//if (fmod == NULL) {
		if(rmmsg->get_id() == 0) {
			VLOG_DBG(lg, "Failed to create OpenFlow FlowMod");
        	} else {
			VLOG_DBG(lg, "The flow mod msg will sent to %0#"PRIx64"", rmmsg->get_id());
            		if(send_of_msg(rmmsg->get_id(), fmod)) {
				
				if(IS_RFVS(rmmsg->get_id()))
					VLOG_DBG(lg, "Successfully send out an openflow msg to OvS!");
				else
					VLOG_DBG(lg, "Successfully send out an openflow msg to Switch!");
			}
			else
				VLOG_DBG(lg, "Fail to send out openflow msg");
        	}
		lg.dbg("In process, num of route_mod is %d", ++route_mod_count);

    	}
    	else if (type == DATA_PLANE_MAP) {
		//while(1)
			lg.dbg("I got a DATA_PLANE_MAP!!!!!!!!!!");

		//dynamic_cast or static_cast ???
		DataPlaneMap* dpmmsg = dynamic_cast<DataPlaneMap*>(&msg);
		lg.dbg("In process(), has received a DataPlaneMap msg, contents are: %s", dpmmsg->str().c_str());
        	table.update_dp_port(dpmmsg->get_dp_id(), dpmmsg->get_dp_port(),
                             dpmmsg->get_vs_id(), dpmmsg->get_vs_port());
		table.list_dp_to_vs();
		table.list_vs_to_dp();
    	}
    	return true;
}

// Initialization
void rfproxy::configure(const Configuration* c) {
    lg.dbg("In rfproxy.cc, Configure called");
}

void rfproxy::install() {

    lg.dbg("In rfproxy.cc, install(), Install called\n");
    //lg.dbg("MONGO_ADDRESS, MONGO_DB_NAME, to_string<uint64_t>(ID) are %s, %s", MONGO_ADDRESS, MONGO_DB_NAME);
    ipc = new MongoIPCMessageService(MONGO_ADDRESS, MONGO_DB_NAME, to_string<uint64_t>(ID));
    //ipc = new MongoIPCMessageService(MONGO_ADDRESS, MONGO_DB_NAME, "0");
    lg.dbg("In rfproxy, install(), ipc created\n");
    factory = new RFProtocolFactory();
    ipc->listen(RFSERVER_RFPROXY_CHANNEL, factory, this, false);

    register_handler(Datapath_join_event::static_get_name(), boost::bind(&rfproxy::on_datapath_up, this, _1));
    register_handler(Datapath_leave_event::static_get_name(), boost::bind(&rfproxy::on_datapath_down, this, _1));
    register_handler(Ofp_msg_event::get_name(OFPT_PORT_STATUS), boost::bind(&rfproxy::on_port_status_in, this, _1));
    register_handler(Ofp_msg_event::get_stats_name(OFPMP_PORT_DESC),boost::bind(&rfproxy::on_port_description_in, this, _1));
    register_handler(Ofp_msg_event::get_name(OFPT_PACKET_IN), boost::bind(&rfproxy::on_packet_in, this, _1));
}

void rfproxy::getInstance(const Context* c, rfproxy*& component) {
    component = dynamic_cast<rfproxy*>
        (c->get_by_interface(container::Interface_description(
            typeid(rfproxy).name())));
}

REGISTER_COMPONENT(Simple_component_factory<rfproxy>, rfproxy);
}
