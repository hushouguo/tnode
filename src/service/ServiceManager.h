/*
 * \file: ServiceManager.h
 * \brief: Created by hushouguo at 15:23:05 Mar 25 2019
 */
 
#ifndef __SERVICEMANAGER_H__
#define __SERVICEMANAGER_H__

BEGIN_NAMESPACE_TNODE {
	class ServiceManager {
		public:
			bool newservice(const char* entryfile);
			Service* getService(u32 id);
			void stop();
			void schedule();
			void schedule(Service* service);
			void pushMessage(u32 sid, const Servicemessage* msg);
			
		private:
			int _init_serviceid = 0;
			std::unordered_map<u32, Service*> _services;
	};
}

#define sServiceManager tnode::Singleton<tnode::ServiceManager>::getInstance()

#endif
