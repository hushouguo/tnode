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
			inline LuaState* luaState() { return this->_luaState; }
			inline MessageParser* messageParser() { return this->_messageParser; }

		public:
			bool init(const char* entryfile);
			void stop();
			void run() override;

		private:
			bool _isstop = false;
			std::string _entryfile;
			LuaState* _luaState = nullptr;
			MessageParser* _messageParser = nullptr;

		public:
			inline void regfunction(SOCKET s, int ref) {
				this->_regfuncs.insert(std::make_pair(s, ref));
			}

			inline void msgfunction(u32 msgid, int ref) {
				this->_msgfuncs.insert(std::make_pair(msgid, ref));
			}

		private:
			std::unordered_map<SOCKET, int> _regfuncs;
			std::unordered_map<u32, int> _msgfuncs;
		
		public:
			void pushMessage(const Servicemessage* msg);
			inline bool msgQueueEmpty() { return this->_msgQueue.empty(); }
			
		private:
			LockfreeQueue<const Servicemessage*> _msgQueue;
			const Servicemessage* getMessage();
	};
}

#endif
