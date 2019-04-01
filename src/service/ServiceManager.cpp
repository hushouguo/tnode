/*
 * \file: ServiceManager.cpp
 * \brief: Created by hushouguo at 16:36:03 Mar 26 2019
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	bool ServiceManager::newservice(const char* entryfile) {
		//TODO: multi-threads reentrance
		Service* service = new Service(this->_init_serviceid++);
		bool result = service->init(entryfile);
		if (!result) {
			SafeDelete(service);
			return false;
		}
		return this->_services.insert(std::make_pair(service->id, service)).second;
	}

	void ServiceManager::stop() {
		for (auto& i : this->_services) {
			SafeDelete(i.second);
		}
		this->_services.clear();
	}

	void ServiceManager::schedule() {
		for (auto& i : this->_services) {
			Service* service = i.second;
			if (!service->msgQueueEmpty() && !service->inQueue()) {
				service->intoQueue();
				sThreadPool.add([](Service* service) {
					service->run();
					service->exitQueue();
				}, service);
			}
		}
	}
	
	INITIALIZE_INSTANCE(ServiceManager);
}
