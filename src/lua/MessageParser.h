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
			bool load(const char* filename); // filename also is directory
			bool encode(lua_State* L, const char* name, void* buf, size_t& bufsize);
			bool encode(lua_State* L, const char* name, std::string& out);
			bool decode(lua_State* L, const char* name, void* buf, size_t bufsize);
			bool decode(lua_State* L, const char* name, const std::string& in);			
	};

	struct MessageParserCreator {
		static MessageParser* create();
	};
}

#endif
