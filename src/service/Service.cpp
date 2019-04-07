/*
 * \file: Service.cpp
 * \brief: Created by hushouguo at 12:46:57 Mar 26 2019
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	Service::Service(u32 id) : Entry<u32>(id) {
	}

	Service::Service(const char* name) : Entry<u32>(hashString(name)) {
	}

	bool Service::init(const char* entryfile) {
		SafeDelete(this->_messageParser);
		const char* proto_dir = sConfig.get("tnode.protocol", (const char*) nullptr);
		CHECK_RETURN(proto_dir, false, "lack `tnode.protocol`");
		this->_messageParser = MessageParserCreator::create();
		bool retval = this->_messageParser->load(proto_dir);
		CHECK_RETURN(retval, false, "load protocol: %s error", proto_dir);

		this->_L = luaT_newstate();
		luaT_setOwner(this->_L, this->id);
		this->_entryfile = entryfile;
		/* run entry script file */
		if (!luaT_execFile(this->_L, this->_entryfile.c_str())) { return false; }
		
		return true;
	}

	void Service::stop() {
		if (!this->_isstop) {
			this->_isstop = true;
			SafeDelete(this->_messageParser);
			luaT_close(this->_L);
		}
	}

	void Service::run() {
		for (const Servicemessage* msg = this->getMessage(); msg; msg = this->getMessage()) {				
			{ // delivery msg to lua
				//int ref = i->second;
				//luaT_getRegistry(this->_L, ref);
				//luaT_callFunction(this->_L, msg->fd, msg->rawmsg.entityid, msg->rawmsg.msgid, (const void*)msg);
			}
		}
	}

	u32 Service::dispatch(const Servicemessage* msg) {
		//TODO:
		return 0;
	}

	void Service::pushMessage(const Servicemessage* msg) {
		this->_msgQueue.push_back(msg);
	}
	
	const Servicemessage* Service::getMessage() {
		return this->_msgQueue.empty() ? nullptr : this->_msgQueue.pop_front();
	}
}

