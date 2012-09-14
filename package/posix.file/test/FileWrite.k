//Written by Joseph
K.import("konoha.bytes");
K.import("konoha.string");
K.import("posix.file");

void test() {
	FILE f = System.fopen("filewrite.txt", "w");
	String str = "1234567890abcde\n";	//the length of str is 16
	int len = str.getSize();
	assert(f.write(str.asBytes(), 0, len) == 16);
	f.close();
}

test();
