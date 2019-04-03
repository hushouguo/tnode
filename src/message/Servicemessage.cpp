/*
 * \file: Servicemessage.cpp
 * \brief: Created by hushouguo at 15:10:50 Apr 03 2019
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	Servicemessage* allocate_message(size_t payload_length) {
		return (Servicemessage *) malloc(payload_length + sizeof(Servicemessage));
	}

	void release_message(const Servicemessage* msg) {
		SafeFree(msg);
	}
}
