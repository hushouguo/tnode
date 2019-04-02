/*
 * \file: LuaState.cpp
 * \brief: Created by hushouguo at 13:12:26 Mar 26 2019
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	LuaState::LuaState(Service* service, u32 stackSize) {
		this->_service = service;
		this->_L = luaL_newstate();
		
		luaL_openlibs(this->luaState());
		lua_checkstack(this->luaState(), stackSize/*LUA_STACK_SIZE*/);
		/* todo: setup error handler */
		/* todo: setup memory allocator */

		//verinfo();

		SetService(this->_L, service);
		
		this->registerNamespace(LUA_REGISTER_NAMESPACE);		

		lua_reg_standard_functions(this);

		//lua_standard_functions(this);
		//lua_network_functions(this);
		//lua_db_functions(this);
	}

	LuaState::~LuaState() {
		lua_close(this->luaState());
	}

	bool LuaState::executeString(const char* codes) {
		luaL_loadstring(this->luaState(), codes);
		return this->executeFunction(0);
	}

	bool LuaState::executeFile(const char* filename) {
		char buf[PATH_MAX];
		snprintf(buf, sizeof(buf), "%s", filename);

		char* suffix = strrchr(buf, '.');
		if (suffix && strcasecmp(suffix, ".lua") == 0) {
			*suffix = '\0';
		}

		char buffer[PATH_MAX];
		snprintf(buffer, sizeof(buffer), "require \"%s\"", buf);

		return this->executeString(buffer);
	}

	bool LuaState::pushGlobalFunction(const char* function) {
		lua_getglobal(this->luaState(), function);
		if (!lua_isfunction(this->luaState(), -1)) {
			Error.cout("name `%s` does not represent a Lua function.\n", function);
			lua_pop(this->luaState(), 1);
			return false;
		}
		return true;
	}

	bool LuaState::executeFunction(int args) {
		lua_State* L = this->luaState();
		int traceback = 0, error;
		int func_idx = -(args + 1);
		if (!lua_isfunction(L, func_idx)) {
			Error.cout("value at stack [%d] is not function\n", func_idx);
			lua_pop(L, args + 1);/* remove function and args */
			return false;
		}

		lua_getglobal(L, "__G_TRACKBACK__"); 	/* L: ...func arg1 arg2 ... G */
		if (!lua_isfunction(L, -1)) {
			lua_pop(L, 1);						/* L: ...func arg1 arg2 ... */
		}
		else {
			lua_insert(L, func_idx - 1);		/* L: ... G func arg1 arg2 ... */
			traceback = func_idx - 1;
		}

		//++stack->call_lua_count;
		error = lua_pcall(L, args, 1, traceback);
		//--stack->call_lua_count;

		if (error) {
			if (traceback == 0) {
				Error.cout("%s\n", lua_tostring(L, -1));/* ... error */
				lua_pop(L, 1); /* remove error message from stack */
			}
			else {
				lua_pop(L, 2); /* remove __G_TRACKBACK__ and error message from stack */
			}
			return false;
		}

		/* get return value */
		int ret = 0;
		if (lua_isnumber(L, -1)) {
			ret = (int)lua_tointeger(L, -1);
		}
		else if (lua_isboolean(L, -1)) {
			ret = (int)lua_toboolean(L, -1);
		}

		//discard return value
		if (ret != 0) {
			Debug.cout("ret: %d\n", ret);
		}

		lua_pop(L, 1); /* remove return value from stack, L: ... [G] */

		if (traceback) {
			lua_pop(L, 1); /* remove __G_TRACKBACK__ from stack, L: ... */
		}

		return true;
	}

	void LuaState::cleanup() {
		lua_settop(this->luaState(), 0);
	}

	void LuaState::dumpRootTable() {
		lua_State* L = this->luaState();
		lua_getglobal(L, "_G");
		Debug.cout("dump root table");
		Debug.cout("{");
		dumpTable(lua_gettop(L), "\t");
		Debug.cout("}");
		lua_pop(L, 1);/* remove `table` */
	}

	void LuaState::dumpRegistryTable() {
		lua_State* L = this->luaState();
		lua_getregistry(L);
		Debug.cout("dump registry table");
		Debug.cout("{");
		dumpTable(lua_gettop(L), "\t");
		Debug.cout("}");
		lua_pop(L, 1);/* remove `table` */
	}

	void LuaState::backtrace() {
		lua_State* L = this->luaState();
		int args = lua_gettop(L);
		Debug << "backtrace: " << args;
		for (int i = 1; i <= args; ++i) {
			Debug << "  [" << i << "] " << this->tostring(i);
		}
	}
	
	void LuaState::registerNamespace(const char* ns) {
		lua_State* L = this->luaState();
		lua_getglobal(L, "_G");
		lua_pushstring(L, ns);
		lua_newtable(L);
		lua_rawset(L, -3);
		lua_pop(L, 1);
	}

	void LuaState::beginNamespace(const char* ns) {
		lua_State* L = this->luaState();
		lua_getglobal(L, "_G");
		lua_pushstring(L, ns);
		lua_rawget(L, -2);
	}

	void LuaState::endNamespace() {
		lua_pop(this->luaState(), 2);
	}

	void LuaState::registerFunction(const char* function, lua_CFunction routine) {
		lua_State* L = this->luaState();
		lua_pushstring(L, function);
		lua_pushcfunction(L, routine);
		lua_rawset(L, -3);
	}

	void LuaState::registerGlobalFunction(const char* function, lua_CFunction routine) {
		lua_State* L = this->luaState();
		lua_getglobal(L, "_G");
		lua_pushstring(L, function);
		lua_pushcfunction(L, routine);
		lua_rawset(L, -3);
		lua_pop(L, 1);
	}

	void LuaState::registerInteger(const char* key, int value) {
		lua_State* L = this->luaState();
		lua_pushstring(L, key);
		lua_pushinteger(L, value);
		lua_settable(L, -3);
	}

	void LuaState::registerLong(const char* key, long value) {
		lua_State* L = this->luaState();
		lua_pushstring(L, key);
		lua_pushnumber(L, value);
		lua_settable(L, -3);
	}

	void LuaState::registerDouble(const char* key, double value) {
		lua_State* L = this->luaState();
		lua_pushstring(L, key);
		lua_pushnumber(L, value);
		lua_settable(L, -3);
	}

	void LuaState::registerString(const char* key, const char* value) {
		lua_State* L = this->luaState();
		lua_pushstring(L, key);
		lua_pushstring(L, value);
		lua_settable(L, -3);
	}

	const char* LuaState::tostring(int idx) {
		lua_State* L = this->luaState();
		if (lua_istable(L, idx)) { return "[table]"; }
		else if (lua_isnone(L, idx)) { return "[none]"; }
		else if (lua_isnil(L, idx)) { return "[nil]"; }
		else if (lua_isboolean(L, idx)) {
			return lua_toboolean(L, idx) != 0 ? "[true]" : "[false]";
		}
		else if (lua_isfunction(L, idx)) { return "[function]"; }
		else if (lua_islightuserdata(L, idx)) { return "[lightuserdata]"; }
		else if (lua_isthread(L, idx)) { return "[thread]"; }
		else {
			const char* str = lua_tostring(L, idx);
			if (str) {
				return str;
			}
		}
		return lua_typename(L, lua_type(L, idx));
	}

	void LuaState::dumpTable(int idx, const char* prefix) {
		lua_State* L = this->luaState();
		lua_pushnil(L);
		while(lua_next(L, idx)) {
			/* -2 : key, -1 : value */
			lua_pushvalue(L, -2);
			const char* key = lua_tostring(L, -1);
			const char* value = this->tostring(-2);
			lua_pop(L, 1);

			Debug.cout("%s%15s: %s", prefix, key, value);

			if (lua_istable(L, -1) && strcasecmp(key, LUA_REGISTER_NAMESPACE) == 0) {
				char buffer[960];
				snprintf(buffer, sizeof(buffer), "%s\t\t", prefix);
				Debug.cout("%15s{", prefix);
				this->dumpTable(lua_gettop(L), buffer);
				Debug.cout("%15s}", prefix);
			}

			lua_pop(L, 1);/* removes 'value'; keeps 'key' for next iteration */
		}
	}
	
	bool LuaState::callFunction(int ref) {
		lua_rawgeti(this->luaState(), LUA_REGISTRYINDEX, ref); //overwrite Lua registry entry with what?
		lua_call(this->luaState(), 0, 0);
		return true;
	}

	void LuaState::verinfo() {
#if defined(USE_LUAJIT)
		lua_State* L = this->luaState();

		Debug.cout("JIT: %s -- %s", LUAJIT_VERSION, LUAJIT_COPYRIGHT);

		lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
		lua_getfield(L, -1, "jit");  /* Get jit.* module table. */
		lua_remove(L, -2);
		lua_getfield(L, -1, "status");
		lua_remove(L, -2);
		int n = lua_gettop(L);
		lua_call(L, 0, LUA_MULTRET);
		Debug.cout("JIT: %s", lua_toboolean(L, n) ? "ON" : "OFF");
		/*
		const char *s;
		for (n++; (s = lua_tostring(this->_L, n)); n++) {
			printf(" %s", s);
		}
		printf("\n");
		*/
#endif
	}
}
