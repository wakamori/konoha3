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

#ifdef __cplusplus
extern "C"{
#endif

// --------------------------------------------------------------------------

static kbool_t const_initPackage(KonohaContext *kctx, kNameSpace *ns, int argc, const char**args, kfileline_t pline)
{
	return true;
}

static kbool_t const_setupPackage(KonohaContext *kctx, kNameSpace *ns, isFirstTime_t isFirstTime, kfileline_t pline)
{
	return true;
}

// --------------------------------------------------------------------------

static KMETHOD StmtTyCheck_ConstDecl(KonohaContext *kctx, KonohaStack *sfp)
{
	VAR_StmtTyCheck(stmt, gma);
	kNameSpace *ns = Stmt_nameSpace(stmt);
	kToken *SymbolToken = SUGAR kStmt_getToken(kctx, stmt, KW_SymbolPattern, NULL);
	ksymbol_t unboxKey = SymbolToken->resolvedSymbol;
	kbool_t result = SUGAR kStmt_tyCheckByName(kctx, stmt, KW_ExprPattern, gma, TY_var, TPOL_CONST);
	if(result) {
		kExpr *ConstExpr = SUGAR kStmt_getExpr(kctx, stmt, KW_ExprPattern, NULL);
		ktype_t type = ConstExpr->ty;
		uintptr_t unboxValue;
		result = false;
		if(ConstExpr->build == TEXPR_NULL) {   // const C = String
			type = TY_TYPE;
			unboxValue = (uintptr_t)(CT_(ConstExpr->ty));
			result = true;
		}
		else if(ConstExpr->build == TEXPR_CONST) {   // const C = "1"
			unboxValue = (uintptr_t)ConstExpr->objectConstValue;
			result = true;
		}
		else if(ConstExpr->build == TEXPR_NCONST) {  // const c = 1
			unboxValue = ConstExpr->unboxConstValue;
			result = true;
		}
		if(result) {
			result = KLIB kNameSpace_setConstData(kctx, ns, unboxKey, type, unboxValue, stmt->uline);
		}
		else {
			kStmt_printMessage(kctx, stmt, ErrTag, "constant value is expected: %s%s", PSYM_t(unboxKey));
		}
	}
	kStmt_done(kctx, stmt);
	RETURNb_(result);
}

static kbool_t const_initNameSpace(KonohaContext *kctx, kNameSpace *packageNameSpace, kNameSpace *ns, kfileline_t pline)
{
	KDEFINE_SYNTAX SYNTAX[] = {
		{ SYM_("const"), 0, "\"const\" $Symbol \"=\" $Expr", 0, 0, NULL, NULL, StmtTyCheck_ConstDecl, NULL, NULL, },
		{ KW_END, },
	};
	SUGAR kNameSpace_defineSyntax(kctx, ns, SYNTAX, packageNameSpace);
	return true;
}

static kbool_t const_setupNameSpace(KonohaContext *kctx, kNameSpace *packageNameSpace, kNameSpace *ns, kfileline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* const_init(void)
{
	static KDEFINE_PACKAGE d = {0};
	KSETPACKNAME(d, "const", "1.0");
	d.initPackage    = const_initPackage;
	d.setupPackage   = const_setupPackage;
	d.initNameSpace  = const_initNameSpace;
	d.setupNameSpace = const_setupNameSpace;
	return &d;
}

#ifdef __cplusplus
}
#endif
