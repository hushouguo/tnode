/*
 * \file: MessageParser.h
 * \brief: Created by hushouguo at 17:01:33 Aug 09 2017
 */
 
#ifndef __MESSAGE_PARSER_H__
#define __MESSAGE_PARSER_H__

BEGIN_NAMESPACE_TNODE {
	class MessageParser {
		public:
			virtual ~MessageParser() = 0;

		public:
			virtual bool load(const char* filename) = 0; // filename also is directory
			virtual bool encode(lua_State* L, const char* name, void* buf, size_t& bufsize) = 0;
			virtual bool encode(lua_State* L, const char* name, std::string& out) = 0;
			virtual bool decode(lua_State* L, const char* name, void* buf, size_t bufsize) = 0;
			virtual bool decode(lua_State* L, const char* name, const std::string& in) = 0;
	};

	struct MessageParserCreator {
		static MessageParser* create();
	};
}

#endif
