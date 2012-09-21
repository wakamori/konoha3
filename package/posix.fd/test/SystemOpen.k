//Written by Joseph

import("konoha.int");
import("posix.fd");

void test() {
	assert(System.open("files/newfile.txt", O_RDONLY) > 0);
	assert(System.open("files/newfile.txt", O_WRONLY) > 0);
	assert(System.open("files/newfile.txt", O_RDWR) > 0);
//	assert(System.open("files/newfile.txt", O_CREAT) == -1);//TODO
//	assert(System.open("files/newfile.txt", O_CREAT, 0666) > 0);
//	assert(System.open("files/newfile.txt", O_CREAT|O_CREAT, 0777) > 0);
//	assert(System.open("files/newfile.txt", O_CREAT|O_CREAT|O_TRUNC, 0750) > 0);
	assert(System.open("files/newfile.txt", O_EXCL) == -1);
	assert(System.open("files/newfile.txt", O_TRUNC) == -1);
	assert(System.open("files/newfile.txt", O_APPEND) == -1);
	assert(System.open("files/newfile.txt", O_NONBLOCK|O_RDONLY) > 0);
	assert(System.open("files/newfile.txt", O_NDELAY|O_WRONLY) > 0);
	assert(System.open("files/newfile.txt", O_NOCTTY|O_RDWR) > 0);
}

test();
