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
#include <minikonoha/security.h>

extern kString *enforce_security;

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

extern kObjectVar **KONOHA_reftail(KonohaContext *kctx, size_t size);

static void val_reftrace(KonohaContext *kctx, KUtilsHashMapEntry *p)
{
}

/* ------------------------------------------------------------------------ */

static void kmodsecurity_setup(KonohaContext *kctx, struct KonohaModule *def, int newctx)
{
	(void)kctx;(void)def;(void)newctx;
}

static void kmodsecurity_reftrace(KonohaContext *kctx, struct KonohaModule *baseh)
{
	kmodsecurity_t *mod = (kmodsecurity_t *)baseh;
	BEGIN_REFTRACE(1);
	KREFTRACEv(mod->checkPermission);
	END_REFTRACE();
	KLIB Kmap_reftrace(kctx, mod->acl, val_reftrace);
}

static void kmodsecurity_free(KonohaContext *kctx, struct KonohaModule *baseh)
{
	kmodsecurity_t *mod = (kmodsecurity_t*)baseh;
	KLIB Kmap_free(kctx, mod->acl, NULL);
	KFREE(baseh, sizeof(kmodsecurity_t));
}

/* ------------------------------------------------------------------------ */

static void Security_init(KonohaContext *kctx, kObject *o, void *conf)
{
}

static void Security_free(KonohaContext *kctx, kObject *o)
{
}

static void Role_init(KonohaContext *kctx, kObject *o, void *conf)
{
	struct _kRole *role = (struct _kRole *)o;
	if(conf == NULL) {
		KINITv(role->name, KNULL(String));
	}
	else {
		KINITv(role->name, (kString *)conf);
	}
}

static void Role_free(KonohaContext *kctx, kObject *o)
{
}

static void Role_reftrace(KonohaContext *kctx, kObject *o)
{
	struct _kRole *role = (struct _kRole *)o;
	BEGIN_REFTRACE(1);
	KREFTRACEv(role->name);
	END_REFTRACE();
}

static kinline uintptr_t aclhash(KonohaContext *kctx, kString *name, kString *method)
{
	const char *name1 = S_text(name), *name2 = S_text(method);
	size_t len1 = S_size(name), len2 = S_size(method);
	uintptr_t i, hcode = 0;
	for(i = 0; i < len1; i++) {
		hcode = name1[i] + (31 * hcode);
	}
	hcode = ' ' + (31 * hcode);
	for(i = 0; i < len2; i++) {
		hcode = name2[i] + (31 * hcode);
	}
	return hcode;
}

static void acl_set(KonohaContext *kctx, kString *name, kString *method)
{
	uintptr_t hcode;
	if(method == NULL) {
		hcode = strhash(S_text(name), S_size(name));
	}
	else {
		hcode = aclhash(kctx, name, method);
	}
	KUtilsHashMap *map = kmodsecurity->acl;
	map_addu(kctx, map, hcode, 1);
}

static kbool_t acl_get(KonohaContext *kctx, kString *name, kString *method)
{
	uintptr_t hcode;
	if(method == NULL) {
		hcode = strhash(S_text(name), S_size(name));
	}
	else {
		hcode = aclhash(kctx, name, method);
	}
	KUtilsHashMap *map = kmodsecurity->acl;
	return (kbool_t)map_getu(kctx, map, hcode, 0);
}

/* ------------------------------------------------------------------------ */

//## void Security.addRole(Role role);
static KMETHOD Security_addRole(KonohaContext *kctx, KonohaStack *sfp)
{
	struct _kRole *role = (struct _kRole *)sfp[1].asObject;
	kString *name = role->name;
	if(acl_get(kctx, name, NULL)) {
		DBG_P("Role %s has already added.", S_text(name));
		RETURNvoid_();
	}
	else {
		acl_set(kctx, name, NULL);
	}
	RETURNvoid_();
}

//## Role String.toRole();
static KMETHOD String_toRole(KonohaContext *kctx, KonohaStack *sfp)
{
	kString *name = sfp[0].asString;
	DBG_P("String.toRole called, with name=%s", S_text(name));
	RETURN_(new_(Role, name));
}

//## String Role.toString();
static KMETHOD Role_toString(KonohaContext *kctx, KonohaStack *sfp)
{
	struct _kRole *role = (struct _kRole *)sfp[0].asObject;
	RETURN_(KLIB new_kString(kctx, S_text(role->name), S_size(role->name), SPOL_ASCII));
}

//## Role Role.new(String name);
static KMETHOD Role_new(KonohaContext *kctx, KonohaStack *sfp)
{
	kString *name = sfp[1].asString;
	DBG_P("Role.new called, with name=%s", S_text(name));
	RETURN_(new_(Role, name));
}

//## void Security.addPermission(Role role, String method);
static KMETHOD Security_addPermission(KonohaContext *kctx, KonohaStack *sfp)
{
	struct _kRole *role = (struct _kRole *)sfp[1].asObject;
	kString *method = sfp[2].asString;
	DBG_P("add permission %s to role %s", S_text(method), S_text(role->name));
	acl_set(kctx, role->name, method);
	RETURNvoid_();
}

//## Boolean Security.checkPermission(Role role, String method);
static KMETHOD Security_checkPermission(KonohaContext *kctx, KonohaStack *sfp)
{
	struct _kRole *role = (struct _kRole *)sfp[1].asObject;
	kString *method = sfp[2].asString;
	DBG_P("check permission %s of role %s", S_text(method), S_text(role->name));
	RETURNb_(acl_get(kctx, role->name, method));
}

/* ------------------------------------------------------------------------ */

#define _Public     kMethod_Public
#define _Const      kMethod_Const
#define _Coercion   kMethod_Coercion
#define _Im         kMethod_Immutable
#define _F(F)       (intptr_t)(F)

static kbool_t security_initPackage(KonohaContext *kctx, kNameSpace *ns, int argc, const char**args, kfileline_t pline)
{
	KUtilsKeyValue *kv = KLIB kNameSpace_getConstNULL(kctx, KNULL(NameSpace), SYM_("ROLE"));
	if(kv == NULL) {
		kv = KLIB kNameSpace_getConstNULL(kctx, ns, SYM_("ROLE"));
		if(kv == NULL) {
			kreportf(CritTag, pline, "global variable ROLE is undefined.");
			return false;
		}
	}
	kmodsecurity_t *base = (kmodsecurity_t*)KCALLOC(sizeof(kmodsecurity_t), 1);
	base->h.name     = "security";
	base->h.setup    = kmodsecurity_setup;
	base->h.reftrace = kmodsecurity_reftrace;
	base->h.free     = kmodsecurity_free;
	KINITv(base->acl, KLIB Kmap_init(kctx, 0));
	KDEFINE_CLASS SecurityDef = {
		STRUCTNAME(Security),
		.cflag = kClass_Final,
		.init = Security_init,
		.free = Security_free,
	};
	base->cSecurity = KLIB Konoha_defineClass(kctx, ns->packageId, ns->packageDomain, NULL, &SecurityDef, pline);
	KDEFINE_CLASS RoleDef = {
		STRUCTNAME(Role),
		.init     = Role_init,
		.free     = Role_free,
		.reftrace = Role_reftrace,
	};
	base->cRole = KLIB Konoha_defineClass(kctx, ns->packageId, ns->packageDomain, NULL, &RoleDef, pline);
	KLIB Konoha_setModule(kctx, MOD_security, &base->h, pline);

	intptr_t MethodData[] = {
		_Public, _F(String_toRole), TY_Role, TY_String, MN_to(TY_Role), 0,
		_Public, _F(Role_toString), TY_String, TY_Role, MN_to(TY_String), 0,
		_Public, _F(Security_addRole), TY_void, TY_Security, MN_("addRole"), 1, TY_Role, FN_("role"),
		//_Public, _F(Security_getRole), TY_Role, TY_Security, MN_("getRole"), 1, TY_String, FN_("name"),
		_Public, _F(Security_addPermission), TY_void, TY_Security, MN_("addPermission"), 2, TY_Role, FN_("role"), TY_String, FN_("method"),
		_Public, _F(Security_checkPermission), TY_Boolean, TY_Security, MN_("checkPermission"), 2, TY_Role, FN_("role"), TY_String, FN_("method"),
		_Public, _F(Role_new), TY_Role, TY_Role, MN_("new"), 1, TY_String, FN_("name"),
		DEND,
	};
	KLIB kNameSpace_loadMethodData(kctx, ns, MethodData);
	kMethod *mtd = KLIB kNameSpace_getMethodNULL(kctx, ns, TY_Security, MN_("checkPermission"), 0, MPOL_FIRST);
	DBG_ASSERT(mtd != NULL);
	KINITv(base->checkPermission, mtd);
	enforce_security = kv->stringValue;
	return true;
}

static kbool_t security_setupPackage(KonohaContext *kctx, kNameSpace *ns, isFirstTime_t isFirstTime, kfileline_t pline)
{
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
