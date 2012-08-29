
K.import("sugar");
K.import("konoha.null");

// >>>echo "hi"
// >>>echo hi
// the result will be hi

sugar "echo" $Expr

boolean tyCheckEcho(Stmt stmt, Gamma gma) {
	stmt.newExpr
	p("hello");
	return stmt.tyCheckExpr("$Expr", gma, 5/*TY_String*/, 0);
}

K.addTopStmtTyCheck("echo", tyCheckEcho);
K.addStmtTyCheck("echo", tyCheckEcho);

void test() {
	echo "hi"
}
test();

echo "hellO";
