/*
 * \file: NetworkManager.h
 * \brief: Created by hushouguo at 16:54:12 Apr 02 2019
 */
 
#ifndef __NETWORKMANAGER_H__
#define __NETWORKMANAGER_H__

#define MAX_SOCKET		65536

BEGIN_NAMESPACE_TNODE {
	class NetworkManager : public Runnable {
		public:
			NetworkManager() : Runnable(0) {}
			
		public:
			void init();
			const char* getClassName() override { return "NetworkManager"; }

		public:
			bool sendMessage(const Servicemessage* message);
			void closeSocket(SOCKET s, const char* reason);
			SOCKET newserver(const char* address, int port);
			SOCKET newclient(const char* address, int port);

		public:			
			void run() override;
			void stop();

		private:
			Poll _poll;
			Socket* _sockets[MAX_SOCKET];
	};
}

#define sNetworkManager tnode::Singleton<tnode::NetworkManager>::getInstance()

#endif
