/*
 * \file: NetworkManager.cpp
 * \brief: Created by hushouguo at 17:02:40 Apr 02 2019
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	bool NetworkManager::init() {
		this->_netthread = new std::thread([]() { this->run(); });
	}

	void NetworkManager::stop() {
		if (!this->_isstop) {
			this->_isstop = true;
			//TODO: close epfd
			if (this->_netthread && this->_netthread->joinable()) {
				this->_netthread->join();
			}
			SafeDelete(this->_netthread);
		}	
	}

	void NetworkManager::run() {
		while (!this->_isstop) {
			this->_poll.run(-1);
		}
	}
			
	SocketServer* NetworkManager::newserver(Service* service, const char* address, int port) {
		SocketServer* socketServer = SocketServerCreator::create();
		socketServer->context(service);
		socketServer->listen(address, port);
		this->_locker.lock();
		this->_sockets.insert(std::make_pair(socketServer->fd(), socketServer));
		this->_locker.unlock();
		this->_poll.addSocket(socketServer);
		return socketServer;
	}

	SocketClient* NetworkManager::newclient(Service* service, const char* address, int port) {
		SocketClient* socketClient = SocketClientCreator::create();
		socketClient->context(service);
		socketClient->connect(address, port);
		this->_locker.lock();
		this->_sockets.insert(std::make_pair(socketClient->fd(), socketClient));
		this->_locker.unlock();
		this->_poll.addSocket(socketClient);
		return socketClient;
	}

	void NetworkManager::close(Service* service, SOCKET s) {
		//TODO:
	}

	INITIALIZE_INSTANCE(NetworkManager);
}
