/*
 * \file: Poll.cpp
 * \brief: Created by hushouguo at 14:39:56 Jun 28 2018
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	Poll::Poll() {
		this->_epfd = epoll_create(NM_POLL_EVENT); /* `NM_POLL_EVENT` is just a hint for the kernel */
		memset(this->_events, 0, sizeof(this->_events));
	}
		
	Poll::~Poll() {
		::close(this->_epfd);
	}

	bool Poll::addSocket(Socket* s) {
		//System << "Poll::addSocket: " << s->fd();
		struct epoll_event ee;
		ee.events = EPOLLET | EPOLLIN | EPOLLOUT | EPOLLERR;
		ee.data.u64 = 0; /* avoid valgrind warning */
		ee.data.ptr = s;
		int rc = epoll_ctl(this->_epfd, EPOLL_CTL_ADD, s->fd(), &ee);		
		CHECK_RETURN(rc == 0, false, "epoll_add error: %d, %s", errno, strerror(errno));
		return true;
	}

	bool Poll::removeSocket(Socket* s) {
		//System << "Poll::removeSocket: " << s;
		struct epoll_event ee;
		ee.events = EPOLLET | EPOLLIN | EPOLLOUT | EPOLLERR;
		ee.data.u64 = 0; /* avoid valgrind warning */
		ee.data.ptr = s;
		/* Note, Kernel < 2.6.9 requires a non null event pointer even for EPOLL_CTL_DEL. */
		int rc = epoll_ctl(this->_epfd, EPOLL_CTL_DEL, s->fd(), &ee);
		CHECK_RETURN(rc == 0, false, "epoll_del error: %d, %s", errno, strerror(errno));
		return true;
	}

	bool Poll::setSocketPollout(Socket* s, bool value) {
		struct epoll_event ee;
		ee.events = value ? (EPOLLET | EPOLLIN | EPOLLOUT | EPOLLERR) : (EPOLLET | EPOLLIN | EPOLLERR);
		ee.data.u64 = 0; /* avoid valgrind warning */
		ee.data.ptr = s;
		/* Note, Kernel < 2.6.9 requires a non null event pointer even for EPOLL_CTL_DEL. */
		int rc = epoll_ctl(this->_epfd, EPOLL_CTL_MOD, s->fd(), &ee);
		CHECK_RETURN(rc == 0, false, "epoll_mod error: %d, %s, socket: %d", errno, strerror(errno), s);
		return true;
	}

	void Poll::shutdown() {
		SafeClose(this->_epfd); // try to wakeup this thread
	}

	void Poll::run(int milliseconds) {
		/* -1 to block indefinitely, 0 to return immediately, even if no events are available. */
		int numevents = ::epoll_wait(this->_epfd, this->_events, NM_POLL_EVENT, milliseconds);
		if (numevents < 0) {
			if (errno == EINTR) {
				return; // wake up by signal
			}
			CHECK_RETURN(false, void(0), "epoll wait error:%d, %s", errno, strerror(errno));
		}
		for (int i = 0; i < numevents; ++i) {
			struct epoll_event* ee = &this->_events[i];
			Socket* s = static_cast<Socket*>(ee.data.ptr);
			if (ee->events & (EPOLLERR | EPOLLHUP)) {
				Error.cout("fd: %d poll error or hup: %d", ee->data.fd, ee->events);
				//this->_slotWorker->errorSocket(ee->data.fd);
			}
			else if (ee->events & EPOLLRDHUP) {
				Error.cout("fd: %d poll error or rdhup: %d", ee->data.fd, ee->events);
				//this->_slotWorker->errorSocket(ee->data.fd);
			}
			else {
				bool rc = true;
				if (ee->events & EPOLLIN) {
					if (rc) {
						rc = s->read();
					}
				}
				if (ee->events & EPOLLOUT) {
					if (rc) {
						rc = s->write();
					}
				}
			}
		}
	}
}

