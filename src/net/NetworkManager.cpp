/*
 * \file: NetworkManager.cpp
 * \brief: Created by hushouguo at 17:02:40 Apr 02 2019
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	void NetworkManager::init() {
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
					for (const Servicemessage* msg = socket->getMessage(); msg; msg = socket->getMessage()) {
						sServiceManager.pushMessage(msg);
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
		}
	}
			
	bool NetworkManager::sendMessage(const Servicemessage* message) {
		SOCKET s = message->fd;
		assert(s < MAX_SOCKET);
		Socket* socket = this->_sockets[s];
		CHECK_RETURN(socket, false, "Not found socket: %d when send msg: %d", s, message->rawmsg.msgid);
		if (!socket->send(message)) {
			// Note: after send, message maybe already released
			this->closeSocket(s, "sendMessage error");
			return false;
		}
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
