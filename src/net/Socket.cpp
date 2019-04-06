/*
 * \file: Socket.cpp
 * \brief: Created by hushouguo at 16:29:36 Jun 21 2018
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	class SocketInternal : public Socket {
		public:
			SocketInternal(SOCKET s);
			~SocketInternal();
			
		public:
			SOCKET fd() override { return this->_fd; }
			int socket_type() override { return this->_socket_type; }
			void socket_type(int value) override { this->_socket_type = value; }
						
		public:
			bool receive() override;
			bool send(const Servicemessage* message) override;
			bool send() override;
			ByteBuffer& getBuffer() override { return this->_rbuffer; }

		private:
			SOCKET _fd = -1;
			int _socket_type = -1;
			std::atomic_flag _slocker = ATOMIC_FLAG_INIT;
			LockfreeQueue<const Servicemessage*> _msglist;

		private:			
			ByteBuffer _rbuffer, _wbuffer;
			ssize_t readBytes(Byte*, size_t);
			ssize_t sendBytes(const Byte*, size_t);
	};

	SocketInternal::SocketInternal(SOCKET s) {
		this->_fd = s;
		this->_socket_type = SOCKET_CONNECTION;
	}

	Socket::~Socket() {}
	SocketInternal::~SocketInternal() {
		SafeClose(this->_fd);
		while (!this->_msglist.empty()) {
			const Servicemessage* message = this->_msglist.pop_front();
			release_message(message);
		}
	}

	bool SocketInternal::receive() {
		size_t readlen = 960;
		ssize_t bytes = this->readBytes(this->_rbuffer.wbuffer(readlen), readlen);
		CHECK_RETURN(bytes >= 0, false, "readBytes error");
		if (bytes > 0) {
			this->_rbuffer.wlength(size_t(bytes));
		}
		//Debug << "Socket: " << this->_fd << " receive bytes: " << bytes;
		return true;	
	}
		
	bool SocketInternal::send(const Servicemessage* message) {
		this->_msglist.push_back(message);
		return this->send();
	}	

	bool SocketInternal::send() {
		if (!this->_slocker.test_and_set()) {	// set OK, return false
			while (true) {
				if (this->_wbuffer.size() > 0) {
					ssize_t bytes = this->sendBytes(this->_wbuffer.rbuffer(), this->_wbuffer.size());
					CHECK_RETURN(bytes >= 0, false, "sendBytes error");
					if (size_t(bytes) > 0) {
						this->_wbuffer.rlength(size_t(bytes));
					}
				}
			
				if (this->_wbuffer.size() > 0) {
					return true;	// wbuffer did not send all
				}
			
				while (!this->_msglist.empty()) {
					const Servicemessage* message = this->_msglist.pop_front();
					ssize_t bytes = this->sendBytes((const Byte*) &message->rawmsg, message->rawmsg.len);
					if (bytes < 0) {
						release_message(message);
						CHECK_RETURN(bytes >= 0, false, "sendBytes error");
					}

					if (size_t(bytes) < message->rawmsg.len) {
						this->_wbuffer.append((const Byte*) (&message->rawmsg) + bytes, message->rawmsg.len - bytes);
					}

					release_message(message);
					
					if (this->_wbuffer.size() > 0) {
						return true;	// send wouldblock
					}
				}
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
	
	Socket* SocketCreator::create(SOCKET s) {
		return new SocketInternal(s);
	}
}

