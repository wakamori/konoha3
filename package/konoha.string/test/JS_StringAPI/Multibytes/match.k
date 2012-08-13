//written by Joseph

K.import("konoha.string");
K.import("konoha.regexp");

void test() {
	assert("世界、こんにちは、世界".match(/世界/g) == "世界,世界");
	assert("世界、こんにちは、世界".match(/せかい/gi).isNull());
	RegExp patt = /[あ-は]/gi;
	assert("こんにちは、世界abc123!@#".match(patt) == "'こ','に','ち','は'");
	assert("こんにちは、世界abc123!@#".match(/[0-ん]/g) == "'こ','ん','に','ち','は','、','a','b','c','1','2','3','@'");
}

test();
