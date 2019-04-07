/*
 * \file: ServiceManager.cpp
 * \brief: Created by hushouguo at 16:36:03 Mar 26 2019
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	bool ServiceManager::init(const char* entryfile) {
		Service* service = this->newservice(entryfile);
		CHECK_RETURN(service, false, "ServiceManager init failure");
		this->_initid = service->id;
		return true;
	}
	
	void ServiceManager::stop() {
		while (!this->_services.empty()) {
			Service* service = this->_services.pop_front();
			SafeDelete(service);
		}
	}

	bool ServiceManager::pushMessage(const Servicemessage* msg) {
		u32 sid = 0;
		Service* initservice = this->getService(this->_initid);
		CHECK_GOTO(initservice, exit_failure, "Not found initservice: %d", this->_initid);

		sid = initservice->dispatch(msg->rawmsg.entityid, msg->rawmsg.msgid);
		CHECK_GOTO(sid != ILLEGAL_SERVICE, exit_failure, "initservice: %s call `dispatch` error", initservice->entryfile.c_str());
		
		if (true) {
			Service* service = this->getService(sid);
			CHECK_GOTO(service, exit_failure, "Not found dispatch service: %d", sid);
			service->pushMessage(msg);
			//this->schedule(service);
		}

		return true;
		
exit_failure:	
		release_message(msg);
		return false;
	}
	
	Service* ServiceManager::newservice(const char* entryfile) {
		u32 sid = this->_autoid++;
		assert(this->getService(sid) == nullptr);
		Service* service = new Service(sid);
		this->_services.insert(sid, service);
		bool result = service->init(entryfile);
		if (!result) {
			this->_services.eraseKey(sid);
			SafeDelete(service);
		}
		return service;
	}

	void ServiceManager::schedule(Service* service) {
		if (!service->msgQueueEmpty()) {
			service->schedule();
		}
	}

	void ServiceManager::schedule() {
		std::vector<Service*> v;
		this->_services.traverse([&v](u32 sid, Service* service) {
			v.push_back(service);
		});
		for (auto& service : v) {
			this->schedule(service);
		}
	}
	
	INITIALIZE_INSTANCE(ServiceManager);
}
