//Written by Joseph
//Modified by Wakamori

import("konoha.null");
import("konoha.new");
import("konoha.array");
import("dscript.subproc");

void test() {
	Subproc s = new Subproc("echo \"hi\" 3>&1 1>&2 2>&3", false);
	s.enableShellmode(true);
	//before execution
	assert(s.isERR2StdOUT() == false);
	assert(s.enableERR2StdOUT(true));//TODO Subproc.ERR2StdOUT(boolean) is not defined
	assert(s.isERR2StdOUT());
	assert(s.enableERR2StdOUT(false));
	assert(s.isERR2StdOUT() == false);
	s.bg();
	//after execution
	assert(s.enableERR2StdOUT(true) == false);
	assert(s.enableERR2StdOUT(false) == false);
	assert(s.isERR2StdOUT() == false);
}

test();
