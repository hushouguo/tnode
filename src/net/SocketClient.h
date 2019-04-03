/*
 * \file: SocketClient.h
 * \brief: Created by hushouguo at 01:56:02 Aug 09 2018
 */
 
#ifndef __SOCKET_CLIENT_H__
#define __SOCKET_CLIENT_H__

BEGIN_NAMESPACE_TNODE {
	class SocketClient : public Socket {
		public:
			virtual ~SocketClient() = 0;

		public:
			virtual bool connect(const char* address, int port) = 0;
	};

	struct SocketClientCreator {
		static SocketClient* create(u32 owner);
	};
}

#endif
