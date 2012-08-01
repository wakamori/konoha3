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
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

//typedef const struct _kSecurity kSecurity;
//struct _kSecurity {
//	KonohaObjectHeader h;
//};
//
//typedef const struct _kRole kRole;
//struct _kRole {
//	KonohaObjectHeader h;
//};

/* ------------------------------------------------------------------------ */

//static void Security_init(KonohaContext *kctx, kObject *o, void *conf)
//{
//}
//
//
//static void Security_free(KonohaContext *kctx, kObject *o)
//{
//}
//
//static void Role_init(KonohaContext *kctx, kObject *o, void *conf)
//{
//}
//
//
//static void Role_free(KonohaContext *kctx, kObject *o)
//{
//}

/* ------------------------------------------------------------------------ */

////## void Security.addRole(Role role);
//static KMETHOD Security_addRole(KonohaContext *kctx, KonohaStack *sfp)
//{
//	struct _kSecurity *security = (struct _kSecurity *)sfp[0].asObject;
//	struct _kRole *role = (struct _kRole *)sfp[1].asObject;
//	asm("int3");
//	RETURNvoid_();
//}
//
////## Role Security.getRole(String name);
//static KMETHOD Role_setPermission(KonohaContext *kctx, KonohaStack *sfp)
//{
//	struct _kSecurity *security = (struct _kSecurity *)sfp[0].asObject;
//	struct _kRole *role = (struct _kRole *)sfp[0].asObject;
//	kString *method = sfp[1].asString;
//	asm("int3");
//	RETURNvoid_();
//}

//## Boolean Security.checkPermission(Role role, String method);
static KMETHOD Security_checkPermission(KonohaContext *kctx, KonohaStack *sfp)
{
	RETURNb_(true);
}

/* ------------------------------------------------------------------------ */

#define _Public     kMethod_Public
#define _Const      kMethod_Const
#define _Coercion   kMethod_Coercion
#define _Im         kMethod_Immutable
#define _F(F)       (intptr_t)(F)

//#define CT_Security cSecurity
#define TY_Security CT_Security->typeId
//#define CT_Role     cRole
#define TY_Role     CT_Role->typeId
static kbool_t security_initPackage(KonohaContext *kctx, kNameSpace *ns, int argc, const char**args, kfileline_t pline)
{
	//KDEFINE_CLASS SecurityDef = {
	//	STRUCTNAME(Security),
	//	.init = Security_init,
	//	.free = Security_free,
	//};
	//KDEFINE_CLASS RoleDef = {
	//	STRUCTNAME(Role),
	//	.init = Role_init,
	//	.free = Role_free,
	//};
	//KonohaClass *cSecurity = KLIB Konoha_defineClass(kctx, ns->packageId, ns->packageDomain, NULL, &SecurityDef, pline);
	//KonohaClass *cRole = KLIB Konoha_defineClass(kctx, ns->packageId, ns->packageDomain, NULL, &RoleDef, pline);

	return true;
}

static kbool_t security_setupPackage(KonohaContext *kctx, kNameSpace *ns, isFirstTime_t isFirstTime, kfileline_t pline)
{
	const char *security = "Security";
	const char *role = "Role";
	KonohaClass *CT_Security = KLIB kNameSpace_getClass(kctx, ns, security, strlen(security), NULL);
	KonohaClass *CT_Role = KLIB kNameSpace_getClass(kctx, ns, role, strlen(role), NULL);
	intptr_t MethodData[] = {
		//_Public, _F(Security_addRole), TY_void, TY_Security, MN_("addRole"), 1, TY_Role, FN_("role"),
		//_Public, _F(Security_getRole), TY_Role, TY_Security, MN_("getRole"), 1, TY_String, FN_("name"),
		_Public, _F(Security_checkPermission), TY_Boolean, TY_Security, MN_("checkPermission"), 2, TY_Role, FN_("role"), TY_String, FN_("method"),
		//_Public, _F(Role_new), TY_Role, TY_Role, MN_("new"), 1, TY_String, FN_("name"),
		DEND,
	};
	KLIB kNameSpace_loadMethodData(kctx, ns, MethodData);
	return true;
}

//static kBlock* kStmt_parseGrantBlockNULL(KonohaContext *kctx, kStmt *stmt, kToken *grantToken)
//{
//	kToken *blockToken = SUGAR kStmt_getToken(kctx, stmt, KW_BlockPattern, NULL);
//	if(blockToken != NULL && blockToken->resolvedSyntaxInfo->keyword == KW_BlockPattern) {
//		const char *name = S_text(grantToken->text);
//		kArray *a = KonohaContext_getSugarContext(kctx)->preparedTokenList;
//		size_t atop = kArray_size(a), s, i;
//		SUGAR kNameSpace_tokenize(kctx, Stmt_nameSpace(stmt), S_text(blockToken->text), blockToken->uline, a);
//		s = kArray_size(a);
//		kToken *prevToken = blockToken;
//		asm("int3");
//		for(i = atop; i < s; i++) {
//			kToken *tk = a->tokenItems[i];
//			/* TODO: accept syntax like 'permission role "method"' */
//			//if(strcmp(prevToken->text, "permission") == 0) {
//			//	kTokenVar *newToken = GCSAFE_new(TokenVar, TokenType_SYMBOL);
//			//}
//			KLIB kArray_add(kctx, a, tk);
//			prevToken = tk;
//		}
//		kBlock *bk = SUGAR new_Block(kctx, Stmt_nameSpace(stmt), stmt, a, s, kArray_size(a), NULL);
//		KLIB kObject_setObject(kctx, stmt, KW_BlockPattern, TY_Block, bk);
//		KLIB kArray_clear(kctx, a, atop);
//		return bk;
//	}
//	return NULL;
//}

//static KMETHOD StmtTyCheck_grant(KonohaContext *kctx, KonohaStack *sfp)
//{
//	VAR_StmtTyCheck(stmt, gma);
//	kToken *grantToken = SUGAR kStmt_getToken(kctx, stmt, KW_SymbolPattern, NULL);
//	kBlock *bk = kStmt_parseGrantBlockNULL(kctx, stmt, grantToken);
//	asm("int3");
//}

//static KMETHOD ParseExpr_permission(KonohaContext *kctx, KonohaStack *sfp)
//{
//	VAR_ParseExpr(stmt, tokenArray, beginIdx, currentIdx, endIdx);
//	DBG_ASSERT(beginIdx == currentIdx);
//	kTokenVar *permToken = tokenArray->tokenVarItems[beginIdx]; // permission MethodName
//	asm("int3");
//}

static kbool_t security_initNameSpace(KonohaContext *kctx,  kNameSpace *ns, kfileline_t pline)
{
	//KDEFINE_SYNTAX SYNTAX[] = {
	//	{ .keyword = SYM_("permission"), ParseExpr_(permission), .precedence_op1 = 300/*FIXME*/},
	//	{ .keyword = SYM_("grant"), .rule="\"grant\" $Symbol $Block", TopStmtTyCheck_(grant), },
	//	{ .keyword = KW_END, },
	//};
	//SUGAR kNameSpace_defineSyntax(kctx, ns, SYNTAX);
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

#ifdef __cplusplus
}
#endif
