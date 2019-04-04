/*
 * \file: Socket.cpp
 * \brief: Created by hushouguo at 16:29:36 Jun 21 2018
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	class SocketInternal : public Socket {
		public:
			SocketInternal(SOCKET s, u32 owner);
			~SocketInternal();
			
		public:
			SOCKET fd() override { return this->_fd; }
			int socket_type() override { return this->_socket_type; }
			void socket_type(int value) override { this->_socket_type = value; }
			u32 owner() override { return this->_owner; }
						
		public:
			bool receive() override;
			bool send(const void*, size_t) override;
			bool send() override;
			ByteBuffer& recvBuffer() override { return this->_rbuffer; }

		private:
			SOCKET _fd = -1;
			int _socket_type = -1;
			u32 _owner = 0;

		private:			
			ByteBuffer _rbuffer, _wbuffer;
			ssize_t readBytes(Byte*, size_t);
			ssize_t sendBytes(const Byte*, size_t);
	};

	SocketInternal::SocketInternal(SOCKET s, u32 owner) {
		this->_fd = s;
		this->_socket_type = SOCKET_CONNECTION;
		this->_owner = owner;
	}

	Socket::~Socket() {}
	SocketInternal::~SocketInternal() {
		SafeClose(this->_fd);
	}

	bool SocketInternal::receive() {
		size_t readlen = 960;
		ssize_t bytes = this->readBytes(this->_rbuffer.wbuffer(readlen), readlen);
		CHECK_RETURN(bytes >= 0, false, "readBytes error");
		if (bytes > 0) {
			this->_rbuffer.wlength(size_t(bytes));
		}
		Debug << "Socket: " << this->_fd << " receive bytes: " << bytes;
		return true;	
	}
		
	bool SocketInternal::send(const void* buffer, size_t len) {
		if (this->_wbuffer.size() == 0) {
			ssize_t bytes = this->sendBytes((const Byte*)buffer, len);
			CHECK_RETURN(bytes >= 0, false, "sendBytes error");
			if (size_t(bytes) < len) {
				this->_wbuffer.append((const Byte*)buffer + bytes, len - bytes);
			}
			return true;
		}
		this->_wbuffer.append((const Byte*)buffer, len);
		return this->send();
	}	

	bool SocketInternal::send() {
		if (this->_wbuffer.size() > 0) {
			ssize_t bytes = this->sendBytes(this->_wbuffer.rbuffer(), this->_wbuffer.size());
			CHECK_RETURN(bytes >= 0, false, "sendBytes error");
			if (size_t(bytes) > 0) {
				this->_wbuffer.rlength(size_t(bytes));
			}
		}
		return true;
	}

	//
	// < 0: error
	ssize_t SocketInternal::readBytes(Byte* buffer, size_t len) {
		ssize_t bytes = 0;
		while (true) {
			ssize_t rc = TEMP_FAILURE_RETRY(::recv(this->_fd, buffer + bytes, len - bytes, MSG_DONTWAIT | MSG_NOSIGNAL));			
			if (rc == 0) {
				//Error.cout("lost Connection: %d", this->_fd);
				return -1; // lost connection
			}
			else if (rc < 0) {
				if (interrupted()) {
					continue;
				}				
				if (wouldblock()) {
					break; // no more data to read
				}				
				CHECK_RETURN(false, rc, "socket receive error: %d, %s", errno, strerror(errno));
			}
			else {
				bytes += rc;
			}			
		}
		return bytes;
	}

	//
	// < 0: error
	ssize_t SocketInternal::sendBytes(const Byte* buffer, size_t len) {
		ssize_t bytes = 0;
		while (len > size_t(bytes)) {
			ssize_t rc = TEMP_FAILURE_RETRY(::send(this->_fd, buffer + bytes, len - bytes, MSG_DONTWAIT | MSG_NOSIGNAL));
			if (rc == 0) {
				//Error.cout("lost Connection:%d", this->_fd);
				return -1; // lost connection
			}
			else if (rc < 0) {
				if (interrupted()) { 
					continue; 
				}		
				if (wouldblock()) {
					break; // socket buffer is full
				}
				CHECK_RETURN(false, rc, "socket send error: %d, %s", errno, strerror(errno));
			}
			else {
				bytes += rc;
			}			
		}
		return bytes;
	}
	
	Socket* SocketCreator::create(SOCKET s, u32 owner) {
		return new SocketInternal(s, owner);
	}
}

