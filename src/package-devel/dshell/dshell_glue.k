/****************************************************************************
 * Copyright (c) 2012, the Konoha project authors. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

import("cstyle");
import("js4.array");
import("konoha.console");
import("konoha.ns");
import("konoha.eval");
import("konoha.file");
import("konoha.math");
import("konoha.object");
import("js4.string");
import("dscript.subproc");
import("konoha.desugar");

String[] concatTokenList(Token[] tokenList) {
	if(defined(DEBUG)) {
		System.p(tokenList);
	}
	String[] ret = [];
	String buf = "";
	for(int i = 0; i < tokenList.getlength(); i++) {
		Token tk = tokenList[i];
		if(tk.isBeforeWhiteSpace()) {
			ret.add(buf + tk.getText());
			buf = "";
		}
		else {
			buf = buf + tk.getText();
		}
	}
	if(buf != "") {
		ret.add(buf);
	}
	return ret;
}

String getVarName() {
	String rand = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	String ret = "__s";
	while(ret.getlength() < 10) {
		ret = ret + rand[(Math.random() * (rand.getlength() - 1)).toint()];
	}
	return ret;
}

String getFuncName() {
	String rand = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	String ret = "__f";
	while(ret.getlength() < 10) {
		ret = ret + rand[(Math.random() * (rand.getlength() - 1)).toint()];
	}
	return ret;
}

int indexOf(String[] a, String s, int startIdx) {
	for(int i = startIdx; i < a.getlength(); i++) {
		if(a[i] == s) {
			return i;
		}
	}
	return -1;
}

String expandDollar(String text) {
	if(text.startsWith("$(") && text.endsWith(")")) { /* shell expression */
		String[] args = text.substring(2, text.getlength() - 1).split(" ");
		String funcName = getFuncName();
		String evaltext = "@Public String " + funcName + "() { ";
		evaltext = evaltext + generateSubProcSequence(args, false).replace("\n", " ");
		if(defined(DEBUG)) {
			System.p(evaltext);
		}
		eval(evaltext);
		return funcName + "()";
	}
	else if(text.startsWith("${") && text.endsWith("}")) { /* shell variable */
		return text.substring(2, text.getlength() - 1) + ".toString()";
	}
	else if(text.startsWith("$")) { /* special variable */
		if(text == "$?") { /* environmental variable */
			return "getExitStatus().toString()";
		}
		return "System.getenv(\"" + text.substring(1, text.getlength()) + "\")";
	}
	else if(text.startsWith("\"") && text.endsWith("\"")) {
		return text;
	}
	return "\"" + text + "\"";
}

String expandArgs(String[] args) {
	String ret = "[";
	for(int i = 0; i < args.getlength() - 1; i++) {
		ret = ret + expandDollar(args[i]) + ", ";
	}
	if(args.getlength() >= 1) {
		ret = ret + expandDollar(args[args.getlength() - 1]);
	}
	ret = ret + "]";
	return ret;
}

boolean isUseTtyCommand(String cmd) {
	if(cmd == "less" || cmd == "man") {
		return true;
	}
	return false;
}

boolean isBuiltinCommand(String cmd) {
	if(cmd == "cd") {
		return true;
	}
	return false;
}

String getPipeString(String prevname, String varname, String[] args, int startindex, int argsize, boolean enableStdout, int index) {
	String ret = "";
	if(prevname != "" && !isUseTtyCommand(args[startindex])) {
		ret = ret + prevname + ".pipe(" + varname + ", false);\n";
	}
	if(isUseTtyCommand(args[startindex])) {
		if(prevname != "") {
			ret = ret + varname + ".bg();\n";
			if(defined(USE_PRINT)) {
				ret = ret + "System.p(";
			}
			ret = ret + varname + ".communicate(" + prevname + ".getOutputStream().readAll())";
			if(defined(USE_PRINT)) {
				ret = ret + "[0])";
			}
			ret = ret + ";\n";
		}
		else {
			if(enableStdout) {
				ret = ret + varname + ".fg();\n";
			}
			else {
				ret = ret + varname + ".bg();\n";
			}
		}
	}
	else {
		if(enableStdout) {
			if(defined(USE_PRINT) && index < 0) {
				/* use System.p instead of pipe to stdout */
				ret = ret + varname + ".bg();\n";
				ret = ret + "System.p(" + varname + ".communicate(\"\")[0]);\n";
			}
			else {
				ret = ret + varname + ".fg();\n";
			}
		}
		else {
			ret = ret + varname + ".bg();\n";
		}
	}
	ret = ret + "global.lastExitStatus = " + varname + ".getStatus();\n";
	return ret;
}

String generateSubProcSequence(String[] args, boolean enableStdout) {
	if(defined(DEBUG)) {
		System.p(args);
	}
	String ret = "";
	int startindex = 0;
	int index = 0;
	String varname = "";
	String prevname = "";
	int argsize = 0;
	int redirectindex = -1;
	String mode = "w";
	while(true) {
		/* pipe */
		index = indexOf(args, "|", startindex);
		if(index < 0) {
			redirectindex = indexOf(args, ">>", startindex);
			if(redirectindex >= 0) {
				mode = "a";
				argsize = redirectindex - startindex - 1;
			}
			else {
				redirectindex = indexOf(args, ">", startindex);
				if(redirectindex >= 0) {
					mode = "w";
					argsize = redirectindex - startindex - 1;
				}
				else {
					argsize = args.getlength() - startindex - 1;
				}
			}
		}
		else {
			argsize = index - startindex - 1;
		}
		varname = getVarName();
		if(isBuiltinCommand(args[startindex])) {
			if(args[startindex] == "cd" && argsize == 1) {
				ret = ret + "System.chdir(" + expandDollar(args[startindex + 1]) + ");\n";
			}
			if(index < 0) {
				break;
			}
		}
		else {
			ret = ret + "SubProc " + varname + " = new SubProc(\"" + args[startindex] + "\");\n";
			if(defined(SHOW_COMMAND)) {
				stdout.println(args[startindex]);
			}
			if(argsize > 0) {
				ret = ret + varname + ".setArgumentList(" + expandArgs(args.slice(startindex + 1, startindex + 1 + argsize)) + ");\n";
			}
			if(index >= 0) {
				ret = ret + getPipeString(prevname, varname, args, startindex, argsize, enableStdout, index);
			}
			else {
				if(redirectindex >= 0) {
					/* redirect */
					String filename = args[redirectindex + 1];
					ret = ret + varname + ".setOutputStream(new FILE(\"" + filename + "\", \"" + mode + "\"));\n";
				}
				else {
					/* end of pipe */
					if(enableStdout && !defined(USE_PRINT)) {
						ret = ret + varname + ".setOutputStream(stdout);\n";
					}
				}
				ret = ret + getPipeString(prevname, varname, args, startindex, argsize, enableStdout, index);
				break;
			}
			prevname = varname;
		}
		startindex = index + 1;
	}
	if(defined(DEBUG)) {
		System.p(ret);
	}
	if(!enableStdout) {
		ret = ret + "global.lastExitStatus = " + varname + ".getStatus(); ";
		ret = ret + "return " + varname + ".communicate(\"\")[0]; }";
	}
	return ret;
}

boolean statementShell(Stmt stmt, Gamma gma) {
	Token[] tokenList = stmt.getTokenList("$Token", null);
	if(tokenList.isNull()) {
		return false;
	}
	String[] args = concatTokenList(tokenList);
	Token[] subprocTokens = tokenize(generateSubProcSequence(args, true));
	Block block = stmt.newBlock(subprocTokens, 0, subprocTokens.getlength());
	boolean ret = block.TypeCheckAll(gma);
	if(!ret) {
		return false;
	}
	if(!defined(SHOW_COMMAND)) {
		stmt.setBlock("$Block", block);
	}
	stmt.setType(TSTMT_BLOCK);
	return ret;
}

int patternMatchShell(Stmt stmt, int nameid, Token[] tokenList, int beginIdx, int endIdx) {
	Token firstToken = tokenList[beginIdx];
	if(firstToken.Is("$Symbol") && SubProc.isCommand(firstToken.getText()) || isBuiltinCommand(firstToken.getText())) {
		if(firstToken.getText() == "Expr") {
			return -1;
		}
		for(int i = beginIdx + 1; i < endIdx; i++) {
			if(tokenList[i].is("$Indent") || tokenList[i].getText() == ";") {
				break;
			}
			if(tokenList[i].Is("()")) {
				return -1;
			}
		}
		return beginIdx;
	}
	else if(firstToken.getText() == "/") {
		String cmd = "";
		for(int i = beginIdx; i < endIdx - 1; i = i + 2) {
			if(tokenList[i].getText() != "/") {
				break;
			}
			cmd = cmd + "/" + tokenList[i + 1].getText();
		}
		if(SubProc.isCommand(cmd)) {
			return beginIdx;
		}
	}
	return -1;
}

Expr expressionAsk(Stmt stmt, Token[] tokenList, int beginIdx, int operatorIdx, int endIdx) {
	if(beginIdx + 1 < endIdx) {
		Token methodToken = new Token("inputUserApproval");
		Expr classExpr = new Expr(Console);
		Expr arg1 = new Expr(tokenList[beginIdx + 1].getText());
		Expr arg2 = new Expr("YES");
		Expr arg3 = new Expr("NO");
		Expr arg4 = new Expr(false);
		Expr expr = stmt.newUntypedCallStyleExpr(methodToken, classExpr, arg1, arg2, arg3, arg4);
		return expr;
	}
}

int getLastIndex(String source) {
	String endSymbol = " )}\n";
	for(int i = 0; i < source.getlength(); i++) {
		if(endSymbol.indexOf(source[i]) >= 0) {
			return i;
		}
	}
	return -1;
}

int tokenFuncDollarVariable(Token tk, String source) {
	int pos = 1;
	if(source.indexOf("{") == 1) {
		pos = source.indexOf("}");
		if(pos > 2) {
			tk.setText("$DollarVariable", source, 0, pos + 1);
		}
		return pos + 1;
	}
	else if(source.indexOf("(") == 1) {
		pos = source.indexOf(")");
		if(pos > 2) {
			tk.setText("$DollarVariable", source, 0, pos + 1);
		}
		return pos + 1;
	}
	pos = getLastIndex(source);
	if(pos < 0) {
		pos = source.getlength();
	}
	tk.setText("$DollarVariable", source, 0, pos);
	return pos;
}

Expr expressionDollarVariable(Stmt stmt, Token[] tokenList, int beginIdx, int operatorIdx, int endIdx) {
	if(defined(DEBUG)) {
		for(int i = beginIdx; i < endIdx; i++) {
			System.p(tokenList[i].getText());
			System.p(tokenList[i].getText().getlength());
			System.p(tokenList[i].is("$Symbol"));
			System.p(tokenList[i].is("()"));
		}
	}
	String text = tokenList[operatorIdx].getText();
	if(text.startsWith("$(") && text.endsWith(")")) {
		String[] args = text.substring(2, text.getlength() - 1).split(" ");
		String funcName = getFuncName();
		String evaltext = "@Public String " + funcName + "() { ";
		evaltext = evaltext + generateSubProcSequence(args, false).replace("\n", " ");
		if(defined(DEBUG)) {
			System.p(evaltext);
		}
		eval(evaltext);
		Token methodToken = new Token(funcName);
		Expr classExpr = new Expr(NameSpace);
		Expr expr = stmt.newUntypedCallStyleExpr(methodToken, classExpr);
		return expr;
	}
	else if(text.startsWith("${") && text.endsWith("}")) {
		Token[] tokens = Preprocess(text.substring(2, text.getlength() - 1));
		return stmt.newExpr(tokens, 0, tokens.getlength());
	}
	else if(text == "$?") {
		Token methodToken = new Token("getExitStatus");
		Expr classExpr = new Expr(NameSpace);
		Expr expr = stmt.newUntypedCallStyleExpr(methodToken, classExpr);
		return expr;
	}
	else {
		text = text.substring(1, text.getlength());
	}
	Token methodToken = new Token("getenv");
	Expr classExpr = new Expr(System);
	Expr arg1 = new Expr(text);
	Expr expr = stmt.newUntypedCallStyleExpr(methodToken, classExpr, arg1);
	return expr;
}

Expr typeCheckStringInterpolation(Stmt stmt, Expr expr, Gamma gma, int typeid) {
	Token tk = expr.getTermToken();
	String text = tk.getText();
	String beginText = String.fromCharCode(36) + String.fromCharCode(123)/*${*/;
	String endText = String.fromCharCode(125)/*}*/;
	String openBrace = String.fromCharCode(40)/*(*/;
	String closeBrace = String.fromCharCode(41)/*)*/;
	String concatText = String.fromCharCode(43)/*+*/;
	int start = text.indexOf(beginText);
	int end = -1;
	if(start < 0) {
		return null;
	}
	expr = new Expr(String);
	while(true) {
		start = text.indexOf(beginText);
		if(start < 0) {
			break;
		}
		end = text.indexOf(endText);
		if(end < 0) {
			break;
		}
		if(end - start > 2) {
			String buf = openBrace + text.substring(start + 2, end) + closeBrace;
			Token[] tokens = Preprocess(buf);
			Expr newexpr = stmt.newExpr(tokens, 0, tokens.getlength());
			if(start > 0) {
				Expr first = new Expr(text.substring(0, start));
				expr = stmt.newTypedCallExpr(gma, String, concatText, expr, first);
			}
			expr = stmt.newTypedCallExpr(gma, String, concatText, expr, newexpr);
			if(expr.isNull()) {
				return expr;
			}
		}
		text = text.substr(end + 1);
	}
	if(start < 0 || end < 0) {
		Expr rest = new Expr(text);
		expr = stmt.newTypedCallExpr(gma, String, concatText, expr, rest);
	}
	return expr;
}

AddPatternMatch("$Shell", patternMatchShell);
AddStatement("$Shell", statementShell);
AddTopLevelStatement("$Shell", statementShell);
AddExpression("ask", expressionAsk);
AddTypeCheck("$Text", typeCheckStringInterpolation);

syntax $Shell $Token*;

AddTokenizer("$DollarVariable", 36/*KonohaChar_Dollar*/, tokenFuncDollarVariable);
AddExpression("$DollarVariable", expressionDollarVariable);
