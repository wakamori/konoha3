//Written by Joseph
K.import("konoha.bytes");
K.import("posix.socket");

void test() {
	int fd = System.socket(PF_INET, SOCK_STREAM, 0);
	p(System.setsockopt(fd, SO_REUSEADDR, 1));
	p(System.getsockopt(fd, SO_REUSEADDR));
	System.close(fd);
}
test();
