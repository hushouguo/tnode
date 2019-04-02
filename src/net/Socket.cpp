/*
 * \file: Socket.cpp
 * \brief: Created by hushouguo at 16:29:36 Jun 21 2018
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	enum {
		SOCKET_SERVER, SOCKET_CLIENT, SOCKET_CONNECTION
	};
	class Socket {
		public:
			virtual ~Socket() = 0;

		public:
			virtual SOCKET fd() = 0;
			virtual int type() = 0;
			virtual void type(int) = 0;
			virtual void* context() = 0;
			virtual void context(void*) = 0;
			
		public:
			virtual void receive() = 0;
			virtual void send() = 0;
			virtual const Servicemessage* receiveMessage() = 0;
			virtual void sendMessage(const void*, size_t) = 0;
			virtual void sendMessage(const Servicemessage*) = 0;
	};

	class SocketInternal : public Socket {
		public:
			SocketInternal(SOCKET s);
			~SocketInternal();
			
		public:
			SOCKET fd() override { return this->_fd; }
			int type() override { return this->_type; }
			void type(int value) override { this->_type = value; }
			void* context() override { return this->_context; }
			void context(void* value) override { this->_context = value; }
						
		public:
			void sendMessage() override;
			const Servicemessage* receiveMessage() override;
			void sendMessage(const void*, size_t) override;
			void sendMessage(const Servicemessage*) override;

		private:
			SOCKET _fd = -1;
			int _type = -1;
			void* _context = nullptr;
			Servicemessage* _currentMessage = nullptr;
			std::list<Servicemessage*> _recvQueue;
			ByteBuffer _sendBuffer;
			void receiveMessage();
			void acceptConnection();			
	};

	SocketInternal::SocketInternal(SOCKET s) {
		this->_fd = s;
		assert(sizeof(Servicemessage) < DEF_MESSAGE_SIZE);
	}

	SocketInternal::~SocketInternal() {
		SafeClose(this->_fd);
		//TODO:
	}

	void SocketInternal::receiveMessage() {
		while (true) {
			if (this->_currentMessage 
					&& this->_currentMessage->source >= sizeof(this->_currentMessage->rawmsg.len)
					&& this->_currentMessage->source >= this->_currentMessage->rawmsg.len) {
				assert(this->_currentMessage->source == this->_currentMessage->rawmsg.len);
				this->_recvQueue.push_back(this->_currentMessage);
				this->_currentMessage = nullptr;
			}
			
			if (!this->_currentMessage) {
				this->currentMessage = (Servicemessage *) malloc(DEF_MESSAGE_SIZE);
				memset(this->currentMessage, 0, sizeof(Servicemessage));
			}

			Byte* offset = (Byte*)(&this->_currentMessage->rawmsg) + this->_currentMessage->source;
			size_t size = 0;
			if (this->_currentMessage->source < sizeof(this->_currentMessage->rawmsg.len)) {
				size = DEF_MESSAGE_SIZE - (sizeof(Servicemessage) - sizeof(Socketmessage));
			}
			else {
				assert(this->_currentMessage->source < this->_currentMessage->rawmsg.len);
				size = this->_currentMessage->rawmsg.len - this->_currentMessage->source;
			}

			ssize_t n = TEMP_FAILURE_RETRY(
					::recv(this->_fd, offset, size, MSG_DONTWAIT | MSG_NOSIGNAL));
			
			if (n == 0) {
				//Error.cout("lost Connection: %d", this->_fd);
				return false; // lost connection
			}
			else if (n < 0) {
				if (interrupted()) {
					continue;
				}
				
				if (wouldblock()) {
					break; // no more data to read
				}
				
				CHECK_RETURN(false, false, "socket receive error: %d, %s", errno, strerror(errno));
			}
			else {
				this->_currentMessage->source += size_t(n);
			}			
		}
		return true;	
	}
	
	void SocketInternal::sendMessage() {
	}
	
	const Servicemessage* SocketInternal::receiveMessage() {
	}
	
	void SocketInternal::sendMessage(const void*, size_t) {
	}
	
	void SocketInternal::sendMessage(const Servicemessage*) {
	}
	
	struct SocketCreator {
		static Socket* create(SOCKET s);
	};





	class SocketServerInternal : public SocketServer {
		public:
			SocketServerInternal(MESSAGE_SPLITER splitMessage, Service* service);
			~SocketServerInternal();

		public:
			SOCKET fd() override {	return this->_fd_listening; }
			bool setWorkerNumber(u32) override;
			bool start(const char* address, int port) override;
			void stop() override;
			inline bool isstop() { return this->_stop; }
			const Socketmessage* receiveMessage() override;
			void sendMessage(SOCKET s, const void*, size_t) override;
			void sendMessage(SOCKET s, const Socketmessage*) override;
			void close(SOCKET s) override;
			size_t size() override;
			bool setsockopt(int opt, const void* optval, size_t optlen) override;
			bool getsockopt(int opt, void* optval, size_t optlen) override;

		private:
			u32 _workerNumber = cpus();
			LockfreeQueue<Socketmessage*> _readQueue;

			SOCKET _fd_listening = -1;
			bool _stop = true;
			size_t _opts[BUNDLE_SOL_MAX];

			MESSAGE_SPLITER _splitMessage = nullptr;
			Service* _service = nullptr;
	};
	

	class SocketServer : public Socket {
		public:
			virtual ~SocketServer() = 0;

		public:
			virtual bool listen(const char* address, int port) = 0;
	};

	struct SocketServerCreator {
		static SocketServer* create(MESSAGE_SPLITER splitMessage, Service* service);
	};
	
	class SocketClient : public Socket {
		public:
			virtual ~SocketClient() = 0;
			
		public:
			virtual bool connect(const char* address, int port, u32 timeout) = 0;
	};

	struct SocketClientCreator {
		static SocketClient* create(MESSAGE_SPLITER splitMessage, Service* service);
	};
}

BEGIN_NAMESPACE_BUNDLE {

	void Socket::receivedMessage() {
		u64 nowtime = currentSecond();
		this->_lastSecond = nowtime;
		u32 mod = nowtime % COUNT_MESSAGE_SPAN;
		this->_countMessage[mod]++;
		this->_totalMessage++;
	}

	u32 Socket::recentMessage(u32 seconds) {
		u32 total = 0;
		u64 nowtime = currentSecond();
		for (u32 n = 0; n < seconds; ++n) {
			total += this->_countMessage[(nowtime - n) % COUNT_MESSAGE_SPAN];
		}
		return total;
	}
	
	bool Socket::receiveMessage(Socketmessage*& msg) {
		msg = nullptr;
		while (true) {
			bool rc = this->splitMessage(msg);
			if (!rc) {
				return false;
			}

			if (msg) {
				this->receivedMessage();
				return true;
			}

			size_t readlen = 960;
			ssize_t n = TEMP_FAILURE_RETRY(
					::recv(this->_fd, this->_rbuffer.wbuffer(readlen), readlen, MSG_DONTWAIT | MSG_NOSIGNAL));
			
			if (n == 0) {
				//Error.cout("lost Connection: %d", this->_fd);
				return false; // lost connection
			}
			else if (n < 0) {
				if (interrupted()) {
					continue;
				}
				
				if (wouldblock()) {
					break; // no more data to read
				}
				
				CHECK_RETURN(false, false, "socket receive error: %d, %s", errno, strerror(errno));
			}
			else {
				this->_rbuffer.wlength(size_t(n));
			}			
		}
		return true;
	}

	bool Socket::sendMessage() {
		while (true) {
			const Socketmessage* msg = nullptr;
			const Byte* buffer = nullptr;
			size_t len = 0;
			if (this->_wbuffer.size() > 0) {
				buffer = this->_wbuffer.rbuffer();
				len = this->_wbuffer.size();
			}
			else if (!this->_sendlist.empty()) {
				msg = this->_sendlist.front();
				this->_sendlist.pop_front();
				buffer = msg->payload;
				len = msg->payload_len;
			}
			else { return true; }

			ssize_t sendsize = TEMP_FAILURE_RETRY(::send(this->_fd, buffer, len, MSG_DONTWAIT | MSG_NOSIGNAL));
		
			if (sendsize == 0) {
				//Error.cout("lost Connection:%d", this->_fd);
				return false; // lost connection
			}
			else if (sendsize < 0) {
				if (interrupted()) { 
					continue; 
				}
		
				if (wouldblock()) {
					break; // socket buffer is full
				}
	
				CHECK_RETURN(false, false, "send error: %d, %s", errno, strerror(errno));
			}
			else {
				if (this->_wbuffer.size() > 0) {
					this->_wbuffer.rlength(size_t(sendsize));
				}
				else {
					if (size_t(sendsize) < len) {
						assert(this->_wbuffer.size() == 0);
						this->_wbuffer.append(buffer + size_t(sendsize), len - size_t(sendsize));
					}
					assert(msg);
					bundle::releaseMessage(msg);
				}
			}
		}
		return true;	
	}
	
	bool Socket::sendMessage(const Socketmessage* msg) {
		assert(msg);
		assert(msg->magic == MAGIC);
		assert(msg->s == this->fd());
		assert(msg->payload_len > 0);
		this->_sendlist.push_back(msg);
		return this->sendMessage();
	}

	void Socket::close() {
		SafeClose(this->_fd);
	}	
}


