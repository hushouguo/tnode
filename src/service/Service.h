/*
 * \file: Service.h
 * \brief: Created by hushouguo at 14:51:12 Mar 25 2019
 */
 
#ifndef __SERVICE_H__
#define __SERVICE_H__

BEGIN_NAMESPACE_TNODE {
	class Service : public Entry<u32>, public Runnable {
		public:
			Service(u32 id);
			Service(const char* name);
			const char* getClassName() override { return "Service"; }

		public:
			inline lua_State* luaState() { return this->_L; }
			inline MessageParser* messageParser() { return this->_messageParser; }

		public:
			bool init(const char* entryfile);
			void stop();
			void run() override;

		private:
			bool _isstop = false;
			std::string _entryfile;
			lua_State* _L = nullptr;
			MessageParser* _messageParser = nullptr;
		
		public:
			u32 dispatch(const Servicemessage* msg);
			//TODO: bool msgParser(const Servicemessage* msg);
			void pushMessage(const Servicemessage* msg);
			inline bool msgQueueEmpty() { return this->_msgQueue.empty(); }
			
		private:
			LockfreeQueue<const Servicemessage*> _msgQueue;
			const Servicemessage* getMessage();
	};
}

#endif
