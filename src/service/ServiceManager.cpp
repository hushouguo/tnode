/*
 * \file: ServiceManager.cpp
 * \brief: Created by hushouguo at 16:36:03 Mar 26 2019
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	ServiceManager::ServiceManager() {
		memset(this->_services, 0, sizeof(this->_services));
	}

	void ServiceManager::stop() {
		for (auto& service : this->_services) {
			SafeDelete(service);
		}
	}

	bool ServiceManager::pushMessage(u32 to, const Servicemessage* msg) {
		assert(to < MAX_SERVICE);
		Service* service = this->_services[to];
		if (!service) {
			release_message(msg);
		}
		CHECK_RETURN(service, false, "Not found service: %d", to);
		service->pushMessage(msg);
		//this->schedule(service);
		return true;
	}
	
	bool ServiceManager::newservice(const char* entryfile) {
		u32 sid = this->_init_serviceid++;
		CHECK_RETURN(sid < MAX_SERVICE, false, "the number of service cannot exceed %d", MAX_SERVICE);
		assert(this->_services[sid] == nullptr);
		Service* service = new Service(sid);
		bool result = service->init(entryfile);
		if (!result) {
			SafeDelete(service);
			return false;
		}
		this->_services[sid] = service;
		return true;
	}

	void ServiceManager::schedule(Service* service) {
		if (!service->msgQueueEmpty()) {
			service->schedule();
		}
	}

	void ServiceManager::schedule() {
		for (auto service : this->_services) {
			if (service != nullptr) {
				this->schedule(service);
			}
		}
	}
	
	INITIALIZE_INSTANCE(ServiceManager);
}
