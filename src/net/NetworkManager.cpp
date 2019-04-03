/*
 * \file: NetworkManager.cpp
 * \brief: Created by hushouguo at 17:02:40 Apr 02 2019
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	NetworkManager::NetworkManager() {
		memset(this->_sockets, 0, sizeof(this->_sockets));
	}
	
	void NetworkManager::stop() {
		this->_poll.shutdown();
		for (auto& socket : this->_sockets) {
			SafeDelete(socket);
		}
	}

	void NetworkManager::run() {
		this->_poll(0, 
		[this](SOCKET s) { 
			Socket* socket = this->_sockets[s];
			CHECK_RETURN(socket, void(0), "Not found socket: %d when readSocket", s);
			if (socket->socket_type() == SOCKET_SERVER) {
				SocketServer* socketServer = dynamic_cast<SocketServer*>(socket);
				assert(socketServer);
				for (SOCKET newfd = socketServer->accept(); newfd != -1; newfd = socketServer->accept()) {
					Socket* newsocket = SocketCreator::create(newfd, socketServer->owner());
					assert(this->_sockets[newfd] == nullptr);
					this->_sockets[newfd] = newsocket;
					this->_poll.addSocket(newfd);
				}
			}
			else {
				if (!socket->receive()) {
					this->closeSocket(s, "recv error");
				}
				else {
					ByteBuffer& buffer = socket->recvBuffer();
					while (true) {
						Socketmessage* rawmsg = buffer.rbuffer();
						if (buffer.size() >= sizeof(rawmsg->len) && buffer.size() >= rawmsg->len) {
							Servicemessage* newmsg = allocate_message(rawmsg->len + sizeof(Servicemessage) - sizeof(Socketmessage));
							newmsg->from = 0;
							newmsg->to = socket->owner();
							newmsg->fd = s;
							memcpy(&newmsg->rawmsg, rawmsg, rawmsg->len);
							sServiceManager.pushMessage(newmsg->to, newmsg);
							buffer.rbuffer.rlength(rawmsg->len);
						}
						else { break; }
					}
				}
			}
		},
		[this](SOCKET s) {
			Socket* socket = this->_sockets[s];
			CHECK_RETURN(socket, void(0), "Not found socket: %d when writeSocket", s);
			if (!socket->send()) {
				this->closeSocket(s, "send error");
			}
		},
		[this](SOCKET s) { 
			this->closeSocket(s, "poll error"); 
		});
	}
			
	SocketServer* NetworkManager::newserver(Service* service, const char* address, int port) {
		SocketServer* socketServer = SocketServerCreator::create(service->id);
		bool retval = socketServer->listen(address, port);
		if (!retval) {
			SafeDelete(socketServer);
			return nullptr;
		}
		assert(this->_sockets[socketServer->fd()] == nullptr);
		this->_sockets[socketServer->fd()] = socketServer;
		this->_poll.addSocket(socketServer->fd());
		return socketServer;
	}

	SocketClient* NetworkManager::newclient(Service* service, const char* address, int port) {
		SocketClient* socketClient = SocketClientCreator::create(service->id);
		bool retval = socketClient->connect(address, port);
		if (!retval) {
			SafeDelete(socketClient);
			return nullptr;
		}
		assert(this->_sockets[socketClient->fd()] == nullptr);
		this->_sockets[socketClient->fd()] = socketClient;
		this->_poll.addSocket(socketClient->fd());
		return socketClient;
	}

	void NetworkManager::sendMessage(const Servicemessage* msg) {
		
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
