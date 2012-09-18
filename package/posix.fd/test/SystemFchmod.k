//Written by Joseph

K.import("posix.fd");
K.import("posix.process");

void test() {
	int fd = System.open("files/another.txt", O_RDONLY);
	assert(System.fchmod(fd, 0750));
	assert(System.fchmod(fd, 0664));
}

test();
