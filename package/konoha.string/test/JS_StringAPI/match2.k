//written by Joseph

K.import("konoha.string");
K.import("konoha.assign");

void test() {
	assert("The rain in SPAIN stays mainly in the plain".match(/ain/gi) == "ain,AIN,ain,ain");
}

test();
