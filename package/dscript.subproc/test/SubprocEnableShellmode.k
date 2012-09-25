//Written by Joseph

import("konoha.new");
import("dscript.subproc");

void test() {
	Subproc s = new Subproc("echo \"hi\"", false);
	//before execution
	assert(s.isShellmode() == false);
	assert(s.enableShellmode(true));
	assert(s.isShellmode());
	assert(s.enableShellmode(false));
	assert(s.isShellmode() == false);
	s.fg();
	//after execution
	assert(s.enableShellmode(true) == false);
	assert(s.enableShellmode(false) == false);
	assert(s.isShellmode() == false);
}

test();
