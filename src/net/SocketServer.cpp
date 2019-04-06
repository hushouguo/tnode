/*
 * \file: SocketServer.cpp
 * \brief: Created by hushouguo at 01:55:44 Aug 09 2018
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {	
	class SocketServerInternal : public SocketServer {
		public:
			SocketServerInternal();
			~SocketServerInternal();

		public:
			SOCKET fd() override { return this->_socket->fd(); }
			int socket_type() override { return this->_socket->socket_type(); }
			void socket_type(int value) override { this->_socket->socket_type(value); }			
			
		public:
			bool receive() override { return this->_socket->receive(); }
			bool send(const void* buffer, size_t len) override { return this->_socket->send(buffer, len); }
			bool send() override { return this->_socket->send(); }
			ByteBuffer& getBuffer() override { return this->_socket->getBuffer(); }

		public:
			bool listen(const char* address, int port) override;
			SOCKET accept() override;

		private:
			Socket* _socket = nullptr;
	};

	SocketServerInternal::SocketServerInternal() {
		this->_socket = SocketCreator::create(::socket(AF_INET, SOCK_STREAM, 0));
		assert(this->_socket);
		this->_socket->socket_type(SOCKET_SERVER);		
	}

	SocketServer::~SocketServer() {}
	SocketServerInternal::~SocketServerInternal() {
		SafeDelete(this->_socket);
	}

	SocketServer* SocketServerCreator::create() {
		return new SocketServerInternal();
	}

	bool SocketServerInternal::listen(const char* address, int port) {
		CHECK_RETURN(this->fd() >= 0, false, "create socket failure: %d, %s", errno, strerror(errno));
		bool rc = reuseableAddress(this->fd());
		CHECK_RETURN(rc, false, "reuseableAddress failure: %d, %s", errno, strerror(errno));
		rc = nonblocking(this->fd());
		CHECK_RETURN(rc, false, "nonblocking failure: %d, %s", errno, strerror(errno));

		struct sockaddr_in sockaddr;
		memset(&sockaddr, 0, sizeof(sockaddr));
		sockaddr.sin_family = AF_INET;
		inet_aton(address, &(sockaddr.sin_addr));
		sockaddr.sin_port = htons(port);

		int val = ::bind(this->fd(), (struct sockaddr*)&sockaddr, sizeof(sockaddr));
		CHECK_RETURN(val == 0, false, "bind failure: %d, %s", errno, strerror(errno));

		val = ::listen(this->fd(), SOMAXCONN);
		CHECK_RETURN(val == 0, false, "listen failure: %d, %s", errno, strerror(errno));

		Debug << "listen on: " << address << ":" << port;
		return true;
	}
	
	SOCKET SocketServerInternal::accept() {
		SOCKET newfd = -1;
		while (true) {
			struct sockaddr_in addr;
			socklen_t len = sizeof(addr);
			newfd = ::accept(this->fd(), (struct sockaddr*)&addr, &len);
			if (newfd < 0) {
				if (interrupted()) {
					continue;
				}
				if (wouldblock()) {
					break; // no more connection
				}
				CHECK_RETURN(false, -1, "accept error:%d,%s", errno, strerror(errno));
			}
			return newfd;
		}
		return newfd;
	}
}
