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

	template <typename T> void luaT_pushvalue(lua_State* L, T value) {
		//assert(false);
		//lua_pushlightuserdata(L, value);
		lua_pushnil(L, value);
		Error << "luaT_pushvalue template specialization failure for type: " << typeid(T).name();
	}
	
	// boolean
	template <> void luaT_pushvalue(lua_State* L, bool value) {
		lua_pushboolean(L, value);
	}

	// integer
	template <> void luaT_pushvalue(lua_State* L, s8 value) {
		lua_pushinteger(L, value);
	}
	template <> void luaT_pushvalue(lua_State* L, s16 value) {
		lua_pushinteger(L, value);
	}
	template <> void luaT_pushvalue(lua_State* L, s32 value) {
		lua_pushinteger(L, value);
	}
	template <> void luaT_pushvalue(lua_State* L, s64 value) {
		lua_pushinteger(L, value);
	}
	template <> void luaT_pushvalue(lua_State* L, u8 value) {
		lua_pushinteger(L, value);
	}
	template <> void luaT_pushvalue(lua_State* L, u16 value) {
		lua_pushinteger(L, value);
	}
	template <> void luaT_pushvalue(lua_State* L, u32 value) {
		lua_pushinteger(L, value);
	}
	template <> void luaT_pushvalue(lua_State* L, u64 value) {
		lua_pushinteger(L, value);
	}
	template <> void luaT_pushvalue(lua_State* L, long long value) {
		lua_pushinteger(L, value);
	}
	template <> void luaT_pushvalue(lua_State* L, unsigned long long value) {
		lua_pushinteger(L, value);
	}

	// float
	template <> void luaT_pushvalue(lua_State* L, float value) {
		lua_pushnumber(L, value);
	}
	template <> void luaT_pushvalue(lua_State* L, double value) {
		lua_pushnumber(L, value);
	}

	// string
	template <> void luaT_pushvalue(lua_State* L, char* value) {
		lua_pushstring(L, value);
	}
	template <> void luaT_pushvalue(lua_State* L, const char* value) {
		lua_pushstring(L, value);
	}
	template <> void luaT_pushvalue(lua_State* L, std::string value) {
		lua_pushlstring(L, value.data(), value.length());
	}

	// lightuserdata
	template <> void luaT_pushvalue(lua_State* L, void* value) {
		lua_pushlightuserdata(L, value);
	}
	template <> void luaT_pushvalue(lua_State* L, const void* value) {
		lua_pushlightuserdata(L, value);
	}
	
#define LUA_REGISTER(L, F, ROUTINE) \
	lua_pushstring(L, F);\
	lua_pushcfunction(L, ROUTINE);\
	lua_rawset(L, -3);

	template <typename T1 = void, typename T2 = void, typename T3 = void, typename T4 = void, typename T5 = void, typename T6 = void>
	void luaT_callback(lua_State* L, int ref, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		luaT_pushvalue(L, t1);
		luaT_pushvalue(L, t2);
		luaT_pushvalue(L, t3);
		luaT_pushvalue(L, t4);
		luaT_pushvalue(L, t5);
		luaT_pushvalue(L, t6);
		lua_call(L, 6, 0);
	}
	
	template <typename T1, typename T2, typename T3, typename T4, typename T5>
	void luaT_callback(lua_State* L, int ref, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		luaT_pushvalue(L, t1);
		luaT_pushvalue(L, t2);
		luaT_pushvalue(L, t3);
		luaT_pushvalue(L, t4);
		luaT_pushvalue(L, t5);
		lua_call(L, 5, 0);
	}
	
	template <typename T1, typename T2, typename T3, typename T4>
	void luaT_callback(lua_State* L, int ref, T1 t1, T2 t2, T3 t3, T4 t4) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		luaT_pushvalue(L, t1);
		luaT_pushvalue(L, t2);
		luaT_pushvalue(L, t3);
		luaT_pushvalue(L, t4);
		lua_call(L, 4, 0);
	}
	
	template <typename T1, typename T2, typename T3>
	void luaT_callback(lua_State* L, int ref, T1 t1, T2 t2, T3 t3) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		luaT_pushvalue(L, t1);
		luaT_pushvalue(L, t2);
		luaT_pushvalue(L, t3);
		lua_call(L, 3, 0);
	}
	
	template <typename T1, typename T2>
	void luaT_callback(lua_State* L, int ref, T1 t1, T2 t2) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		luaT_pushvalue(L, t1);
		luaT_pushvalue(L, t2);
		lua_call(L, 2, 0);
	}
	
	template <typename T1>
	void luaT_callback(lua_State* L, int ref, T1 t1) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		luaT_pushvalue(L, t1);
		lua_call(L, 1, 0);
	}
	
	template <>
	void luaT_callback(lua_State* L, int ref) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		lua_call(L, 0, 0);
	}

	class Service;
	class LuaState {
		public:
			LuaState(Service* service, u32 stackSize = LUA_STACK_SIZE);
			~LuaState();

		public:
			bool executeString(const char* codes);
			bool executeFile(const char* filename);

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
