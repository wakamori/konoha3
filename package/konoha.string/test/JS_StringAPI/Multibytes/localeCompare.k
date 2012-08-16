//written by Joseph

K.import("konoha.string");

void test() {
	assert("あ".localeCompare("あ") == 0);
	assert("あ".localeCompare("a") == 12257);
	assert("あ".localeCompare("0") == 12306);
	assert("か".localeCompare("kakiku") == 12256);
}

test();
