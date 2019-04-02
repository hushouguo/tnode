/*
 * \file: MessageQueue.h
 * \brief: Created by hushouguo at 17:34:01 Apr 02 2019
 */
 
#ifndef __MESSAGEQUEUE_H__
#define __MESSAGEQUEUE_H__

BEGIN_NAMESPACE_TNODE {
	class MessageQueue {
		public:
			bool empty();
			size_t size();
			void addMessage(Servicemessage*);
			Servicemessage* getMessage();
			Servicemessage* allocateMessage();
			void releaseMessage(Servicemessage*);

		public:
			void stop();
			void dispatch();

		private:
			LockfreeQueue<Servicemessage*> _msgQueue;
	};
}

#define sMessageQueue tnode::Singleton<tnode::MessageQueue>::getInstance()

#endif
