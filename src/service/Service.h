/*
 * \file: Service.h
 * \brief: Created by hushouguo at 14:51:12 Mar 25 2019
 */
 
#ifndef __SERVICE_H__
#define __SERVICE_H__

BEGIN_NAMESPACE_TNODE {
	class Service : public Entry<int> {
		public:
			Service(int id);
			Service(const char* name);
			const char* getClassName() override { return "Service"; }

		public:
			inline LuaState* luaState() { return this->_luaState; }

		public:
			bool init(const char* entryfile);
			void stop();
			void run();

		private:
			bool _isstop = false;
			std::string _entryfile;
			LuaState* _luaState = nullptr;

		public:
			inline void regfunction(int ref) {
				this->_regfuncs.push_back(ref);
			}

		private:
			std::list<int> _regfuncs;
		
		public:
			void pushMessage(Servicemessage* newmsg);
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
