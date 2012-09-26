//Written by Joseph

import("konoha.new");
import("dscript.subproc");

void test() {
	Subproc s = new Subproc("echo \"hi\"", false);
	//before execution
	assert(s.isERR2StdOUT() == false);
	assert(s.enableERR2StdOUT(true));//TODO Subproc.ERR2StdOUT(boolean) is not defined
	assert(s.isERR2StdOUT());
	assert(s.enableERR2StdOUT(false));
	assert(s.isERR2StdOUT() == false);
	s.fg();
	//after execution
	assert(s.enableERR2StdOUT(true) == false);
	assert(s.enableERR2StdOUT(false) == false);
	assert(s.isERR2StdOUT() == false);
}

test();
