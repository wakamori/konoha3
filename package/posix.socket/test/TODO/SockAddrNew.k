//Written by Joseph

import("konoha.null");
import("konoha.new");
import("konoha.bytes");
import("posix.socket");

void test() {
	SockAddr s = new SockAddr();
	assert(s.isNotNull());
//	assert(SockAddr.new().isNotNull());//TODO
}

test();
