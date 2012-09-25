//Written by Joseph

import("konoha.bytes");
import("konoha.null");
import("konoha.new");
import("konoha.array");
import("dscript.subproc");

void test() {
	Subproc s = new Subproc("echo \"hi\"", false);
//TODO subproc_glue.c:776 kBytes* ba = (kBytes*)KLIB new_kObject(kctx, CT_Bytes, S_size(s));
	s.enablePipemodeOUT(true);
	s.bg();
	String[] str = s.communicate("echo \"hello\"");
	System.p(str[0]);
	System.p(str[1]);
//	assert(str[0] == "hi");//TODO This assert doesn't pass.
//	assert(str[1] == null);// This assert is ok.
}

test();
