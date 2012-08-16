//written by Joseph

K.import("konoha.string");

void test() {
	assert("こんにちは、世界、日本は?".split("、") == "'こんにちは','世界','日本は?'");
	assert("こんにちは、世界、日本は?".split() == "こんにちは、世界、日本は?");
	assert("こんにちは、世界、日本は?".split("は") == "'こんにち','、世界、日本','?'");
}

test();
