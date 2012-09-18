//Written by Joseph
K.import("konoha.bytes");
K.import("posix.file");

void test() {
	assert(System.truncate("files/truncate.txt", 10) == 0);
	assert(System.truncate("files/truncate.txt", 30) == 0);
	assert(System.truncate("files/truncate.txt", -5) == -1);
	assert(System.truncate("files/notexist.txt", 5) == -1);
	assert(System.truncate("files/notexist.txt", -5) == -1);
}

test();
