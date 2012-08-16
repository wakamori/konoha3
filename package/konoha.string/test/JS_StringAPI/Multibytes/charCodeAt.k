//written by Joseph

K.import("konoha.string");

void test() {
	assert("こんにちは、世界".charCodeAt(0) == 12371);
	assert("こんにちは、世界".charCodeAt(999).isNull());
}

test();
