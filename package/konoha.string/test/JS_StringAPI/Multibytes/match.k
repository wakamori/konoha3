//written by Joseph

K.import("konoha.string");
K.import("konoha.regexp");

void test() {
	assert("世界、こんにちは、世界!".match(/世界/g) == "世界,世界");
}

test();
