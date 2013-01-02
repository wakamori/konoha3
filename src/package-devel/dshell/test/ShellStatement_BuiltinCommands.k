// written by chen_ji

import("dshell");

void test() {
	String currentDir = $(pwd);
	cd ..;
	assert(currentDir != $(pwd));
}

test();
