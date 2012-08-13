//written by Joseph

K.import("konoha.string");
K.import("konoha.assign");

void test() {
	assert("The rain in SPAIN stays mainly in the plain".match(/ain/g) == "ain,ain,ain");
}

test();
