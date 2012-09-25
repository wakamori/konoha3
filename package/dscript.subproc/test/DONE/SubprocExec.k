//import("konoha.bytes");
import("dscript.subproc");

void f () {
	Subproc s = Subproc.new("", false);
	s.enableShellmode(false);
	assert(s.exec("echo $HOME") == "$HOME\n");
}

f();

