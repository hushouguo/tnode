/*
 * \file: MessageParser.h
 * \brief: Created by hushouguo at 17:01:33 Aug 09 2017
 */
 
#ifndef __MESSAGE_PARSER_H__
#define __MESSAGE_PARSER_H__

using namespace google::protobuf;
using namespace google::protobuf::compiler;

BEGIN_NAMESPACE_TNODE {
	class MessageParser {
		public:
			MessageParser();
			~MessageParser();

		public:
			bool init();
			bool init(const char* filename);/* filename also is directory */
			void cleanup();

		public:
			bool encode(lua_State* L, const char* name, void* buf, size_t& bufsize);
			bool encode(lua_State* L, const char* name, std::string& out);
			bool decode(lua_State* L, const char* name, void* buf, size_t bufsize);
			bool decode(lua_State* L, const char* name, const std::string& in);

		private:
			DiskSourceTree _tree;
			Importer* _in;
			DynamicMessageFactory _factory;
			std::unordered_map<uint32_t, Message*> _messages;

			class ImporterErrorCollector : public MultiFileErrorCollector {
				public:
					// implements ErrorCollector ---------------------------------------
					void AddError(const std::string& filename, int line, int column,	const std::string& message) override {
						Error.cout("file: %s:%d:%d, error: %s", filename.c_str(), line, column, message.c_str());
					}

					void AddWarning(const std::string& filename, int line, int column, const std::string& message) override {
						Error.cout("file: %s:%d:%d, error: %s", filename.c_str(), line, column, message.c_str());
					}
			};
			ImporterErrorCollector _errorCollector;

			bool parseProtoFile(const char* filename);
			Message* NewMessage(const char* name);

		private:
			bool encodeDescriptor(lua_State* L, Message* message, const Descriptor* descriptor, const Reflection* ref);
			bool encodeField(lua_State* L, Message* message, const FieldDescriptor* field, const Reflection* ref);
			bool encodeFieldSimple(lua_State* L, Message* message, const FieldDescriptor* field, const Reflection* ref);
			bool encodeFieldRepeated(lua_State* L, Message* message, const FieldDescriptor* field, const Reflection* ref);

			bool decodeDescriptor(lua_State* L, const Message& message, const Descriptor* descriptor, const Reflection* ref);
			bool decodeField(lua_State* L, const Message& message, const FieldDescriptor* field, const Reflection* ref);
			bool decodeFieldSimple(lua_State* L, const Message& message, const FieldDescriptor* field, const Reflection* ref);
			bool decodeFieldRepeated(lua_State* L, const Message& message, const FieldDescriptor* field, const Reflection* ref, int index);
			void decodeFieldDefaultValue(lua_State* L, const Message& message, const FieldDescriptor* field);
	};
}

#endif
