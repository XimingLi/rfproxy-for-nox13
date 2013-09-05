#include "MongoIPC.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "vlog.hh"

namespace vigil {

static Vlog_module lg("MongoIPC");
}

MongoIPCMessageService::MongoIPCMessageService(const string &address, const string db, const string id) {
    vigil::lg.dbg("Hi! In MongoIPC.c constructor\n");
    this->set_id(id);
    vigil::lg.dbg("id = %s\n",id.c_str());
    this->db = db;
    vigil::lg.dbg("db = %s\n", db.c_str());
    this->address = address;
    vigil::lg.dbg("address = %s\n", address.c_str());
    this->connect(producerConnection, this->address);
    vigil::lg.dbg("End of this constructor\n");
}

void MongoIPCMessageService::createChannel(mongo::DBClientConnection &con, const string &ns) {
    con.createCollection(ns, CC_SIZE, true);
    con.ensureIndex(ns, BSON("_id" << 1));
    con.ensureIndex(ns, BSON(TO_FIELD << 1));
}

void MongoIPCMessageService::connect(mongo::DBClientConnection &connection, const string &address) {
    try {
	vigil::lg.dbg("Hi! In MongoIPCMessageService::connect ##################################\n");
        vigil::lg.dbg("address = %s\n", address.c_str());

	//string addr = "192.169.1.1";
	//int port = 27017;
	//char szDest[128]={0};
        //sprintf(szDest,"%s:%d",addr.c_str(),port);
	string err;
        if(connection.connect(address, err)) 
		vigil::lg.dbg("connect to MongoDB address = %s, Successful!\n", address.c_str());
	else
		vigil::lg.dbg("connect to MongoDB address = %s, Failed!\n", address.c_str());
	//mongo::DBClientConnection aaa;
	//vigil::lg.dbg("In MongoIPCMessageService::connect why????????????????????????????????\n");
        //aaa.connect(szDest);

//try
/*
Cmongodb cm;
	string errmsg;
	string hash = "1223";
	cm.Init("127.0.0.1", 27017, "test.hash");
vigil::lg.dbg("fuck!!!!!!!!1");
    if(!cm.ConnectServer(errmsg))
	{
        vigil::lg.dbg("connect mongodb server error.\n");
        //return -1;
    }
else vigil::lg.dbg("connect mongodb server succ.\n");

    if(cm.QueryTarget(hash, errmsg))
	{
        cout << "hit get table " << hash  << endl;
	}
	else
	{
		cm.InsertTarget(hash, errmsg);
		cout << "not found, insert " << hash << endl;
	}
*/
//try end


	vigil::lg.dbg("Hi! End of MongoIPCMessageService::connect\n");
    }
    catch(mongo::DBException &e) {
	vigil::lg.dbg("ERROR! In MongoIPCMessageService::connect\n");
        cout << "Exception: " << e.what() << endl;
        exit(1);
    }
}
void MongoIPCMessageService::listenWorker(const string &channelId, IPCMessageFactory *factory, IPCMessageProcessor *processor) {
    string ns = this->db + "." + channelId;

    mongo::DBClientConnection connection;
    this->connect(connection, this->address);

    this->createChannel(connection, ns);
    mongo::Query query = QUERY(TO_FIELD << this->get_id() << READ_FIELD << false).sort("$natural");
    while (true) {
        auto_ptr<mongo::DBClientCursor> cur = connection.query(ns, query,
                                                               PENDINGLIMIT);
        while (cur->more()) {
            mongo::BSONObj envelope = cur->nextSafe();
            IPCMessage *msg = takeFromEnvelope(envelope, factory);
            processor->process(envelope["from"].String(), this->get_id(), channelId, *msg);
            delete msg;

            connection.update(ns,
                QUERY("_id" << envelope["_id"]), 
                BSON("$set" << BSON(READ_FIELD << true)),
                false, true);
        }
        usleep(50000); // 50ms
    }
}

void MongoIPCMessageService::listen(const string &channelId, IPCMessageFactory *factory, IPCMessageProcessor *processor, bool block) {
    boost::thread t(&MongoIPCMessageService::listenWorker, this, channelId, factory, processor);
    if (block)
        t.join();
    else
        t.detach();
}

bool MongoIPCMessageService::send(const string &channelId, const string &to, IPCMessage& msg) {
    boost::lock_guard<boost::mutex> lock(ipcMutex);
    string ns = this->db + "." + channelId;
    //vigil::lg.dbg("In MongoIPCMessageService::send, send or not?\n");
    //vigil::lg.dbg("ns = %s", ns.c_str());
    this->createChannel(producerConnection, ns);
    this->producerConnection.insert(ns, putInEnvelope(this->get_id(), to, msg));

    return true;
}

mongo::BSONObj putInEnvelope(const string &from, const string &to, IPCMessage &msg) {
    mongo::BSONObjBuilder envelope;

    envelope.genOID();
    envelope.append(FROM_FIELD, from);
    envelope.append(TO_FIELD, to);
    envelope.append(TYPE_FIELD, msg.get_type());
    envelope.append(READ_FIELD, false);

    const char* data = msg.to_BSON();
    envelope.append(CONTENT_FIELD, mongo::BSONObj(data));
    delete data;

    return envelope.obj();
}

IPCMessage* takeFromEnvelope(mongo::BSONObj envelope, IPCMessageFactory *factory) {
   IPCMessage* msg = factory->buildForType(envelope[TYPE_FIELD].Int());
   msg->from_BSON(envelope[CONTENT_FIELD].Obj().objdata());
   return msg;
}
