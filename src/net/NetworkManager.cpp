/*
 * \file: NetworkManager.cpp
 * \brief: Created by hushouguo at 17:02:40 Apr 02 2019
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	NetworkManager::NetworkManager() {
		memset(this->_sockets, 0, sizeof(this->_sockets));
		this->schedule();
	}
	
	void NetworkManager::stop() {
		this->_poll.shutdown();
		for (auto& socket : this->_sockets) {
			SafeDelete(socket);
		}
	}

	void NetworkManager::run() {
		auto readSocket = [this](SOCKET s) {
			Socket* socket = this->_sockets[s];
			CHECK_RETURN(socket, void(0), "Not found socket: %d when readSocket", s);
			if (socket->socket_type() == SOCKET_SERVER) {
				SocketServer* socketServer = dynamic_cast<SocketServer*>(socket);
				assert(socketServer);
				for (SOCKET newfd = socketServer->accept(); newfd != -1; newfd = socketServer->accept()) {
					Socket* newsocket = SocketCreator::create(newfd);
					assert(this->_sockets[newfd] == nullptr);
					this->_sockets[newfd] = newsocket;
					this->_poll.addSocket(newfd);
					//Debug << "accept connection: " << newfd;
				}
			}
			else {
				if (!socket->receive()) {
					this->closeSocket(s, "recv error");
				}
				else {
					ByteBuffer& buffer = socket->getBuffer();
					while (true) {
						Socketmessage* rawmsg = (Socketmessage*) (buffer.rbuffer());
						//Debug << "Socket: " << socket->fd() << ", buffer.size: " << buffer.size() << ", rawmsg->len: " << rawmsg->len;
						if (buffer.size() >= sizeof(rawmsg->len) && buffer.size() >= rawmsg->len) {
							Servicemessage* newmsg = allocate_message(rawmsg->len);
							newmsg->fd = s;
							memcpy(&newmsg->rawmsg, rawmsg, rawmsg->len);
							sServiceManager.pushMessage(newmsg);
							buffer.rlength(rawmsg->len);
						}
						else { break; }
					}
				}
			}
		};

		auto writeSocket = [this](SOCKET s) {
			Socket* socket = this->_sockets[s];
			CHECK_RETURN(socket, void(0), "Not found socket: %d when writeSocket", s);
			if (!socket->send()) {
				this->closeSocket(s, "send error");
			}
		};

		auto errorSocket = [this](SOCKET s) {
			this->closeSocket(s, "poll error"); 
		};
		
		while (!sConfig.halt) {
			this->_poll.run(-1, readSocket, writeSocket, errorSocket);
			while (!this->_sendQueue.empty()) {
				Servicemessage* msg = this->_sendQueue.pop_front();
				assert(msg->fd < MAX_SOCKET);
				Socket* socket = this->_sockets[msg->fd];
				CHECK_ALARM(socket, "Not found socket: %d when send msg: %d", msg->fd, msg->rawmsg.msgid);
				if (socket) {
					if (!socket->send(&msg->rawmsg, msg->rawmsg.len)) {
						this->closeSocket(msg->fd, "sendQueue error");
					}
					// Note: socket is invalid when closeSocket
				}
				release_message(msg);
			}
		}
	}
			
	bool NetworkManager::sendString(SOCKET s, u64 entityid, u32 msgid, std::string& outstring) {
		assert(s < MAX_SOCKET);
		Socket* socket = this->_sockets[s];
		CHECK_RETURN(socket, false, "Not found socket: %d when send msg: %d", s, msgid);
		Servicemessage* msg = allocate_message(outstring.length());
		msg->fd = s;
		msg->rawmsg.len = outstring.length() + sizeof(Socketmessage);
		msg->rawmsg.msgid = msgid;
		msg->rawmsg.entityid = entityid;
		memcpy(msg->rawmsg.payload, outstring.data(), outstring.length());
		this->_sendQueue.push_back(msg);
		return true;
	}
			
	SOCKET NetworkManager::newserver(const char* address, int port) {
		SocketServer* socketServer = SocketServerCreator::create();
		bool retval = socketServer->listen(address, port);
		if (!retval) {
			SafeDelete(socketServer);
			return -1;
		}
		assert(socketServer->fd() < MAX_SOCKET);
		assert(this->_sockets[socketServer->fd()] == nullptr);
		this->_sockets[socketServer->fd()] = socketServer;
		this->_poll.addSocket(socketServer->fd());
		return socketServer->fd();
	}

	SOCKET NetworkManager::newclient(const char* address, int port) {
		SocketClient* socketClient = SocketClientCreator::create();
		bool retval = socketClient->connect(address, port);
		if (!retval) {
			SafeDelete(socketClient);
			return -1;
		}
		assert(socketClient->fd() < MAX_SOCKET);
		assert(this->_sockets[socketClient->fd()] == nullptr);
		this->_sockets[socketClient->fd()] = socketClient;
		this->_poll.addSocket(socketClient->fd());
		return socketClient->fd();
	}

	void NetworkManager::closeSocket(SOCKET s, const char* reason) {
		Debug << "Socket: " << s << " close for reason: " << reason;
		this->_poll.removeSocket(s);
		assert(s < MAX_SOCKET);
		Socket* socket = this->_sockets[s];
		CHECK_ALARM(socket, "Not found socket: %d", s);
		SafeDelete(socket);
		this->_sockets[s] = nullptr;
	}

	INITIALIZE_INSTANCE(NetworkManager);
}
