/*
 * \file: Service.cpp
 * \brief: Created by hushouguo at 12:46:57 Mar 26 2019
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	Service::Service(int id) : Entry<int>(id) {
	}

	Service::Service(const char* name) : Entry<int>(hashString(name)) {
	}

	bool Service::init(const char* entryfile) {
		SafeDelete(this->_luaState);
		this->_luaState = new LuaState(this);
		this->_entryfile = entryfile;
		/* run entry script file */
		if (!this->_luaState->executeFile(this->_entryfile.c_str())) { return false; }	
		return true;
	}

	void Service::stop() {
		if (!this->_isstop) {
			this->_isstop = true;
			SafeDelete(this->_luaState);
		}
	}

	void Service::run() {
		while (!this->_isstop) {
			for (Servicemessage* msg = this->getMessage(); msg; msg = this->getMessage()) {
				// delivery msg to lua
			}
#if 0		
			int ref = this->_funcs.front();
			this->_funcs.pop_front();
			Debug << "call function: " << ref;
			this->_luaState->callFunction(ref);
#endif			
		}
	}

	void Service::pushMessage(Servicemessage* newmsg) {
		this->_msgQueue.push_back(newmsg);
	}
	
	Servicemessage* Service::getMessage() {
		return this->_msgQueue.empty() ? nullptr : this->_msgQueue.pop_front();
	}
}

