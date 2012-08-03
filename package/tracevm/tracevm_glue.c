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

#include<minikonoha/minikonoha.h>
#include<minikonoha/sugar.h>
#include<minikonoha/tracevm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

static void kmodtrace_setup(KonohaContext *kctx, struct KonohaModule *def, int newctx)
{
	(void)kctx;
	(void)def;
	(void)newctx;
}

extern kObjectVar **KONOHA_reftail(KonohaContext *kctx, size_t size);

static void kmodtrace_reftrace(KonohaContext *kctx, struct KonohaModule*baseh)
{
	kmodtrace_t *mod = (kmodtrace_t *)baseh;
	BEGIN_REFTRACE(2);
	KREFTRACEv(mod->before);
	KREFTRACEv(mod->after);
	END_REFTRACE();
}

static void kmodtrace_free(KonohaContext *kctx, struct KonohaModule *baseh)
{
	KFREE(baseh, sizeof(kmodtrace_t));
}

static void Trace_init(KonohaContext *kctx, kObject *o, void *conf)
{
}

static void Trace_free(KonohaContext *kctx, kObject *o)
{
}

#define TRACE_DELTA K_CALLDELTA + K_CALLDELTA

static int beforeTrace(KonohaContext *kctx, KonohaStack *sfp, kfileline_t pline)
{
	kMethod *trace = sfp[K_MTDIDX].mtdNC;
	kParam *pa = Method_param(trace);
	int argc = pa->psize + 1;
	kMethod *mtd = kmodtrace->before;
	DBG_ASSERT(mtd != NULL);
	INIT_GCSTACK();
	BEGIN_LOCAL(lsfp, TRACE_DELTA + 3);
	KSETv(lsfp[TRACE_DELTA+0].asObject, KLIB Knull(kctx, CT_Trace));
	KUtilsWriteBuffer wb;
	KLIB Kwb_init(&(kctx->stack->cwb), &wb);
	KLIB Kwb_printf(kctx, &wb, "%s.%s%s", Method_t(trace));
	KSETv(lsfp[TRACE_DELTA+1].s, KLIB new_kString(kctx, KLIB Kwb_top(kctx, &wb, 0), Kwb_bytesize(&wb), SPOL_POOL));
	KLIB Kwb_free(&wb);
	kArray *a = new_(Array, argc);
	size_t i;
	for(i = 0; i < pa->psize; i++) {
		ktype_t paramType = pa->paramtypeItems[i].ty;
		kObject *o = KLIB new_kObject(kctx, CT_(paramType), sfp[i+1].unboxValue);
		KLIB kArray_add(kctx, a, o);/*FIXME: correct type*/
	}
	KSETv(lsfp[TRACE_DELTA+2].asArray, a);
	KCALL(lsfp + K_CALLDELTA, 0, mtd, 3, KNULL(Boolean));
	END_LOCAL();
	RESET_GCSTACK();
	return lsfp[K_CALLDELTA].boolValue == true ? 0 : -1;
}

static int afterTrace(KonohaContext *kctx, KonohaStack *sfp, kfileline_t pline)
{
//	kMethod *mtd = kmodtrace->after;
//	DBG_ASSERT(mtd != NULL);
//	INIT_GCSTACK();
//	BEGIN_LOCAL(lsfp, TRACE_DELTA + 4);
//	KSETv(lsfp[TRACE_DELTA+0].asObject, KLIB Knull(kctx, CT_Trace));
//	KUtilsWriteBuffer wb;
//	KLIB Kwb_init(&(kctx->stack->cwb), &wb);
//	KLIB Kwb_printf(kctx, &wb, "%s.%s%s", Method_t(sfp[K_MTDIDX2-K_CALLDELTA].mtdNC));
//	KSETv(lsfp[TRACE_DELTA+1].s, KLIB new_kString(kctx, KLIB Kwb_top(kctx, &wb, 0), Kwb_bytesize(&wb), SPOL_POOL));
//	KLIB Kwb_free(&wb);
//	if(Method_returnType(mtd) == TY_void) {
//		KSETv(lsfp[TRACE_DELTA+2].asObject, K_NULL);
//	}
//	else {
//		KSETv(lsfp[TRACE_DELTA+2].asObject, sfp[K_RTNIDX].asObject);
//	}
//	kArray *a = new_(Array, argc);
//	size_t i;
//	kParam *pa = Method_param(sfp[K_MTDIDX2-K_CALLDELTA].mtdNC);
//	for(i = 0; i < pa->psize; i++) {
//		ktype_t paramType = pa->paramtypeItems[i].ty;
//		kObject *o = KLIB new_kObject(kctx, CT_(paramType), sfp[i+1].unboxValue);
//		KLIB kArray_add(kctx, a, o);/*FIXME: incorrect type*/
//	}
//	KSETv(lsfp[TRACE_DELTA+3].asArray, a);
//	KCALL(lsfp + K_CALLDELTA, 0, mtd, 4, KNULL(Boolean));
//	END_LOCAL();
//	RESET_GCSTACK();
	return 0;
}

static KMETHOD DEFAULT_Before(KonohaContext *kctx, KonohaStack *sfp)
{
	DBG_P("default before");
}

static KMETHOD DEFAULT_After(KonohaContext *kctx, KonohaStack *sfp)
{
	DBG_P("default after");
}

/* ------------------------------------------------------------------------ */

//## void Trace.setBefore(Func[Boolean,String,Object[]] beforefunc);
static KMETHOD Trace_setBefore(KonohaContext *kctx, KonohaStack *sfp)
{
	kFunc *fo = sfp[1].asFunc;
	KSETv(kmodtrace->before, fo->mtd);
	RETURNvoid_();
}

////## void Trace.setAfter(Func[Boolean,String,Object,Object[]] afterfunc);
//static KMETHOD Trace_setAfter(KonohaContext *kctx, KonohaStack *sfp)
//{
//	kFunc *fo = sfp[1].asFunc;
//	KSETv(kmodtrace->after, fo->mtd);
//	RETURNvoid_();
//}

/* ------------------------------------------------------------------------ */

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Im       kMethod_Immutable
#define _Coercion kMethod_Coercion
#define _Static   kMethod_Static
#define _F(F)   (intptr_t)(F)

static kbool_t tracevm_initPackage(KonohaContext *kctx, kNameSpace *ns, int argc, const char **args, kfileline_t pline)
{
	kmodtrace_t *base = (kmodtrace_t*)KCALLOC(sizeof(kmodtrace_t), 1);
	base->h.name         = "tracevm";
	base->h.setup        = kmodtrace_setup;
	base->h.reftrace     = kmodtrace_reftrace;
	base->h.free         = kmodtrace_free;
	base->beforeTrace    = beforeTrace;
	base->afterTrace     = afterTrace;
	KDEFINE_CLASS TraceDef = {
		STRUCTNAME(Trace),
		.cflag = kClass_Final,
		.init = Trace_init,
		.free = Trace_free,
	};
	base->cTrace = KLIB Konoha_defineClass(kctx, ns->packageId, ns->packageDomain, NULL, &TraceDef, pline);
	KLIB Konoha_setModule(kctx, MOD_tracevm, &base->h, pline);
	KINITv(base->before, KLIB new_kMethod(kctx, 0, 0, 0, DEFAULT_Before));
	KINITv(base->after, KLIB new_kMethod(kctx, 0, 0, 0, DEFAULT_After));

	/* Array[Object] */
	kparamtype_t P_ObjectArray[] = {{TY_Object}};
	int TY_ObjectArray = (KLIB KonohaClass_Generics(kctx, CT_Array, TY_void, 1, P_ObjectArray))->typeId;
	/* Func[Boolean,String,Object[]] */
	kparamtype_t P_FuncBefore[] = {{TY_String}, {TY_ObjectArray}};
	int TY_FuncBefore = (KLIB KonohaClass_Generics(kctx, CT_Func, TY_Boolean, 2, P_FuncBefore))->typeId;
	///* Func[Boolean,String,Object,Object[]] */
	//kparamtype_t P_FuncAfter[] = {{TY_String}, {TY_Object}, {TY_ObjectArray}};
	//int TY_FuncAfter = (KLIB KonohaClass_Generics(kctx, CT_Func, TY_Boolean, 3, P_FuncAfter))->typeId;

	KDEFINE_METHOD MethodData[] = {
		_Public, _F(Trace_setBefore), TY_void, TY_Trace, MN_("setBefore"), 1, TY_FuncBefore, FN_("func"),
		//_Public, _F(Trace_setAfter), TY_void, TY_Trace, MN_("setAfter"), 1, TY_FuncAfter, FN_("func"),
		DEND,
	};
	KLIB kNameSpace_loadMethodData(kctx, ns, MethodData);
	return true;
}

static kbool_t tracevm_setupPackage(KonohaContext *kctx, kNameSpace *ns, isFirstTime_t isFirstTime, kfileline_t pline)
{
	return true;
}

static kbool_t tracevm_initNameSpace(KonohaContext *kctx, kNameSpace *ns, kfileline_t pline)
{

	return true;
}

static kbool_t tracevm_setupNameSpace(KonohaContext *kctx, kNameSpace *ns, kfileline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* tracevm_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("tracevm", "1.0"),
		.initPackage = tracevm_initPackage,
		.setupPackage = tracevm_setupPackage,
		.initNameSpace = tracevm_initNameSpace,
		.setupNameSpace = tracevm_setupNameSpace,
	};
	return &d;
}

#ifdef __cplusplus
}
#endif
