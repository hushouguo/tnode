/*
 * \file: lua_reg_standard_functions.cpp
 * \brief: Created by hushouguo at 15:44:05 Mar 26 2019
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
#if 0
	static int cc_newtimer(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 0, 0, "`%s` parameter lack: %d\n", __FUNCTION__, args);
		CHECK_RETURN(lua_isfunction(L, -args), 0, "`%s` expect function: %s", __FUNCTION__, lua_typename(L, lua_type(L, -args)));
		lua_pushvalue(L, -args);
		int ref = luaL_ref(L, LUA_REGISTRYINDEX);
		Service* service = GetService(L);
		assert(service);
		service->regfunction(ref);
		return 0;		
	}
#endif

	//
	// table json_decode(string)
	static int cc_json_decode(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 0, 0, "`%s` lack args:%d", __FUNCTION__, args);
		CHECK_RETURN(lua_isstring(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));	
		size_t len = 0;
		const char* jsonstr = lua_tolstring(L, -args, &len);
		return lua_json_parser_decode(L, jsonstr, len) ? 1 : 0;
	}

	//
	// string json_encode(table)
	static int cc_json_encode(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 0, 0, "`%s` lack args:%d", __FUNCTION__, args);
		CHECK_RETURN(lua_istable(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));
		ByteBuffer byteBuffer;
		if (lua_json_parser_encode(L, &byteBuffer)) {
			lua_pushstring(L, (const char*)(byteBuffer.rbuffer()));
		}
		else {
			lua_pushnil(L);
		}
		return 1;
	}

	//
	// table xml_decode(string)
	static int cc_xml_decode(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 0, 0, "`%s` lack args:%d", __FUNCTION__, args);
		CHECK_RETURN(lua_isstring(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));	
		size_t len = 0;
		const char* file = lua_tolstring(L, -args, &len);	
		return lua_xml_parser_decode(L, file) ? 1 : 0;
	}

	//
	// string xml_encode(table)
	static int cc_xml_encode(lua_State* L) {
		Error << "xml_encode not implement!";
		return 0;
	}
	
	//
	// int hash_string(string)
	static int cc_hash_string(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 0, 0, "`%s` lack args:%d", __FUNCTION__, args);
		CHECK_RETURN(lua_isstring(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));		
		size_t len = 0;
		const char* string = lua_tolstring(L, -args, &len);
		lua_pushinteger(L, hashString(string, len));
		return 1;
	}

	//
	// int random()
	static int cc_random(lua_State* L) {
		lua_pushinteger(L, randomValue());
		return 1;
	}

	//
	// int random_between(int min, int max)
	static int cc_random_between(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 1, 0, "`%s` lack args:%d", __FUNCTION__, args);
		CHECK_RETURN(lua_isnumber(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));		
		CHECK_RETURN(lua_isnumber(L, -(args - 1)), 0, "[%s]", lua_typename(L, lua_type(L, -(args - 1))));		
		lua_Integer min = lua_tointeger(L, -args);
		lua_Integer max = lua_tointeger(L, -(args - 1));	
		lua_pushinteger(L, randomBetween(min, max));
		return 1;
	}

	//
	// string base64_encode(string)
	static int cc_base64_encode(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 0, 0, "`%s` lack args:%d", __FUNCTION__, args);
		CHECK_RETURN(lua_isstring(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));		
		size_t len = 0;
		const char * string = lua_tolstring(L, -args, &len);	
		std::string base64_string;
	   	base64_encode((unsigned char const*) string, len, base64_string);	
		lua_pushstring(L, base64_string.c_str());
		return 1;
	}

	//
	// string base64_decode(string)
	static int cc_base64_decode(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 0, 0, "`%s` lack args:%d", __FUNCTION__, args);
		CHECK_RETURN(lua_isstring(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));			
		std::string encoded_string = lua_tostring(L, -args);	
		std::string decoded_string;
	   	base64_decode(encoded_string, decoded_string);	
		lua_pushstring(L, decoded_string.c_str());
		return 1;
	}
	
	//
	// string message_encode(name, table)
	static int cc_message_encode(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 1, 0, "`%s` lack args:%d", __FUNCTION__, args);
		CHECK_RETURN(lua_isstring(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));
		CHECK_RETURN(lua_istable(L, -(args - 1)), 0, "[%s]", lua_typename(L, lua_type(L, -(args - 1))));
		const char* name = lua_tostring(L, -args);
		Service* service = GetService(L);
		assert(service);
		std::string outstring;
		bool retval = service->messageParser()->encode(L, name, outstring);
		CHECK_RETURN(retval, 0, "encode message: %s error", name);
		lua_pushstring(L, outstring.c_str());
		return 1;
	}

	//
	// table message_decode(name, string)
	static int cc_message_decode(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 1, 0, "`%s` lack args:%d", __FUNCTION__, args);
		CHECK_RETURN(lua_isstring(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));			
		CHECK_RETURN(lua_isstring(L, -(args - 1)), 0, "[%s]", lua_typename(L, lua_type(L, -(args - 1))));			
		const char* name = lua_tostring(L, -args);
		std::string instring = lua_tostring(L, -(args - 1));
		Service* service = GetService(L);
		assert(service);
		bool retval = service->messageParser()->decode(L, name, instring);
		CHECK_RETURN(retval, 0, "decode message: %s error", name);
		// table is in the top of stack
		return 1;
	}

	//
	// table md5(string)
	static int cc_md5(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 0, 0, "`%s` lack args:%d", __FUNCTION__, args);
		CHECK_RETURN(lua_isstring(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));	
		size_t len = 0;
		const char * content = lua_tolstring(L, -args, &len);
		unsigned char digest[MD5_DIGEST_LENGTH];	
		MD5((const unsigned char *)content, len, digest);	
		lua_newtable(L);
		for (int n = 0; n < MD5_DIGEST_LENGTH; ++n) {
			lua_pushinteger(L, n);
			lua_pushinteger(L, digest[n]);
			lua_settable(L, -3);
		}	
		return 1;
	}

	//
	// u64 timesec()
	static int cc_timesec(lua_State* L) {
		sTime.now();
		lua_pushinteger(L, sTime.secondPart());
		return 1;
	}

	//
	// u64 timemsec()
	static int cc_timemsec(lua_State* L) {
		sTime.now();
		lua_pushinteger(L, sTime.milliseconds());
		return 1;
	}

	//
	// YYYY, MM, DD, HH, MM, SS timespec([u64])
	static int cc_timespec(lua_State* L) {
		std::time_t seconds;
		std::time_t ts_milliseconds;
	
		int args = lua_gettop(L);
		if (args == 0) {
			auto time_now = std::chrono::system_clock::now();
			auto duration_millisecond = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch());
			auto millisecond_part = duration_millisecond - std::chrono::duration_cast<std::chrono::seconds>(duration_millisecond);
			seconds = std::chrono::system_clock::to_time_t(time_now);
			ts_milliseconds = millisecond_part.count();
		}
		else if (args == 1) {
			CHECK_RETURN(lua_isnumber(L, -1), 0, "[%s]", lua_typename(L, lua_type(L, -1)));		
			lua_Integer milliseconds = lua_tointeger(L, -1);
			seconds = milliseconds / 1000;
			ts_milliseconds = milliseconds % 1000;
		}
	
		std::tm * local_time_now = std::localtime(&seconds);
	
		lua_pushinteger(L, local_time_now->tm_year + 1900);
		lua_pushinteger(L, local_time_now->tm_mon + 1);
		lua_pushinteger(L, local_time_now->tm_mday);
		lua_pushinteger(L, local_time_now->tm_hour);
		lua_pushinteger(L, local_time_now->tm_min);
		lua_pushinteger(L, local_time_now->tm_sec);
		lua_pushinteger(L, ts_milliseconds);
	
		return 7;
	}

	//
	// string timestamp()
	// string timestamp(seconds)
	// string timestamp(string)
	// string timestamp(seconds, string)
	static int cc_timestamp(lua_State* L) {
		char buffer[64];
		int args = lua_gettop(L);
		
		if (args == 0) {
			timestamp(buffer, sizeof(buffer));
		}
		else if (args == 1) {
			if (lua_isnumber(L, -1)) { // string timestamp(seconds)
				timestamp(buffer, sizeof(buffer), lua_tointeger(L, -1));
			}
			else if (lua_isstring(L, -1)) { // string timestamp(string)
				timestamp(buffer, sizeof(buffer), 0, lua_tostring(L, -1));
			}
			else {
				Error.cout("`%s` parameter error:%s", __FUNCTION__, lua_typename(L, lua_type(L, -1)));
				return 0;
			}
		}
		else if (args == 2) {
			if (!lua_isnumber(L, -args)) {
				Error.cout("`%s` parameter error:%s", __FUNCTION__, lua_typename(L, lua_type(L, -args)));
				return 0;
			}
			if (!lua_isstring(L, -(args - 1))) {
				Error.cout("`%s` parameter error:%s", __FUNCTION__, lua_typename(L, lua_type(L, -(args - 1))));
				return 0;
			}
			timestamp(buffer, sizeof(buffer), lua_tointeger(L, -args), lua_tostring(L, -(args - 1)));
		}
		else {
			Error.cout("`%s` parameter error:%d", __FUNCTION__, args);
		}
	
		lua_pushstring(L, buffer);
	
		return 1;
	}

	//
	// void msleep(milliseconds)
	static int cc_msleep(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 0, 0, "`%s` lack args:%d", __FUNCTION__, args);
		CHECK_RETURN(lua_isnumber(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));
		lua_Integer milliseconds = lua_tointeger(L, -1);	
		std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));	
		return 0;
	}

	//
	// logger
	static int cc_log_debug(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 0, 0, "`%s` lack args:%d", __FUNCTION__, args);
		CHECK_RETURN(lua_isstring(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));
		const char* content = lua_tostring(L, -args);
		if (content) {
			Debug << "[LUA] " << content;
		}
		return 0;
	}
	static int cc_log_trace(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 0, 0, "`%s` lack args:%d", __FUNCTION__, args);
		CHECK_RETURN(lua_isstring(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));
		const char* content = lua_tostring(L, -args);
		if (content) {
			Trace << "[LUA] " << content;
		}
		return 0;
	}
	static int cc_log_alarm(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 0, 0, "`%s` lack args:%d", __FUNCTION__, args);
		CHECK_RETURN(lua_isstring(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));
		const char* content = lua_tostring(L, -args);
		if (content) {
			Alarm << "[LUA] " << content;
		}
		return 0;
	}
	static int cc_log_error(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 0, 0, "`%s` lack args:%d", __FUNCTION__, args);
		CHECK_RETURN(lua_isstring(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));
		const char* content = lua_tostring(L, -args);
		if (content) {
			Error << "[LUA] " << content;
		}
		return 0;
	}

	//
	// bool newservice(entryfile)
	static int cc_newservice(lua_State* L) {
		int args = lua_gettop(L);
		CHECK_RETURN(args > 0, 0, "`%s` lack args: %d", __FUNCTION__, args);
		CHECK_RETURN(lua_isstring(L, -args), 0, "[%s]", lua_typename(L, lua_type(L, -args)));
		const char* entryfile = lua_tostring(L, -args);
		bool retval = sServiceManager.newservice(entryfile);
		lua_pushboolean(L, retval ? 1 : 0);
		return 1;
	}


	//
	// void sendmsg(service, fd, msgid, userdata, msglen)
	static int cc_sendmsg(lua_State* L) {
		return 0;
	}


	//
	// bool listen(address, port, function(fd, entityid, msgid, Servicemessage))
	static int cc_listen(lua_State* L) {
		return 0;
	}


	//
	// bool connect(address, port, function(fd, entityid, msgid, Servicemessage))
	static int cc_connect(lua_State* L) {
		return 0;
	}


	//
	// void response(fd, msgid, o)
	static int cc_response(lua_State* L) {
		return 0;
	}
	
	void lua_reg_standard_functions(LuaState* luaState) {
		lua_State* L = luaState->luaState();

		luaState->beginNamespace(LUA_REGISTER_NAMESPACE);

		// service
		LUA_REGISTER(L, "newservice", cc_newservice);
		LUA_REGISTER(L, "sendmsg", cc_sendmsg);

		// network
		LUA_REGISTER(L, "listen", cc_listen);
		LUA_REGISTER(L, "connect", cc_connect);
		LUA_REGISTER(L, "response", cc_response);

		// logger
		LUA_REGISTER(L, "log_debug", cc_log_debug);
		LUA_REGISTER(L, "log_trace", cc_log_trace);
		LUA_REGISTER(L, "log_alarm", cc_log_alarm);
		LUA_REGISTER(L, "log_error", cc_log_error);
		
		// json
		LUA_REGISTER(L, "json_decode", cc_json_decode);
		LUA_REGISTER(L, "json_encode", cc_json_encode);

		// xml
		LUA_REGISTER(L, "xml_decode", cc_xml_decode);
		LUA_REGISTER(L, "xml_encode", cc_xml_encode);

		// hash string
		LUA_REGISTER(L, "hash_string", cc_hash_string);
		
		// random
		LUA_REGISTER(L, "random", cc_random);
		LUA_REGISTER(L, "random_between", cc_random_between);

		// base64
		LUA_REGISTER(L, "base64_encode", cc_base64_encode);
		LUA_REGISTER(L, "base64_decode", cc_base64_decode);

		// md5
		LUA_REGISTER(L, "md5", cc_md5);

		// time
		LUA_REGISTER(L, "timesec", cc_timesec);
		LUA_REGISTER(L, "timemsec", cc_timemsec);
		LUA_REGISTER(L, "timestamp", cc_timestamp);
		LUA_REGISTER(L, "timespec", cc_timespec);
		LUA_REGISTER(L, "msleep", cc_msleep);

		// message parser
		LUA_REGISTER(L, "message_encode", cc_message_encode);
		LUA_REGISTER(L, "message_decode", cc_message_decode);

		
		//TODO: timer
		//TODO: file i/o
		//TODO: http
		//TODO: curl
		//TODO: db agent
		
		luaState->endNamespace();
	}
}
