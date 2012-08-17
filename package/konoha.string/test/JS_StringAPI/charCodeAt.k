//written by Joseph

K.import("konoha.string");
K.import("konoha.null");

void test() {
	assert("HELLO WORLD".charCodeAt(0) == 72);
	assert("HELLO WORLD".charCodeAt(999).isNull());
}

test();
