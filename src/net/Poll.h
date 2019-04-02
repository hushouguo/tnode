/*
 * \file: Poll.h
 * \brief: Created by hushouguo at 14:39:25 Jun 28 2018
 */
 
#ifndef __POLL_H__
#define __POLL_H__

#define NM_POLL_EVENT		128

BEGIN_NAMESPACE_TNODE {
	class Socket;
	class Poll {
		public:
			Poll();
			~Poll();

		public:
			void shutdown();
			void run(int milliseconds);
			
		public:
			bool addSocket(Socket* s);
			bool removeSocket(Socket* s);
			bool setSocketPollout(Socket* s, bool value);

		private:
			int _epfd = -1;
			struct epoll_event _events[NM_POLL_EVENT];
	};
}

#endif
