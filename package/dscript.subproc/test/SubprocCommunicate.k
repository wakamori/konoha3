//Written by Joseph

import("konoha.null");
import("konoha.new");
import("konoha.array");
import("dscript.subproc");

void test() {
	Subproc s = new Subproc("echo \"hi\"", false);
	s.enablePipemodeOUT(true);
	s.bg();
	String[] str = s.communicate("echo \"hello\"");
	assert(str[0] == "\"hi\"\n");
	assert(str[1] == null);
}

test();
