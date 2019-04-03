/*
 * \file: Servicemessage.cpp
 * \brief: Created by hushouguo at 15:10:50 Apr 03 2019
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	Servicemessage* allocate_message(size_t size) {
		assert(size >= sizeof(Servicemessage));
		return (Servicemessage *) malloc(size);
	}

	void release_message(const Servicemessage* msg) {
		SafeDelete(msg);
	}
}
