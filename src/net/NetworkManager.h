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
			NetworkManager();

			// multi-thread exclusion
		public:
			bool sendString(SOCKET s, u64 entityid, u32 msgid, std::string& outstring);
			void closeSocket(SOCKET s, const char* reason);
			SOCKET newserver(const char* address, int port);
			SOCKET newclient(const char* address, int port);

		public:			
			void run() override;
			void stop();

		private:
			Poll _poll;
			Socket* _sockets[MAX_SOCKET];
			LockfreeQueue<Servicemessage*> _sendQueue;
	};
}

#define sNetworkManager tnode::Singleton<tnode::NetworkManager>::getInstance()

#endif
