/*
 * \file: Helper.cpp
 * \brief: Created by hushouguo at 20:07:30 Aug 09 2018
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {

	//
	// errno helper
	//
	
	bool interrupted() {
		return errno == EINTR;
	}

	bool wouldblock() {
		return errno == EAGAIN || errno == EWOULDBLOCK;
	}

	bool connectionlost() {
		return errno == ECONNRESET || errno == ENOTCONN || errno == ESHUTDOWN || errno == ECONNABORTED || errno == EPIPE;
	}


	//
	// network utilities
	//	
	
	bool reuseableAddress(SOCKET s) {
		int one = 1;
		/* REUSEADDR on Unix means, "don't hang on to this address after the
		 * listener is closed."  On Windows, though, it means "don't keep other
		 * processes from binding to this address while we're using it. 
		 */
		return setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void*)&one, sizeof(one)) == 0;		
	}

	bool reuseablePort(SOCKET s) {
		int val = 1;
		return setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val)) == 0;
	}

	bool blocking(SOCKET s) {
		return fcntl(s, F_SETFL, fcntl(s, F_GETFL) & ~O_NONBLOCK) != -1;
	}

	bool nonblocking(SOCKET s) {
		return fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK) != -1;
	}
	
	bool nodelay(SOCKET s, bool enable) {
		int on = enable ? 1 : 0;
		return setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (void *)&on, sizeof(on)) == 0;
	}

	bool connectSignal(SOCKET s, const char* address, int port, int seconds) {
	
		bool connectResult = true;
		
		struct sigaction act,oldact;
        act.sa_handler = [](int){};
        sigemptyset(&act.sa_mask);  
        sigaddset(&act.sa_mask, SIGALRM);  
        act.sa_flags = SA_INTERRUPT; //The system call that is interrupted by this signal will not be restarted automatically
        sigaction(SIGALRM, &act, &oldact);

		alarm(seconds);
		
		struct sockaddr_in sockaddr;
		memset(&sockaddr, 0, sizeof(sockaddr));
		sockaddr.sin_family = AF_INET;
		/*sockaddr.sin_addr.s_addr = inet_addr(ip);*/
		inet_aton(address, &(sockaddr.sin_addr));
		sockaddr.sin_port = htons(port);

		blocking(s);
		if (::connect(s, (struct sockaddr*)&sockaddr, sizeof(struct sockaddr_in)) < 0) {
			if (errno == EINTR) {
				Error.cout("connectSignal timeout");
			}
			else {
				Error.cout("connectSignal error: %d, %s", errno, strerror(errno));
			}
			connectResult = false;
		}
		nonblocking(s);

		alarm(0);
		sigaction(SIGALRM, &oldact, nullptr);

		return connectResult;
	}
	
	bool connectSelect(SOCKET s, const char* address, int port, int milliseconds) {

		struct sockaddr_in sockaddr;
		memset(&sockaddr, 0, sizeof(sockaddr));
		sockaddr.sin_family = AF_INET;
		/*sockaddr.sin_addr.s_addr = inet_addr(ip);*/
		inet_aton(address, &(sockaddr.sin_addr));
		sockaddr.sin_port = htons(port);

		nonblocking(s);
		if (::connect(s, (struct sockaddr*)&sockaddr, sizeof(struct sockaddr_in)) < 0) {
			if (errno != EINPROGRESS) {
				Error.cout("connectSelect error: %d, %s", errno, strerror(errno));
				return false;
			}
			
			fd_set fdread;
			fd_set fdwrite;
			struct timeval tv;

			FD_ZERO(&fdread);
			FD_ZERO(&fdwrite);
			FD_SET(s, &fdread);
			FD_SET(s, &fdwrite);

			tv.tv_sec = milliseconds / 1000;
			tv.tv_usec = (milliseconds % 1000) * 1000;

			int rc = ::select(s + 1, &fdread, &fdwrite, nullptr, &tv);

			if (rc < 0) {
				Error.cout("connectSelect error: %d, %s", errno, strerror(errno));
				return false;
			}
			else if (rc == 0) {
				Error.cout("connectSelect timeout");
			}
			else {
				if (FD_ISSET(s, &fdwrite) || FD_ISSET(s, &fdread)) {
					int error = 0;
					socklen_t len = sizeof(error);
					int rc = getsockopt(s, SOL_SOCKET, SO_ERROR, &error, &len);
					if (rc != 0) {
						Error.cout("connectSelect failure: %d, %s", errno, strerror(errno));
						return false;
					}

					if (error != 0) {
						Error.cout("connectSelect failure: %d, %s", errno, strerror(errno));
						return false;
					}

					nonblocking(s);
					return true; // connect success
				}
				else {
					Error.cout("connectSelect failure");
				}
			}
		}

		return true;
	}

	// Socketmessage
	//
	Socketmessage* allocateMessage(SOCKET s, u8 opcode) {
		Socketmessage* msg = (Socketmessage*) ::malloc(sizeof(Socketmessage));
		msg->magic = MAGIC;
		msg->s = s;
		msg->opcode = opcode;
		msg->payload_len = 0;
		return msg;
	}

	Socketmessage* allocateMessage(SOCKET s, u8 opcode, size_t payload_len) {
		Socketmessage* msg = (Socketmessage*) ::malloc(sizeof(Socketmessage) + payload_len);
		msg->magic = MAGIC;
		msg->s = s;
		msg->opcode = opcode;
		msg->payload_len = payload_len;
		return msg;
	}

	Socketmessage* allocateMessage(SOCKET s, u8 opcode, const void* payload, size_t payload_len) {
		Socketmessage* msg = (Socketmessage*) ::malloc(sizeof(Socketmessage) + payload_len);
		msg->magic = MAGIC;
		msg->s = s;
		msg->opcode = opcode;
		msg->payload_len = payload_len;
		if (payload) {
			memcpy(msg->payload, payload, payload_len);
		}
		return msg;
	}
	
	void releaseMessage(const Socketmessage* msg) {
		assert(msg->magic == MAGIC);
		SafeFree(msg);
	}

    Socketmessage* allocateMessage(size_t payload_len) {
		return allocateMessage(-1, SM_OPCODE_MESSAGE, payload_len);
	}

    Socketmessage* allocateMessage(size_t payload_len, const void* payload) {
		return allocateMessage(-1, SM_OPCODE_MESSAGE, payload, payload_len);
	}

    const void* GET_MESSAGE_PAYLOAD(const Socketmessage* msg) {
		assert(msg);
		assert(msg->magic == MAGIC);
		return msg->payload;
	}

    size_t GET_MESSAGE_PAYLOAD_LENGTH(const Socketmessage* msg) {
		assert(msg);
		assert(msg->magic == MAGIC);
		return msg->payload_len;
	}

	const Socketmessage* GET_MESSAGE_BY_PAYLOAD(const void* payload) {
		const Socketmessage* msg = (const Socketmessage *) ((Byte*) payload - offsetof(Socketmessage, payload));
		assert(msg->magic == MAGIC);
		return msg;
	}

	SOCKET GET_MESSAGE_SOCKET(const Socketmessage* msg) {
		assert(msg);
		assert(msg->magic == MAGIC);
		return msg->s;
	}

	bool IS_ESTABLISH_MESSAGE(const Socketmessage* msg) {
		assert(msg);
		assert(msg->magic == MAGIC);
		return msg->opcode == SM_OPCODE_ESTABLISH;
	}
	
	bool IS_CLOSE_MESSAGE(const Socketmessage* msg) {
		assert(msg);
		assert(msg->magic == MAGIC);
		return msg->opcode == SM_OPCODE_CLOSE;
	}
}
