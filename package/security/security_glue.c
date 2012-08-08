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

extern kRole *enforce_security;

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

extern kObjectVar **KONOHA_reftail(KonohaContext *kctx, size_t size);

static void val_reftrace(KonohaContext *kctx, KUtilsHashMapEntry *p)
{
	BEGIN_REFTRACE(1);
	KREFTRACEv(p->objectValue);
	END_REFTRACE();
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
	KLIB Kmap_reftrace(kctx, mod->roles, val_reftrace);
}

static void kmodsecurity_free(KonohaContext *kctx, struct KonohaModule *baseh)
{
	kmodsecurity_t *mod = (kmodsecurity_t*)baseh;
	KLIB Kmap_free(kctx, mod->acl, NULL);
	KLIB Kmap_free(kctx, mod->roles, NULL);
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
	KINITv(role->parent, K_NULL);
}

static void Role_free(KonohaContext *kctx, kObject *o)
{
}

static void Role_reftrace(KonohaContext *kctx, kObject *o)
{
	struct _kRole *role = (struct _kRole *)o;
	BEGIN_REFTRACE(2);
	KREFTRACEv(role->name);
	KREFTRACEv(role->parent);
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

static kRole *roles_get(KonohaContext *kctx, kString *name)
{
	uintptr_t hcode = strhash(S_text(name), S_size(name));
	KUtilsHashMap *map = kmodsecurity->roles;
	KUtilsHashMapEntry *e = KLIB Kmap_get(kctx, map, hcode);
	if (e) {
		return (kRole *)e->unboxValue;
	} else {
		return (kRole *)K_NULL;
	}
}

static kbool_t roles_set(KonohaContext *kctx, kString *name, kRole *role)
{
	uintptr_t hcode = strhash(S_text(name), S_size(name));
	KUtilsHashMap *map = kmodsecurity->roles;
	if(roles_get(kctx, name) != (kRole *)K_NULL) {
		DBG_P("Role %s is already defined", S_text(name));
		return false;
	}
	KUtilsHashMapEntry *newe = KLIB Kmap_newEntry(kctx, map, hcode);
	newe->unboxValue = (uintptr_t)role;
	return true;
}

/* ------------------------------------------------------------------------ */

//## void Security.addRole(Role role);
static KMETHOD Security_addRole(KonohaContext *kctx, KonohaStack *sfp)
{
	kRole *role = (kRole *)sfp[1].asObject;
	kString *name = role->name;
	roles_set(kctx, name, role);
	RETURNvoid_();
}

//## Role Security.getRole(String name);
static KMETHOD Security_getRole(KonohaContext *kctx, KonohaStack *sfp)
{
	kString *name = sfp[1].asString;
	RETURN_(roles_get(kctx, name));
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

//## Role Role.new(String name, Role parent);
static KMETHOD Role_newwithParent(KonohaContext *kctx, KonohaStack *sfp)
{
	kString *name = sfp[1].asString;
	kRole *parent = (kRole *)sfp[2].asObject;
	kString *pname = parent->name;
	DBG_P("Role.new called, with name=%s, parent=%s", S_text(name), S_text(pname));
	struct _kRole *role = (struct _kRole *)KLIB new_kObject(kctx, CT_Role, (uintptr_t)name);
	KSETv(role, role->parent, parent);
	RETURN_(role);
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
	kRole *role = (kRole *)sfp[1].asObject;
	kString *method = sfp[2].asString;
	while(role != (kRole *)K_NULL) {
		DBG_P("check permission %s of role %s", S_text(method), S_text(role->name));
		if(acl_get(kctx, role->name, method)) {
			RETURNb_(true);
		}
		role = role->parent;
	}
	RETURNb_(false);
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
	KINITv(base->roles, KLIB Kmap_init(kctx, 0));
	KDEFINE_CLASS SecurityDef = {
		STRUCTNAME(Security),
		.cflag    = kClass_Final,
		.init     = Security_init,
		.free     = Security_free,
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
		_Public, _F(Security_getRole), TY_Role, TY_Security, MN_("getRole"), 1, TY_String, FN_("name"),
		_Public, _F(Security_addPermission), TY_void, TY_Security, MN_("addPermission"), 2, TY_Role, FN_("role"), TY_String, FN_("method"),
		_Public, _F(Security_checkPermission), TY_Boolean, TY_Security, MN_("checkPermission"), 2, TY_Role, FN_("role"), TY_String, FN_("method"),
		_Public, _F(Role_new), TY_Role, TY_Role, MN_("new"), 1, TY_String, FN_("name"),
		_Public, _F(Role_newwithParent), TY_Role, TY_Role, MN_("new"), 2, TY_String, FN_("name"), TY_Role, FN_("parent"),
		DEND,
	};
	KLIB kNameSpace_loadMethodData(kctx, ns, MethodData);
	kMethod *mtd = KLIB kNameSpace_getMethodNULL(kctx, ns, TY_Security, MN_("checkPermission"), 0, MPOL_FIRST);
	DBG_ASSERT(mtd != NULL);
	KINITv(base->checkPermission, mtd);
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

static kRole *getRole(KonohaContext *kctx, kNameSpace *ns) {
	KUtilsKeyValue *kv = KLIB kNameSpace_getConstNULL(kctx, KNULL(NameSpace), SYM_("ROLE"));
	kString *rolename = kv->stringValue;
	INIT_GCSTACK();
	/* call Security.getRole(ROLE) */
	BEGIN_LOCAL(lsfp, K_CALLDELTA + 1);
	KSETv_AND_WRITE_BARRIER(NULL, lsfp[K_CALLDELTA+0].o, KLIB Knull(kctx, CT_Security), GC_NO_WRITE_BARRIER);
	KSETv_AND_WRITE_BARRIER(NULL, lsfp[K_CALLDELTA+1].asString, rolename, GC_NO_WRITE_BARRIER);
	kMethod *getrole = KLIB kNameSpace_getMethodNULL(kctx, ns, TY_Security, MN_("getRole"), 0, MPOL_FIRST);
	DBG_ASSERT(getrole != NULL);
	KCALL(lsfp, 0, getrole, 1, KNULL(Role));
	END_LOCAL();
	RESET_GCSTACK();
	return (kRole *)lsfp[0].o;
}

static kbool_t security_setupNameSpace(KonohaContext *kctx, kNameSpace *ns, kfileline_t pline)
{
	char pathbuf[256];
	const char *policypath = PLATAPI formatPackagePath(pathbuf, sizeof(pathbuf), "security", "_policy.k");
	kMethod *mtd = KLIB kNameSpace_getMethodNULL(kctx, ns, TY_NameSpace, MN_("load"), 0, MPOL_FIRST);
	INIT_GCSTACK();
	BEGIN_LOCAL(lsfp, K_CALLDELTA + 2);
	KSETv_AND_WRITE_BARRIER(NULL, lsfp[K_CALLDELTA+0].o, (kObject *)ns, GC_NO_WRITE_BARRIER);
	KSETv_AND_WRITE_BARRIER(NULL, lsfp[K_CALLDELTA+1].s, KLIB new_kString(kctx, policypath, strlen(policypath), 0), GC_NO_WRITE_BARRIER);
	KCALL(lsfp, 0, mtd, 2, KNULL(Boolean));
	END_LOCAL();
	RESET_GCSTACK();
	/* FIXME: set enforce_security after loading security_policy.k */
	if(ns == KNULL(NameSpace)) {
		enforce_security = getRole(kctx, ns);
	}
	return lsfp[0].boolValue;
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
