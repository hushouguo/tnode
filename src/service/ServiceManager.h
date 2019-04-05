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
			bool pushMessage(const Servicemessage* msg);		
			bool newservice(const char* entryfile);
			inline Service* getService(u32 sid) {
				return sid < MAX_SERVICE ? this->_services[sid] : nullptr;
			}
			void stop();
			void schedule();
			void schedule(Service* service);
			
		private:
			int _initid = 0;
			Service* _services[MAX_SERVICE];

		public:
			void entityfunc(u64 entityid, u32 service, int ref);
			void msgfunc(u32 msgid, u32 service, int ref);
			
		private:
			LockfreeMap<u64, u64> _entityfuncs;
			LockfreeMap<u32, u64> _msgfuncs;
	};
}

#define sServiceManager tnode::Singleton<tnode::ServiceManager>::getInstance()

#endif
