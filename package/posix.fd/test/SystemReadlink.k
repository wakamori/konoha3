//Written by Joseph

K.import("konoha.bytes");
K.import("posix.fd");

void test() {
	assert(System.readlink("files/symlinked.txt") == "files/newfile.txt");
	assert(System.readlink("files/newfile.txt") == "");
	assert(System.readlink("files/notexist.txt") == "");
}

test();
