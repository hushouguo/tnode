/*
 * \file: Socket.h
 * \brief: Created by hushouguo at 16:29:33 Jun 21 2018
 */
 
#ifndef __SOCKET_H__
#define __SOCKET_H__

BEGIN_NAMESPACE_TNODE {
	enum {
		SOCKET_SERVER, SOCKET_CLIENT, SOCKET_CONNECTION
	};
	class Socket {
		public:
			virtual ~Socket() = 0;

		public:
			virtual SOCKET fd() = 0;
			virtual int socket_type() = 0;
			virtual void socket_type(int) = 0;
			virtual u32 owner() = 0;
			
		public:
			virtual bool receive() = 0;
			virtual bool send(const void*, size_t) = 0;
			virtual bool send() = 0;
			virtual ByteBuffer& recvBuffer() = 0;
	};

	struct SocketCreator {
		static Socket* create(SOCKET s, u32 owner);
	};
}

#endif
