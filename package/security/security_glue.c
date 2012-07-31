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

// --------------------------------------------------------------------------
//## Boolean Object.isNull();
static KMETHOD Object_isNull(KonohaContext *kctx, KonohaStack *sfp)
{
	kObject *o = sfp[0].asObject;
	RETURNb_(IS_NULL(o));
}

//## Boolean Object.isNotNull();
static KMETHOD Object_isNotNull(KonohaContext *kctx, KonohaStack *sfp)
{
	kObject *o = sfp[0].asObject;
	RETURNb_(!IS_NULL(o));
}

// --------------------------------------------------------------------------

#define _F(F) (intptr_t)(F)
static kbool_t security_initPackage(KonohaContext *kctx, kNameSpace *ns, int argc, const char**args, kfileline_t pline)
{
	//KDEFINE_CLASS SecurityDef = {
	//}
	//intptr_t MethodData[] = {
	//	DEND,
	//};
	//KLIB kNameSpace_loadMethodData(kctx, ns, MethodData);

	return true;
}

static kbool_t security_setupPackage(KonohaContext *kctx, kNameSpace *ns, isFirstTime_t isFirstTime, kfileline_t pline)
{
	return true;
}

static KMETHOD StmtTyCheck_grant(KonohaContext *kctx, KonohaStack *sfp)
{
	VAR_StmtTyCheck(stmt, gma);
	kToken *grantToken = SUGAR kStmt_getToken(kctx, stmt, KW_SymbolPattern, NULL);
	kToken *blockToken = SUGAR kStmt_getToken(kctx, stmt, KW_BlockPattern, NULL);
	if(blockToken != NULL && blockToken->resolvedSyntaxInfo->keyword == KW_BlockPattern) {
		asm("int3");
	}
	return NULL;
}

static KMETHOD ParseExpr_permission(KonohaContext *kctx, KonohaStack *sfp)
{
	VAR_ParseExpr(stmt, tokenArray, beginIdx, currentIdx, endIdx);
	DBG_ASSERT(beginIdx == currentIdx);
	kTokenVar *permToken = tokenArray->tokenVarItems[beginIdx]; // permission MethodName
	asm("int3");
}

static kbool_t security_initNameSpace(KonohaContext *kctx,  kNameSpace *ns, kfileline_t pline)
{
	KDEFINE_SYNTAX SYNTAX[] = {
		{ .keyword = SYM_("permission"), ParseExpr_(permission), .precedence_op1 = 300/*FIXME*/},
		{ .keyword = SYM_("grant"), .rule="\"grant\" $Symbol $Block", TopStmtTyCheck_(grant), },
		{ .keyword = KW_END, },
	};
	SUGAR kNameSpace_defineSyntax(kctx, ns, SYNTAX);
	return true;
}

static kbool_t security_setupNameSpace(KonohaContext *kctx, kNameSpace *ns, kfileline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* security_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("security", "0.1"),
		.initPackage    = security_initPackage,
		.setupPackage   = security_setupPackage,
		.initNameSpace  = security_initNameSpace,
		.setupNameSpace = security_setupNameSpace,
	};
	return &d;
}

