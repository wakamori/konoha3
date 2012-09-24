//Written by Joseph

import("konoha.new");
import("dscript.subproc");

void test() {
	Subproc s = new Subproc("", false);
	System.p(s.exec("echo \"hi\""));
	System.p(s.exec("echo \"hello\""));
}

test();
