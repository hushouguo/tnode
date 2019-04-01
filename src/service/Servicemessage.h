/*
 * \file: Servicemessage.h
 * \brief: Created by hushouguo at 14:57:56 Mar 25 2019
 */
 
#ifndef __SERVICE_MESSAGE_H__
#define __SERVICE_MESSAGE_H__

BEGIN_NAMESPACE_TNODE {
	struct Servicemessage {
		int source;
		int type;
		int fd;
		int msgid;
		size_t len;
		u8 data[0];		
	};
}

#endif
