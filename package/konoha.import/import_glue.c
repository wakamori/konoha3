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

#include <minikonoha/minikonoha.h>
#include <minikonoha/sugar.h>
#include <stdio.h>

static KMETHOD StmtTyCheck_import(KonohaContext *kctx, KonohaStack *sfp _RIX)
{
	USING_SUGAR;
	int ret = false;
	VAR_StmtTyCheck(stmt, gma);
	kTokenArray *tls = (kTokenArray *) kObject_getObjectNULL(stmt, KW_ToksPattern);
	if (tls == NULL) {
		RETURNb_(false);
	}
	KUtilsWriteBuffer wb;
	kwb_init(&(kctx->stack->cwb), &wb);
	int i = 0;
	if (i + 2 < kArray_size(tls)) {
		for (; i < kArray_size(tls)-1; i+=2) {
			/* name . */
			kToken *tk  = tls->toks[i+0];
			kToken *dot = tls->toks[i+1];
			assert(tk->keyword  == TK_SYMBOL);
			assert(dot->keyword == KW_DOT);
			kwb_write(&wb, S_text(tk->text), S_size(tk->text));
			kwb_putc(&wb, '.');
		}
	}
	kString *name = tls->toks[i]->text;
	kwb_write(&wb, S_text(name), S_size(name));
	kString *pkgname = new_kString(KUtilsWriteBufferop(&wb, 1), kwb_bytesize(&wb), 0);
	kNameSpace *ns = gma->genv->ns;
	SugarSyntaxVar *syn1 = (SugarSyntaxVar*) SYN_(ns, KW_ExprMethodCall);
	kTokenVar *tkImport = new_Var(Token, 0);
	kExpr *ePKG = new_ConstValue(TY_String, pkgname);
	tkImport->keyword = MN_("import");
	kExpr *expr = SUGAR new_ConsExpr(kctx, syn1, 3,
			tkImport, new_ConstValue(O_cid(ns), ns), ePKG);
	kObject_setObject(stmt, KW_ExprPattern, expr);
	ret = SUGAR Stmt_tyCheckExpr(kctx, stmt, KW_ExprPattern, gma, TY_Boolean, 0);
	if (ret) {
		kStmt_typed(stmt, EXPR);
	}
	RETURNb_(ret);
}

// --------------------------------------------------------------------------

static kbool_t import_initPackage(KonohaContext *kctx, kNameSpace *ns, int argc, const char**args, kfileline_t pline)
{
	return true;
}

static kbool_t import_setupPackage(KonohaContext *kctx, kNameSpace *ns, kfileline_t pline)
{
	return true;
}

static kbool_t import_initNameSpace(KonohaContext *kctx, kNameSpace *ns, kfileline_t pline)
{
	USING_SUGAR;
	KDEFINE_SYNTAX SYNTAX[] = {
		{ .keyword = SYM_("import"), .rule = "\"import\" $toks", TopStmtTyCheck_(import)},
		{ .keyword = KW_END, },
	};
	SUGAR NameSpace_defineSyntax(kctx, ns, SYNTAX);
	return true;
}

static kbool_t import_setupNameSpace(KonohaContext *kctx, kNameSpace *ns, kfileline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* import_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("import", "1.0"),
		.initPackage = import_initPackage,
		.setupPackage = import_setupPackage,
		.initNameSpace = import_initNameSpace,
		.setupNameSpace = import_setupNameSpace,
	};
	return &d;
}
