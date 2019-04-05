/*
 * \file: SocketServer.h
 * \brief: Created by hushouguo at 23:00:13 Aug 08 2018
 */
 
#ifndef __SOCKET_SERVER_H__
#define __SOCKET_SERVER_H__

BEGIN_NAMESPACE_TNODE {
	class SocketServer : public Socket {
		public:
			virtual ~SocketServer() = 0;

		public:
			virtual bool listen(const char* address, int port) = 0;
			virtual SOCKET accept() = 0;
	};

	struct SocketServerCreator {
		static SocketServer* create();
	};
}

#endif
