
K.import("sugar");
K.import("konoha.null");

// >>>echo "hi"
//TODO >>>echo hi
// the result will be hi


sugar "echo" $Expr

boolean tyCheckEcho(Stmt stmt, Gamma gma) {
	Expr e = stmt.getExpr("$Expr", null);
	Token t = e.getTermToken();
	//Token tk = stmt.getToken("$Expr", null);
	//Token tk = stmt.getToken("$Expr", t);
	p(t.getText());//print hello
	//p(tk.getText());//print hello
	//return stmt.tyCheckExpr("$Expr", gma, 5/*TY_String*/, 0);
	return true;
}

K.addTopStmtTyCheck("echo", tyCheckEcho);
K.addStmtTyCheck("echo", tyCheckEcho);

//void test() {
//	echo "hi"
//}
//test();

echo "hellO";
