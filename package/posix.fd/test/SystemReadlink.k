//Written by Joseph

import("konoha.bytes");
import("posix.fd");

void test() {
	assert(System.readlink("files/readlink.txt") == "files/readlink.txt");
	assert(System.readlink("files/newfile.txt") == "");
	assert(System.readlink("files/notexist.txt") == "");
}

test();
