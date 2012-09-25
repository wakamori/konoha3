//Written by Joseph

import("konoha.null");
import("konoha.new");
import("konoha.array");
import("dscript.subproc");

void test() {
	Subproc a = new Subproc("echo \"hello\"", false);
	a.enablePipemodeOUT(true);
	assert(a.bg());
	String[] str1 = a.communicate("\"echo test1\"");
	System.p(str1.getSize());
	System.p(str1[0]);
	System.p(str1[1]);

	Subproc c = new Subproc("echo \"hi\"", true);
	c.enablePipemodeOUT(true);
	assert(c.bg());
	String[] str2 = c.communicate("echo \"test2\"");
	System.p(str2.getSize());
	System.p(str2[0]);
	System.p(str2[1]);

	Subproc b = new Subproc("", false);
	assert(b.fg() != 0);

	Subproc d = new Subproc("", true);
	assert(d.fg() != 0);

	Subproc e;
	assert(e.isNull());
}

test();
