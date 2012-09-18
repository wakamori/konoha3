//Written by Joseph

K.import("posix.fd");
K.import("posix.process");

void test() {
	int fd = System.open("files/ftruncate.txt", O_WRONLY);
	assert(System.ftruncate(fd, 10));
	assert(System.ftruncate(fd, 30));
	fd = System.open("files/ftruncate.txt", O_RDWR);
	assert(System.ftruncate(fd, 20));
	fd = System.open("files/ftruncate.txt", O_RDONLY);
	assert(System.ftruncate(fd, 5) == false);
}

test();
