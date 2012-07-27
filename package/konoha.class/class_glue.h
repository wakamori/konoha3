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

#ifndef CLASS_GLUE_H_
#define CLASS_GLUE_H_

#include <minikonoha/minikonoha.h>
#include <minikonoha/sugar.h>

static KMETHOD MethodFunc_ObjectFieldGetter(KonohaContext *kctx, KonohaStack *sfp)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	RETURN_((sfp[0].asObject)->fieldObjectItems[delta]);
}
static KMETHOD MethodFunc_UnboxFieldGetter(KonohaContext *kctx, KonohaStack *sfp)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	RETURNd_((sfp[0].asObject)->fieldUnboxItems[delta]);
}
static KMETHOD MethodFunc_ObjectFieldSetter(KonohaContext *kctx, KonohaStack *sfp)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	KSETv((sfp[0].asObjectVar)->fieldObjectItems[delta], sfp[1].asObject);
	RETURN_(sfp[1].asObject);
}
static KMETHOD MethodFunc_UnboxFieldSetter(KonohaContext *kctx, KonohaStack *sfp)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	(sfp[0].asObjectVar)->fieldUnboxItems[delta] = sfp[1].unboxValue;
	RETURNd_(sfp[1].unboxValue);
}
static kMethod *new_FieldGetter(KonohaContext *kctx, ktype_t cid, ksymbol_t sym, ktype_t ty, int idx)
{
	kmethodn_t mn = MN_toGETTER(sym);
	MethodFunc f = (TY_isUnbox(ty)) ? MethodFunc_UnboxFieldGetter : MethodFunc_ObjectFieldGetter;
	kMethod *mtd = KLIB new_kMethod(kctx, kMethod_Public|kMethod_Immutable, cid, mn, f);
	KLIB Method_setParam(kctx, mtd, ty, 0, NULL);
	((kMethodVar*)mtd)->delta = idx;  // FIXME
	return mtd;
}

static kMethod *new_FieldSetter(KonohaContext *kctx, ktype_t cid, kmethodn_t sym, ktype_t ty, int idx)
{
	kmethodn_t mn = MN_toSETTER(sym);
	MethodFunc f = (TY_isUnbox(ty)) ? MethodFunc_UnboxFieldSetter : MethodFunc_ObjectFieldSetter;
	kparamtype_t p = {ty, FN_("x")};
	kMethod *mtd = KLIB new_kMethod(kctx, kMethod_Public, cid, mn, f);
	KLIB Method_setParam(kctx, mtd, ty, 1, &p);
	((kMethodVar*)mtd)->delta = idx;   // FIXME
	return mtd;
}

static intptr_t KLIB2_Method_indexOfField(kMethod *mtd)
{
	MethodFunc f = mtd->invokeMethodFunc;
	if(f== MethodFunc_ObjectFieldGetter || f == MethodFunc_UnboxFieldGetter || f == MethodFunc_ObjectFieldSetter || f == MethodFunc_UnboxFieldSetter) {
		return (intptr_t)mtd->delta;
	}
	return -1;
}

static KMETHOD MethodFunc_ObjectPrototypeGetter(KonohaContext *kctx, KonohaStack *sfp)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ksymbol_t key = (ksymbol_t)mtd->delta;
	RETURN_(KLIB kObject_getObject(kctx, sfp[0].asObject, key, sfp[K_RTNIDX].o));
}

static KMETHOD MethodFunc_UnboxPrototypeGetter(KonohaContext *kctx, KonohaStack *sfp)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ksymbol_t key = (ksymbol_t)mtd->delta;
	RETURNd_(KLIB kObject_getUnboxValue(kctx, sfp[0].asObject, key, 0));
}

static KMETHOD MethodFunc_ObjectPrototypeSetter(KonohaContext *kctx, KonohaStack *sfp)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ksymbol_t key = (ksymbol_t)mtd->delta;
	KLIB kObject_setObject(kctx, sfp[0].asObject, key, O_classId(sfp[1].asObject), sfp[1].asObject);
	RETURN_(sfp[1].asObject);
}

static KMETHOD MethodFunc_UnboxPrototypeSetter(KonohaContext *kctx, KonohaStack *sfp)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ksymbol_t key = (ksymbol_t)mtd->delta;
	kParam *pa = Method_param(mtd);
	KLIB kObject_setUnboxValue(kctx, sfp[0].asObject, key, pa->paramtypeItems[0].ty, sfp[1].unboxValue);
	RETURNd_(sfp[1].unboxValue);
}

static kMethod *new_PrototypeGetter(KonohaContext *kctx, ktype_t cid, ksymbol_t sym, ktype_t ty)
{
	kmethodn_t mn = MN_toGETTER(sym);
	MethodFunc f = (TY_isUnbox(ty)) ? MethodFunc_UnboxPrototypeGetter : MethodFunc_ObjectPrototypeGetter;
	kMethod *mtd = KLIB new_kMethod(kctx, kMethod_Public|kMethod_Immutable, cid, mn, f);
	KLIB Method_setParam(kctx, mtd, ty, 0, NULL);
	((kMethodVar*)mtd)->delta = sym;
	return mtd;
}

static kMethod *new_PrototypeSetter(KonohaContext *kctx, ktype_t cid, ksymbol_t sym, ktype_t ty)
{
	kmethodn_t mn = MN_toSETTER(sym);
	MethodFunc f = (TY_isUnbox(ty)) ? MethodFunc_UnboxPrototypeSetter : MethodFunc_ObjectPrototypeSetter;
	kparamtype_t p = {ty, FN_("x")};
	kMethod *mtd = KLIB new_kMethod(kctx, kMethod_Public, cid, mn, f);
	KLIB Method_setParam(kctx, mtd, ty, 1, &p);
	((kMethodVar*)mtd)->delta = sym;
	return mtd;
}

static void KonohaClass_addMethod(KonohaContext *kctx, KonohaClass *ct, kMethod *mtd)
{
	if(unlikely(ct->methodList == K_EMPTYARRAY)) {
		KINITv(((KonohaClassVar*)ct)->methodList, new_(MethodArray, 8));
	}
	KLIB kArray_add(kctx, ct->methodList, mtd);
}

static void KonohaClass_addField(KonohaContext *kctx, KonohaClassVar *definedClass, int flag, ktype_t ty, ksymbol_t sym)
{
	int pos = definedClass->fieldsize;
	if(pos < definedClass->fieldAllocSize) {
		definedClass->fieldsize += 1;
		definedClass->fieldItems[pos].flag = flag;
		definedClass->fieldItems[pos].ty = ty;
		definedClass->fieldItems[pos].fn = sym;
		if(TY_isUnbox(ty)) {
			definedClass->defaultValueAsNull_->fieldUnboxItems[pos] = 0;
		}
		else {
			KSETv(definedClass->defaultValueAsNull_->fieldObjectItems[pos], KLIB Knull(kctx, CT_(ty)));
			definedClass->fieldItems[pos].isobj = 1;
		}
		if(FLAG_is(definedClass->fieldItems[pos].flag, kField_Getter)) {
			FLAG_unset(definedClass->fieldItems[pos].flag, kField_Getter);
			kMethod *mtd = new_FieldGetter(kctx, definedClass->classId, sym, ty, pos);
			KonohaClass_addMethod(kctx, definedClass, mtd);
		}
		if(FLAG_is(definedClass->fieldItems[pos].flag, kField_Setter)) {
			FLAG_unset(definedClass->fieldItems[pos].flag, kField_Setter);
			kMethod *mtd = new_FieldSetter(kctx, definedClass->classId, sym, ty, pos);
			KonohaClass_addMethod(kctx, definedClass, mtd);
		}
	}
	else {
		kMethod *mtd = new_PrototypeGetter(kctx, definedClass->classId, sym, ty);
		KonohaClass_addMethod(kctx, definedClass, mtd);
		mtd = new_PrototypeSetter(kctx, definedClass->classId, sym, ty);
		KonohaClass_addMethod(kctx, definedClass, mtd);
	}
}

static kbool_t KonohaClass_setClassFieldObjectValue(KonohaContext *kctx, KonohaClassVar *definedClass, ksymbol_t sym, kObject *objectValue)
{
	int i;
	for(i = definedClass->fieldsize; i >= 0; i--) {
		if(definedClass->fieldItems[i].fn == sym  && O_ct(definedClass->defaultValueAsNull_->fieldObjectItems[i]) == O_ct(objectValue)) {
			KSETv(definedClass->defaultValueAsNull_->fieldObjectItems[i], objectValue);
			return true;
		}
	}
	return false;
}

static kbool_t KonohaClass_setClassFieldUnboxValue(KonohaContext *kctx, KonohaClassVar *definedClass, ksymbol_t sym, uintptr_t unboxValue)
{
	int i;
	for(i = definedClass->fieldsize; i >= 0; i--) {
		if(definedClass->fieldItems[i].fn == sym  && TY_isUnbox(definedClass->fieldItems[i].ty)) {
			definedClass->defaultValueAsNull_->fieldUnboxItems[i] = unboxValue;
			return true;
		}
	}
	return false;
}

//
//// --------------------------------------------------------------------------
//
//// int NameSpace.getCid(String name, int defval)
//static KMETHOD NameSpace_getCid(KonohaContext *kctx, KonohaStack *sfp)
//{
//	KonohaClass *ct = KLIB kNameSpace_getClass(kctx, sfp[0].asNameSpace, S_text(sfp[1].asString), S_size(sfp[1].asString), NULL);
//	kint_t cid = ct != NULL ? ct->classId : sfp[2].intValue;
//	RETURNi_(cid);
//}
//
//static void setfield(KonohaContext *kctx, KDEFINE_CLASS *ct, int fctsize, KonohaClass *supct)
//{
//	size_t fieldsize = supct->fieldsize + fctsize;
//	ct->cstruct_size = fctsize * sizeof(kObject*); //size64((fieldsize * sizeof(void*)) + sizeof(KonohaObjectHeader));
//	//DBG_P("supct->fieldsize=%d, fctsize=%d, cstruct_size=%d", supct->fieldsize, fctsize, ct->cstruct_size);
//	if(fieldsize > 0) {
//		ct->fieldItems = (KonohaClassField*)KCALLOC(fieldsize, sizeof(KonohaClassField));
//		ct->fieldsize = supct->fieldsize;
//		ct->fieldAllocSize = fieldsize;
//		if(supct->fieldsize > 0) {
//			memcpy(ct->fieldItems, supct->fieldItems, sizeof(KonohaClassField)*ct->fieldsize);
//		}
//	}
//}
//
//static KonohaClass* defineClass(KonohaContext *kctx, kNameSpace *ns, kshortflag_t cflag, kString *name, KonohaClass *supct, int fieldsize, kfileline_t pline)
//{
//	KDEFINE_CLASS defNewClass = {
//		.cflag  = cflag,
//		.classId    = TY_newid,
//		.baseclassId   = TY_Object,
//		.superclassId = supct->classId,
//	};
//	setfield(kctx, &defNewClass, fieldsize, supct);
//	KonohaClass *ct = KLIB Konoha_defineClass(kctx, ns->packageId, ns->packageDomain, name, &defNewClass, pline);
//	ct->fnull(kctx, ct);  // create null object
//	return ct;
//}
//
//// int NameSpace.defineClass(int flag, String name, int superclassId, int fieldsize);
//static KMETHOD NameSpace_defineClass(KonohaContext *kctx, KonohaStack *sfp)
//{
//	ktype_t superclassId = sfp[3].intValue == 0 ? TY_Object :(ktype_t)sfp[3].intValue;
//	KonohaClass *supct = kclass(superclassId, sfp[K_RTNIDX].uline);
//	if(CT_isFinal(supct)) {
//		kreportf(CritTag, sfp[K_RTNIDX].uline, "%s is final", TY_t(superclassId));
//	}
//	if(!CT_isDefined(supct)) {
//		kreportf(CritTag, sfp[K_RTNIDX].uline, "%s has undefined field(s)", TY_t(superclassId));
//	}
//	KonohaClass *ct = defineClass(kctx, sfp[0].asNameSpace, sfp[1].intValue, sfp[2].s, supct, sfp[4].intValue, sfp[K_RTNIDX].uline);
//	RETURNi_(ct->classId);
//}
//
//
//// int NameSpace.defineClassField(int cid, int flag, int ty, String name, Object *value);
//static KMETHOD NameSpace_defineClassField(KonohaContext *kctx, KonohaStack *sfp)
//{
//	ktype_t cid = (ktype_t)sfp[1].intValue;
//	kshortflag_t flag = (kshortflag_t)sfp[2].intValue;
//	ktype_t ty = (ktype_t)sfp[3].intValue;
//	kString *name = sfp[4].s;
//	kObject *value = sfp[5].o;
//	KonohaClassVar *ct = (KonohaClassVar*)kclass(cid, sfp[K_RTNIDX].uline);
//	if(CT_isDefined(ct)) {
//		kreportf(CritTag, sfp[K_RTNIDX].uline, "%s has no undefined field", TY_t(ct->classId));
//	}
//	KonohaClass_addField(kctx, ct, flag, ty, name, value, 0);
//	if(CT_isDefined(ct)) {
//		DBG_P("all fields are set");
//		KLIB2_setGetterSetter(kctx, ct);
//	}
//}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static	kbool_t class_initPackage(KonohaContext *kctx, kNameSpace *ns, int argc, const char**args, kfileline_t pline)
{
//	int FN_flag = FN_("flag"), FN_cid = FN_("cid"), FN_name = FN_("name"), FN_defval = FN_("defval");
//	KDEFINE_METHOD MethodData[] = {
//		_Public, _F(NameSpace_getCid), TY_Int, TY_NameSpace, MN_toGETTER(MN_("getCid")), 2, TY_String, FN_name, TY_Int, FN_defval,
//		_Public, _F(NameSpace_defineClass), TY_Int, TY_NameSpace, MN_("defineClass"), 4, TY_Int, FN_flag, TY_String, FN_name, TY_Int, FN_("superclassId"), TY_Int, FN_("fieldSize"),
//		_Public, _F(NameSpace_defineClassField), TY_Int, TY_NameSpace, MN_("defineClassField"), 5, TY_Int, FN_cid, TY_Int, FN_flag, TY_Int, FN_("type"), TY_String, FN_name, TY_Object, FN_defval,
//		DEND,
//	};
//	KLIB kNameSpace_loadMethodData(kctx, ns, MethodData);
	KSET_KLIB2(kMethod_indexOfField, KLIB2_Method_indexOfField, pline);
	return true;
}

static kbool_t class_setupPackage(KonohaContext *kctx, kNameSpace *ns, isFirstTime_t isFirstTime, kfileline_t pline)
{
	return true;
}

// --------------------------------------------------------------------------

static kExpr* NewExpr(KonohaContext *kctx, SugarSyntax *syn, kToken *tk, ktype_t ty)
{
	kExprVar *expr = GCSAFE_new(ExprVar, syn);
	KSETv(expr->termToken, tk);
	Expr_setTerm(expr, 1);
	expr->build = TEXPR_NEW;
	expr->ty = ty;
	return (kExpr*)expr;
}

static KMETHOD ParseExpr_new(KonohaContext *kctx, KonohaStack *sfp)
{
	VAR_ParseExpr(stmt, tokenArray, s, c, e);
	DBG_ASSERT(s == c);
	kTokenVar *tkNEW = tokenArray->tokenVarItems[s];
	if(s + 2 < kArray_size(tokenArray)) {
		kToken *tk1 = tokenArray->tokenItems[s+1];
		kToken *tk2 = tokenArray->tokenItems[s+2];
		KonohaClass *ct = CT_(Token_typeLiteral(tk1));
		if (ct->classId == TY_void) {
			RETURN_(SUGAR Stmt_p(kctx, stmt, tk1, ErrTag, "undefined class: %s", S_text(tk1->text)));
		} else if (CT_isForward(ct)) {
			SUGAR Stmt_p(kctx, stmt, NULL, ErrTag, "invalid application of 'new' to incomplete class %s", CT_t(ct));
		}

		if(Token_isVirtualTypeLiteral(tk1) && tk2->keyword == AST_PARENTHESIS) {  // new C (...)
			SugarSyntax *syn = SYN_(Stmt_nameSpace(stmt), KW_ExprMethodCall);
			kExpr *expr = SUGAR new_ConsExpr(kctx, syn, 2, tkNEW, NewExpr(kctx, syn, tk1, Token_typeLiteral(tk1)));
			tkNEW->keyword = MN_new;
			RETURN_(expr);
		}
		if(Token_isVirtualTypeLiteral(tk1) && tk2->keyword == AST_BRACKET) {     // new C [...]
			SugarSyntax *syn = SYN_(Stmt_nameSpace(stmt), KW_new);
			KonohaClass *ct = CT_p0(kctx, CT_Array, Token_typeLiteral(tk1));
			tkNEW->keyword = MN_("newArray");
			kExpr *expr = SUGAR new_ConsExpr(kctx, syn, 2, tkNEW, NewExpr(kctx, syn, tk1, ct->classId));
			RETURN_(expr);
		}
	}
}

static ksymbol_t tosymbolUM(KonohaContext *kctx, kToken *tk)
{
	DBG_ASSERT(tk->keyword == TK_SYMBOL);
	return ksymbolA(S_text(tk->text), S_size(tk->text), SYM_NEWID);
}

static KMETHOD ExprTyCheck_Getter(KonohaContext *kctx, KonohaStack *sfp)
{
	VAR_ExprTyCheck(stmt, expr, gma, reqty);
	kToken *tkN = expr->cons->tokenItems[0];
	ksymbol_t fn = tosymbolUM(kctx, tkN);
	kExpr *self = SUGAR kkStmt_tyCheckByNameAt(kctx, stmt, expr, 1, gma, TY_var, 0);
	kNameSpace *ns = Stmt_nameSpace(stmt);
	if(self != K_NULLEXPR) {
		kMethod *mtd = KLIB kNameSpace_getMethodNULL(kctx, ns, self->ty, MN_toGETTER(fn), 0, MPOL_GETTER);
		if(mtd != NULL) {
			KSETv(expr->cons->methodItems[0], mtd);
			RETURN_(SUGAR kStmt_tyCheckCallParamExpr(kctx, stmt, expr, mtd, gma, reqty));
		}
		SUGAR Stmt_p(kctx, stmt, tkN, ErrTag, "undefined field: %s", S_text(tkN->text));
	}
	RETURN_(K_NULLEXPR);
}

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
/* define class */

typedef struct {
	const char *key;
	uintptr_t ty;
	KonohaClass *ct;
} KDEFINE_CLASS_CONST;

static void ObjectField_init(KonohaContext *kctx, kObject *o, void *conf)
{
	KonohaClass *c = O_ct(o);
	DBG_ASSERT(c->defaultValueAsNull != NULL);
	size_t fieldsize = c->fieldsize;
	memcpy(((kObjectVar *)o)->fieldObjectItems, c->defaultValueAsNull->fieldObjectItems, fieldsize * sizeof(void*));
}

extern kObjectVar** KONOHA_reftail(KonohaContext *kctx, size_t size);

static void ObjectField_reftrace(KonohaContext *kctx, kObject *o)
{
	KonohaClass *c =O_ct(o);
	KonohaClassField *fieldItems = c->fieldItems;
	size_t i, fieldsize = c->fieldsize;
	BEGIN_REFTRACE(fieldsize);
	for (i = 0; i < fieldsize; i++) {
		if (fieldItems[i].isobj) {
			KREFTRACEn(o->fieldObjectItems[i]);
		}
	}
	END_REFTRACE();
}

static KonohaClassVar* kNameSpace_defineClassName(KonohaContext *kctx, kNameSpace *ns, kshortflag_t cflag, kString *name, KonohaClass *superClass, size_t declsize, kfileline_t pline)
{
	KDEFINE_CLASS defNewClass = {
		.cflag         = cflag,
		.classId       = TY_newid,
		.baseclassId   = TY_Object,
		.superclassId  = superClass->classId,
	};
	KonohaClassVar *definedClass = (KonohaClassVar*)KLIB Konoha_defineClass(kctx, ns->packageId, ns->packageDomain, name, &defNewClass, pline);
	KDEFINE_CLASS_CONST ClassData[] = {
		{S_text(name), TY_TYPE, definedClass},
		{NULL},
	};
	KLIB kNameSpace_loadConstData(kctx, ns, KonohaConst_(ClassData), 0);
	size_t fieldsize = superClass->fieldsize + declsize;
	definedClass->cstruct_size = size64(fieldsize * sizeof(kObject*) + sizeof(KonohaObjectHeader));
	DBG_P("superClass->fieldsize=%d, definedFieldSize=%d, cstruct_size=%d", superClass->fieldsize, declsize, definedClass->cstruct_size);
	if(fieldsize > 0) {
		definedClass->fieldItems = (KonohaClassField*)KCALLOC(fieldsize, sizeof(KonohaClassField));
		definedClass->fieldAllocSize = fieldsize;
		definedClass->fieldsize = superClass->fieldsize; /* supsize */
		if(superClass->fieldsize > 0) {
			memcpy(definedClass->fieldItems, superClass->fieldItems, sizeof(KonohaClassField) * superClass->fieldsize);
		}
	}
	definedClass->defaultValueAsNull_ = KCALLOC(definedClass->cstruct_size, sizeof(kObject*));
	superClass->fnull(kctx, superClass); // ensure default value of super class
	memcpy(definedClass->defaultValueAsNull_, superClass->defaultValueAsNull_, sizeof(kObject*) * superClass->fieldsize + sizeof(KonohaObjectHeader));
	definedClass->defaultValueAsNull_->h.ct = definedClass; // FIXME: proto
	definedClass->init     = ObjectField_init;
	definedClass->reftrace = ObjectField_reftrace;
	return (KonohaClassVar*)definedClass;
}

/* Block */

static kBlock* kStmt_parseClassBlockNULL(KonohaContext *kctx, kStmt *stmt, kToken *tokenClassName)
{
	kToken *tokenBlock = (kToken*)kStmt_getObject(kctx, stmt, KW_BlockPattern, NULL);
	if(tokenBlock != NULL && tokenBlock->keyword == TK_CODE) {
		const char *cname = S_text(tokenClassName->text);
		kArray *a = ctxsugar->preparedTokenList;
		size_t atop = kArray_size(a), s, i;
		SUGAR kNameSpace_tokenize(kctx, Stmt_nameSpace(stmt), S_text(tokenBlock->text), tokenBlock->uline, a);
		s = kArray_size(a);
		for(i = atop; i < s; i++) {
			kToken *tk = a->tokenItems[i];
			int topch = Token_topch(tk);
			if(topch == '(' && tokenBlock->keyword == TK_SYMBOL && strcmp(cname, S_text(tokenBlock->text)) == 0) {
				kTokenVar *tkNEW = GCSAFE_new(TokenVar, 0);
				tkNEW->keyword = TK_SYMBOL;
				KSETv(tkNEW->text, SYM_s(MN_new));
				tkNEW->uline = tokenBlock->uline;
				KLIB kArray_add(kctx, a, tkNEW);
			}
			KLIB kArray_add(kctx, a, tk);
			tokenBlock = tk;
		}
		kBlock *bk = SUGAR new_Block(kctx, Stmt_nameSpace(stmt), stmt, a, s, kArray_size(a), NULL);
		KLIB kObject_setObject(kctx, stmt, KW_BlockPattern, TY_Block, bk);
		KLIB kArray_clear(kctx, a, atop);
		return bk;
	}
	return NULL;
}

static size_t kBlock_countFieldSize(KonohaContext *kctx, kBlock *bk)
{
	size_t i, c = 0;
	if(bk != NULL) {
		for(i = 0; i < kArray_size(bk->stmtList); i++) {
			kStmt *stmt = bk->stmtList->stmtItems[i];
			DBG_P("stmt->keyword=%s%s", KW_t(stmt->syn->keyword));
			if(stmt->syn->keyword == KW_StmtTypeDecl) {
				kExpr *expr = SUGAR kStmt_getExpr(kctx, stmt, KW_ExprPattern, NULL);
				if(expr->syn->keyword == KW_COMMA) {
					c += (kArray_size(expr->cons) - 1);
				}
				else if(expr->syn->keyword == KW_LET || Expr_isTerm(expr)) {
					c++;
				}
			}
		}
	}
	return c;
}

static kbool_t kStmt_addClassField(KonohaContext *kctx, kStmt *stmt, kGamma *gma, KonohaClassVar *definedClass, kshortflag_t flag, ktype_t ty, kExpr *expr)
{
	if(Expr_isTerm(expr)) {
		kString *name = expr->termToken->text;
		ksymbol_t symbol = ksymbolA(S_text(name), S_size(name), SYM_NEWID);
		KonohaClass_addField(kctx, definedClass, flag, ty, symbol);
		return true;
	}
	else if(expr->syn->keyword == KW_LET) {
		kExpr *lexpr = kExpr_at(expr, 1);
		if(Expr_isTerm(lexpr)) {
			kString *name = lexpr->termToken->text;
			ksymbol_t symbol = ksymbolA(S_text(name), S_size(name), SYM_NEWID);
			kExpr *vexpr =  SUGAR kkStmt_tyCheckByNameAt(kctx, stmt, expr, 2, gma, ty, 0);
			if(vexpr == K_NULLEXPR) return false;
			if(vexpr->build == TEXPR_CONST) {
				KonohaClass_addField(kctx, definedClass, flag, ty, symbol);
				KonohaClass_setClassFieldObjectValue(kctx, definedClass, symbol, vexpr->objectConstValue);
			}
			else if(vexpr->build == TEXPR_NCONST) {
				KonohaClass_addField(kctx, definedClass, flag, ty, symbol);
				KonohaClass_setClassFieldUnboxValue(kctx, definedClass, symbol, vexpr->unboxConstValue);
			}
			else if(vexpr->build == TEXPR_NULL) {
				KonohaClass_addField(kctx, definedClass, flag, ty, symbol);
			}
			else {
				SUGAR Stmt_p(kctx, stmt, lexpr->termToken, ErrTag, "field initial value must be const: %s", S_text(name));
				return false;
			}
			return true;
		}
	} else if(expr->syn->keyword == KW_COMMA) {
		size_t i;
		for(i = 1; i < kArray_size(expr->cons); i++) {
			if(!kStmt_addClassField(kctx, stmt, gma, definedClass, flag, ty, kExpr_at(expr, i))) return false;
		}
		return true;
	}
	SUGAR Stmt_p(kctx, stmt, NULL, ErrTag, "field name is expected");
	return false;
}

static kbool_t kBlock_declClassField(KonohaContext *kctx, kBlock *bk, kGamma *gma, KonohaClassVar *ct)
{
	size_t i;
	kbool_t failedOnce = false;
	for(i = 0; i < kArray_size(bk->stmtList); i++) {
		kStmt *stmt = bk->stmtList->stmtItems[i];
		if(stmt->syn->keyword == KW_StmtTypeDecl) {
			kshortflag_t flag = kField_Getter | kField_Setter;
			kToken *tk  = SUGAR kStmt_getToken(kctx, stmt, KW_TypePattern, NULL);
			kExpr *expr = SUGAR kStmt_getExpr(kctx, stmt,  KW_ExprPattern, NULL);
			if(!kStmt_addClassField(kctx, stmt, gma, ct, flag, Token_typeLiteral(tk), expr)) {
				failedOnce = true;
			}
		}
	}
	return !(failedOnce);
}

static void kBlock_addMethodDeclStmt(KonohaContext *kctx, kBlock *bk, kToken *tokenClassName, kStmt **lastStmtRef)
{
	if(bk != NULL) {
		size_t i;
		for(i = 0; i < kArray_size(bk->stmtList); i++) {
			kStmt *stmt = bk->stmtList->stmtItems[i];
			if(stmt->syn->keyword == KW_StmtTypeDecl) continue;
			if(stmt->syn->keyword == KW_StmtMethodDecl) {
				kStmt *lastStmt = lastStmtRef[0];
				KLIB kObject_setObject(kctx, stmt, SYM_("type"), TY_Token, tokenClassName);
				SUGAR kBlock_insertAfter(kctx, lastStmt->parentBlockNULL, lastStmt, stmt);
				lastStmtRef[0] = stmt;
			}
			else {
				SUGAR Stmt_p(kctx, stmt, NULL, WarnTag, "%s is not available within the class clause", KW_t(stmt->syn->keyword));
			}
		}
	}
}

static inline size_t initFieldSizeOfVirtualClass(KonohaClass *superClass)
{
	return size64(sizeof(KonohaObjectHeader) + (superClass->fieldsize + 4) * sizeof(kObject*)) / sizeof(kObject*);
}

static KMETHOD StmtTyCheck_class(KonohaContext *kctx, KonohaStack *sfp)
{
	VAR_StmtTyCheck(stmt, gma);
//<<<<<<< HEAD
	kToken *tokenClassName = SUGAR kStmt_getToken(kctx, stmt, KW_UsymbolPattern, NULL);
	kNameSpace *ns = Stmt_nameSpace(stmt);
	KonohaClassVar *definedClass = (KonohaClassVar*)KLIB kNameSpace_getClass(kctx, ns, S_text(tokenClassName->text), S_size(tokenClassName->text), NULL);
	kBlock *bk = kStmt_parseClassBlockNULL(kctx, stmt, tokenClassName);
	size_t declsize = kBlock_countFieldSize(kctx, bk);
	if (definedClass != NULL) {
		if(declsize > 0 && !CT_isForward(definedClass)) {
			SUGAR Stmt_p(kctx, stmt, NULL, ErrTag, "%s has already defined", CT_t(definedClass));
//=======
//	kToken *tkC = SUGAR kStmt_getToken(kctx, stmt, KW_UsymbolPattern, NULL);
//	kToken *tkE = SUGAR kStmt_getToken(kctx, stmt, SYM_("extends"), NULL);
//	kshortflag_t cflag = 0;
//	ktype_t superclassId = TY_Object;
//	KonohaClass *supct = CT_Object;
//	kNameSpace *ns = Stmt_nameSpace(stmt);
//	if (tkE) {
//		//if(TK_isType(tkE)) {
//			superclassId = TK_type(tkE);
//			supct = CT_(superclassId);
//		//}
//		//else {
//		//	supct = KLIB kNameSpace_getClass(kctx, ns, S_text(tkE->text), S_size(tkE->text), NULL);
//		//	DBG_ASSERT(supct != NULL);
//		//	superclassId = supct->classId;
//		//}
//
//		if(CT_isFinal(supct)) {
//			SUGAR Stmt_p(kctx, stmt, NULL, ErrTag, "%s is final", CT_t(supct));
//			RETURNb_(false);
//		}
//		if(!CT_isDefined(supct)) {
//			SUGAR Stmt_p(kctx, stmt, NULL, ErrTag, "%s has undefined field(s)", CT_t(supct));
//>>>>>>> ae5daa82da8036b20e286ad09718511207fcd89f
			RETURNb_(false);
		}
	}
	else {
		kshortflag_t cflag = 0;
		KonohaClass *superClass = CT_Object;
		kToken *tokenSuperClass= SUGAR kStmt_getToken(kctx, stmt, SYM_("extends"), NULL);
		if (tokenSuperClass == NULL) {
			DBG_ASSERT(Token_isVirtualTypeLiteral(tokenSuperClass));
			superClass = CT_(Token_typeLiteral(tokenSuperClass));
			if(CT_isFinal(superClass)) {
				SUGAR Stmt_p(kctx, stmt, NULL, ErrTag, "%s is final", CT_t(superClass));
				RETURNb_(false);
			}
			if(CT_isForward(superClass)) {
				SUGAR Stmt_p(kctx, stmt, NULL, ErrTag, "%s is still virtual", CT_t(superClass));
				RETURNb_(false);
			}
		}
		size_t initsize = (bk != NULL) ? declsize : initFieldSizeOfVirtualClass(superClass);
		definedClass = kNameSpace_defineClassName(kctx, ns, cflag, tokenClassName->text, superClass, initsize, stmt->uline);
	}
	kToken_setVirtualTypeLiteral(kctx, tokenClassName, ns, definedClass->classId);
	if(declsize > 0) {
		if(!kBlock_declClassField(kctx, bk, gma, definedClass)) {
			RETURNb_(false);
		}
	}
	kBlock_addMethodDeclStmt(kctx, bk, tokenClassName, &stmt);
	kStmt_done(stmt);
	RETURNb_(true);
}

static kbool_t class_initNameSpace(KonohaContext *kctx,  kNameSpace *ns, kfileline_t pline)
{
	KDEFINE_SYNTAX SYNTAX[] = {
		{ .keyword = SYM_("new"), ParseExpr_(new), },
		{ .keyword = SYM_("class"), .rule = "\"class\" $USYMBOL [\"extends\" extends: $type] [$block]", TopStmtTyCheck_(class), },
		{ .keyword = SYM_("."), ExprTyCheck_(Getter) },
		{ .keyword = KW_END, },
	};
	SUGAR kNameSpace_defineSyntax(kctx, ns, SYNTAX);
	return true;
}

static kbool_t class_setupNameSpace(KonohaContext *kctx, kNameSpace *ns, kfileline_t pline)
{
	return true;
}

#endif /* CLASS_GLUE_H_ */
