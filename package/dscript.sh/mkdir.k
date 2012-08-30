K.import("posix.process");
K.import("sugar");
K.import("konoha.null");

sugar "mkdir" $Expr

boolean tyCheckMkdir(Stmt stmt, Gamma gma) {
	Expr e = stmt.getExpr("$Expr", null);
	Token t = e.getTermToken();
	String dir = t.getText();
	p(dir);
	return true;
}

K.addTopStmtTyCheck("mkdir", tyCheckMkdir);
K.addStmtTyCheck("mkdir", tyCheckMkdir);

//Func for execution.

//System.mkdir("hoge", 509);

//System.mkdir("$Expr", 509/*mode*/);

mkdir "hoge";



