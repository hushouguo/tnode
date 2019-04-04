/*
 * \file: luaT.h
 * \brief: Created by hushouguo at 15:00:36 Apr 04 2019
 */
 
#ifndef __LUAT_H__
#define __LUAT_H__

#define	USE_LUAJIT							1
#define LUA_STACK_SIZE						8192
#define LUA_REGISTER_NAMESPACE				"cc"
#define LUA_REGISTER_SERVICE				"cc.service"

BEGIN_NAMESPACE_TNODE {
	lua_State* luaT_newstate(u32 stackSize = LUA_STACK_SIZE);
	void luaT_close(lua_State*);

	void luaT_setOwner(lua_State*, u32 sid);
	u32 luaT_getOwner(lua_State*);
	
	void luaT_showversion(lua_State* L);
	const char* luaT_tostring(lua_State* L, int idx);
	
	bool luaT_pcall(lua_State* L, int args);
	void luaT_getRegistry(lua_State* L, int ref);
	void luaT_getGlobalFunction(lua_State* L, const char* func);
		
	bool luaT_execString(lua_State* L, const char* s);
	bool luaT_execFile(lua_State* L, const char* filename);
		
	void luaT_cleanup(lua_State* L);
	void luaT_tracestack(lua_State* L);
	
	void luaT_dumpTable(lua_State* L, int idx, const char* prefix);
	void luaT_dumpRootTable(lua_State* L);
	void luaT_dumpRegistryTable(lua_State* L);	
	
	void luaT_regNamespace(lua_State* L, const char* ns);
	void luaT_beginNamespace(lua_State* L, const char* ns);
	void luaT_endNamespace(lua_State* L);
	
	void luaT_regFunction(lua_State* L, const char* func, lua_CFunction routine);
	void luaT_regGlobalFunction(lua_State* L, const char* func, lua_CFunction routine);	
	void luaT_regInteger(lua_State* L, const char* key, int value);
	void luaT_regLong(lua_State* L, const char* key, long value);
	void luaT_regDouble(lua_State* L, const char* key, double value);
	void luaT_regString(lua_State* L, const char* key, const char* value);
	void luaT_regObject(lua_State* L, const char* key, const void* value);

	template <typename T> void luaT_pushvalue(lua_State* L, T value) {
		//assert(false);
		//lua_pushlightuserdata(L, value);
		lua_pushnil(L);
		Error << "luaT_pushvalue template specialization failure for type: " << typeid(T).name();
	}

	// boolean
	template <> void luaT_pushvalue(lua_State* L, bool value);

	// integer
	template <> void luaT_pushvalue(lua_State* L, s8 value);
	template <> void luaT_pushvalue(lua_State* L, s16 value);
	template <> void luaT_pushvalue(lua_State* L, s32 value);
	template <> void luaT_pushvalue(lua_State* L, s64 value);
	template <> void luaT_pushvalue(lua_State* L, u8 value);
	template <> void luaT_pushvalue(lua_State* L, u16 value);
	template <> void luaT_pushvalue(lua_State* L, u32 value);
	template <> void luaT_pushvalue(lua_State* L, u64 value);
	template <> void luaT_pushvalue(lua_State* L, long long value);
	template <> void luaT_pushvalue(lua_State* L, unsigned long long value);

	// float
	template <> void luaT_pushvalue(lua_State* L, float value);
	template <> void luaT_pushvalue(lua_State* L, double value);

	// string
	template <> void luaT_pushvalue(lua_State* L, char* value);
	template <> void luaT_pushvalue(lua_State* L, const char* value);
	template <> void luaT_pushvalue(lua_State* L, std::string value);

	// lightuserdata
	template <> void luaT_pushvalue(lua_State* L, void* value);
	template <> void luaT_pushvalue(lua_State* L, const void* value);

#define LUA_REGISTER(L, F, ROUTINE) \
	lua_pushstring(L, F);\
	lua_pushcfunction(L, ROUTINE);\
	lua_rawset(L, -3);

	bool luaT_callFunction(lua_State* L);
	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	bool luaT_callFunction(lua_State* L, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6) {
		luaT_pushvalue(L, t1);
		luaT_pushvalue(L, t2);
		luaT_pushvalue(L, t3);
		luaT_pushvalue(L, t4);
		luaT_pushvalue(L, t5);
		luaT_pushvalue(L, t6);
		return luaT_pcall(L, 6);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5>
	bool luaT_callFunction(lua_State* L, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) {
		luaT_pushvalue(L, t1);
		luaT_pushvalue(L, t2);
		luaT_pushvalue(L, t3);
		luaT_pushvalue(L, t4);
		luaT_pushvalue(L, t5);
		return luaT_pcall(L, 5);
	}
	
	template <typename T1, typename T2, typename T3, typename T4>
	bool luaT_callFunction(lua_State* L, T1 t1, T2 t2, T3 t3, T4 t4) {
		luaT_pushvalue(L, t1);
		luaT_pushvalue(L, t2);
		luaT_pushvalue(L, t3);
		luaT_pushvalue(L, t4);
		return luaT_pcall(L, 4);
	}
	
	template <typename T1, typename T2, typename T3>
	bool luaT_callFunction(lua_State* L, T1 t1, T2 t2, T3 t3) {
		luaT_pushvalue(L, t1);
		luaT_pushvalue(L, t2);
		luaT_pushvalue(L, t3);
		return luaT_pcall(L, 3);
	}
	
	template <typename T1, typename T2>
	bool luaT_callFunction(lua_State* L, T1 t1, T2 t2) {
		luaT_pushvalue(L, t1);
		luaT_pushvalue(L, t2);
		return luaT_pcall(L, 2);
	}
	
	template <typename T1>
	bool luaT_callFunction(lua_State* L, T1 t1) {
		luaT_pushvalue(L, t1);
		return luaT_pcall(L, 1);
	}	
}

#endif
