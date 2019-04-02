/*
 * \file: MessageQueue.cpp
 * \brief: Created by hushouguo at 17:34:13 Apr 02 2019
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	bool MessageQueue::empty() {
		return this->_msgQueue.empty();
	}

	size_t MessageQueue::size() {
		return this->_msgQueue.size();
	}
	
	void MessageQueue::addMessage(Servicemessage* msg) {
		this->_msgQueue.push_back(msg);
	}
	
	Servicemessage* MessageQueue::getMessage() {
		return this->empty() ? nullptr : this->_msgQueue.pop_front();
	}
	
	Servicemessage* MessageQueue::allocateMessage() {
		Servicemessage* msg = (Servicemessage*) malloc(1024);
		msg->source = msg->rawmsg.len = 0;	// reset readed and length of message
		return msg;
	}
	
	void MessageQueue::releaseMessage(Servicemessage* msg) {
		SafeFree(msg);
	}
	
	void MessageQueue::stop() {
		Debug << "remains " << this->size() << " messages in the Queue";
		for (Servicemessage* msg = this->getMessage(); msg; msg = this->getMessage()) {
			this->releaseMessage();
		}
	}

	void MessageQueue::dispatch() {
		for (Servicemessage* msg = this->getMessage(); msg; msg = this->getMessage()) {
			Service* service = sServiceManager.getService(msg->to);
			service->pushMessage(msg);
		}		
	}

	INITIALIZE_INSTANCE(MessageQueue);
}

