/*
 * \file: Socket.h
 * \brief: Created by hushouguo at 16:29:33 Jun 21 2018
 */
 
#ifndef __SOCKET_H__
#define __SOCKET_H__

#define COUNT_MESSAGE_SPAN	60

BEGIN_NAMESPACE_TNODE {
	class Service;
	class Socket {
		public:						
			virtual ~Socket() = 0;

		public:
			virtual SOCKET fd() = 0;
			virtual void close() = 0;
			
		public:
			virtual const Socketmessage* receiveMessage() = 0;
			virtual void sendMessage(const void*, size_t) = 0;
			virtual void sendMessage(const Socketmessage*) = 0;
		
			virtual bool receiveMessage(Socketmessage*& msg) = 0;
			virtual bool sendMessage() = 0;
			virtual bool sendMessage(const Socketmessage* msg) = 0;
	};
	
	class Socket {
		public:
			Socket(SOCKET connfd, Service* service) : _fd(connfd) {
				nonblocking(this->fd());
				this->_service = service;
				memset(this->_countMessage, 0, sizeof(this->_countMessage));
			}

			~Socket() {
				this->close();
				for (auto& msg : this->_sendlist) {
					bundle::releaseMessage(msg);
				}
				this->_sendlist.clear();
			}
			
		public:
			inline SOCKET fd() { return this->_fd; }
			inline bool is_listening() { return this->_is_listening; }
			inline void set_listening(bool value) { this->_is_listening = value; }
			void close();
			inline u64 lastSecond() { return this->_lastSecond; }
			inline u32 totalMessage() { return this->_totalMessage; }
			u32 recentMessage(u32 seconds);

		public:
			bool receiveMessage(Socketmessage*& msg);
			bool sendMessage();
			bool sendMessage(const Socketmessage* msg);
			
		private:
			SOCKET _fd = -1;
			bool _is_listening = false;
			ByteBuffer _rbuffer, _wbuffer;
			std::list<const Socketmessage*> _sendlist;
			Service* _service = nullptr;
			bool splitMessage(Socketmessage*& msg);
			
			u64 _lastSecond = currentSecond();
			u32 _countMessage[COUNT_MESSAGE_SPAN];
			u32 _totalMessage = 0;
			void receivedMessage();
	};	
}

#endif
