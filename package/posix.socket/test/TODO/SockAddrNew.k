//Written by Joseph
K.import("konoha.null");
K.import("konoha.new");
K.import("konoha.bytes");
K.import("posix.socket");

void test() {
	SockAddr s = new SockAddr();
	assert(s.isNotNull());
//	assert(SockAddr.new().isNotNull());//TODO
}
test();
