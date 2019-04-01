/*
 * \file: SocketServer.h
 * \brief: Created by hushouguo at 23:00:13 Aug 08 2018
 */
 
#ifndef __SOCKET_SERVER_H__
#define __SOCKET_SERVER_H__

#define	BUNDLE_SOL_MAXSIZE				1
#define BUNDLE_SOL_SILENCE_SECOND		2
#define BUNDLE_SOL_THRESHOLD_MESSAGE	3
#define BUNDLE_SOL_THRESHOLD_INTERVAL	4
#define BUNDLE_SOL_MAX					128

BEGIN_NAMESPACE_TNODE {
	class Service;
	class SocketServer {
		public:
			virtual ~SocketServer() = 0;

		public:
			virtual SOCKET fd() = 0;
			virtual bool setWorkerNumber(u32) = 0;
			virtual bool start(const char* address, int port) = 0;
			virtual void stop() = 0;
			virtual const Socketmessage* receiveMessage() = 0;
			virtual void sendMessage(SOCKET s, const void*, size_t) = 0;
			virtual void close(SOCKET s) = 0;
	};

	struct SocketServerCreator {
		static SocketServer* create(MESSAGE_SPLITER splitMessage, Service* service);
	};
}

#endif
