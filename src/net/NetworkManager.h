/*
 * \file: NetworkManager.h
 * \brief: Created by hushouguo at 16:54:12 Apr 02 2019
 */
 
#ifndef __NETWORKMANAGER_H__
#define __NETWORKMANAGER_H__

BEGIN_NAMESPACE_TNODE {
	class NetworkManager {
		public:
			bool init();
			void stop();

		public:
			SocketServer* newserver(Service* service, const char* address, int port);
			SocketClient* newclient(Service* service, const char* address, int port);
			void close(Service* service, SOCKET s);

		public:
			void run();

		private:
			bool _isstop = false;
			Poll _poll;
			Spinlocker _locker;
			std::thread* _netthread = nullptr;
			std::unordered_map<SOCKET, Socket*> _sockets;
	};
}

#define sNetworkManager tnode::Singleton<tnode::NetworkManager>::getInstance()

#endif
