/*
 * \file: LuaState.h
 * \brief: Created by hushouguo at 13:11:09 Mar 26 2019
 */
 
#ifndef __LUASTATE_H__
#define __LUASTATE_H__

BEGIN_NAMESPACE_TNODE {

#define	USE_LUAJIT							1
#define LUA_STACK_SIZE						8192
#define LUA_REGISTER_NAMESPACE				"cc"
#define LUA_REGISTER_SERVICE				"cc.service"

	const char* luaT_tostring(lua_State* L, int idx);

#define luaT_tracestack(L)	\
	do {\
		int args = lua_gettop(L);\
		Debug << "lua tracestack: " << args;\
		for (int i = 1; i <= args; ++i) {\
			Debug << "  [" << i << "]  " << luaT_tostring(L, i) << "(" << lua_typename(L, lua_type(L, i)) << ")";\
		}\
	} while(0)

	template <typename T> void luaT_set_object(lua_State* L, const char* name, T* object) {
		lua_getglobal(L, "_G");
		lua_pushstring(L, name);
		lua_pushlightuserdata(L, object);
		lua_rawset(L, -3);
		lua_pop(L, 1);
	}

	template <typename T> T* luaT_get_object(lua_State* L, const char* name) {
		lua_getglobal(L, name);
		Assert(lua_islightuserdata(L, -1), "top: %d, type: %s", lua_gettop(L), lua_typename(L, lua_type(L, -1)));
		void* userdata = lua_touserdata(L, -1);
		lua_pop(L, 1);/* remove `userdata` */
		return static_cast<T*>(userdata);
	}

#define SetService(L, S)	luaT_set_object<Service>(L, LUA_REGISTER_SERVICE, S)
#define GetService(L)		luaT_get_object<Service>(L, LUA_REGISTER_SERVICE)

#define LUA_REGISTER(L, F, ROUTINE) \
	lua_pushstring(L, F);\
	lua_pushcfunction(L, ROUTINE);\
	lua_rawset(L, -3);

	class Service;
	class LuaState {
		public:
			LuaState(Service* service, u32 stackSize = LUA_STACK_SIZE);
			~LuaState();

		public:
			bool executeString(const char* codes);
			bool executeFile(const char* filename);

			bool callFunction(int ref, int nargs);	
			bool pushGlobalFunction(const char* function);
			bool executeFunction(int args);

			void cleanup();
			void dumpRootTable();
			void dumpRegistryTable();

			void registerNamespace(const char* ns);
			void beginNamespace(const char* ns);
			void endNamespace();

			void registerFunction(const char* function, lua_CFunction routine);
			void registerGlobalFunction(const char* function, lua_CFunction routine);

			void registerInteger(const char* key, int value);
			void registerLong(const char* key, long value);
			void registerDouble(const char* key, double value);
			void registerString(const char* key, const char* value);

			inline lua_State* luaState() { return this->_L; }
			inline Service* service() { return this->_service; }

			void verinfo();

		private:
			Service* _service = nullptr;
			lua_State* _L = nullptr;
			void dumpTable(int idx, const char* prefix);
	};
}

#endif
