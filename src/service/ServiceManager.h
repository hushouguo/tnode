/*
 * \file: ServiceManager.h
 * \brief: Created by hushouguo at 15:23:05 Mar 25 2019
 */
 
#ifndef __SERVICEMANAGER_H__
#define __SERVICEMANAGER_H__

#define MAX_SERVICE		512

BEGIN_NAMESPACE_TNODE {
	class ServiceManager {
		public:
			ServiceManager();
			
			// multi-thread exclusion
		public:
			bool pushMessage(u32 to, const Servicemessage* msg);
		
			bool newservice(const char* entryfile);
			inline Service* getService(u32 sid) {
				return sid < MAX_SERVICE ? this->_services[sid] : nullptr;
			}
			void stop();
			void schedule();
			void schedule(Service* service);
			
		private:
			int _init_serviceid = 0;
			Service* _services[MAX_SERVICE];
	};
}

#define sServiceManager tnode::Singleton<tnode::ServiceManager>::getInstance()

#endif
