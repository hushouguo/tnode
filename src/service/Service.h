/*
 * \file: Service.h
 * \brief: Created by hushouguo at 14:51:12 Mar 25 2019
 */
 
#ifndef __SERVICE_H__
#define __SERVICE_H__

BEGIN_NAMESPACE_TNODE {
	class Service : public Entry<u32> {
		public:
			Service(u32 id);
			Service(const char* name);
			const char* getClassName() override { return "Service"; }

		public:
			inline LuaState* luaState() { return this->_luaState; }
			inline MessageParser* messageParser() { return this->_messageParser; }

		public:
			bool init(const char* entryfile);
			void stop();
			void run();

		private:
			bool _isstop = false;
			std::string _entryfile;
			LuaState* _luaState = nullptr;
			MessageParser* _messageParser = nullptr;

		public:
			inline void regfunction(int ref) {
				this->_regfuncs.push_back(ref);
			}

		private:
			std::list<int> _regfuncs;
		
		public:
			void pushMessage(const Servicemessage* msg);
			inline bool msgQueueEmpty() { return this->_msgQueue.empty(); }
			inline bool inQueue() { return this->_inQueue; }
			inline void intoQueue() { assert(this->_inQueue == false); this->_inQueue = true; }
			inline void exitQueue() { assert(this->_inQueue == true); this->_inQueue = false; }
			
		private:
			bool _inQueue = false;
			LockfreeQueue<Servicemessage*> _msgQueue;
			Servicemessage* getMessage();
	};
}

#endif
