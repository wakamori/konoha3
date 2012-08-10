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

#ifndef TRACEVM_GLUE_H
#define TRACEVM_GLUE_H

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
	BEGIN_REFTRACE(1);
	KREFTRACEv(mod->before);
	END_REFTRACE();
}

static void kmodtrace_free(KonohaContext *kctx, struct KonohaModule *baseh)
{
	KFREE(baseh, sizeof(kmodtrace_t));
}

/* ------------------------------------------------------------------------ */

static int beforeTrace(KonohaContext *kctx, KonohaStack *sfp, kfileline_t pline)
{
	kMethod *trace = sfp[K_MTDIDX].mtdNC;
	kParam *pa = Method_param(trace);
	kMethod *mtd = kmodtrace->before;
	DBG_ASSERT(mtd != NULL);
	INIT_GCSTACK();
	((KonohaContextVar*)kctx)->esp += K_CALLDELTA;
	BEGIN_LOCAL(lsfp, K_CALLDELTA + 3);
	KSETv_AND_WRITE_BARRIER(NULL, lsfp[K_CALLDELTA+0].asObject, KLIB Knull(kctx, CT_Trace), GC_NO_WRITE_BARRIER);
	KUtilsWriteBuffer wb;
	KLIB Kwb_init(&(kctx->stack->cwb), &wb);
	KLIB Kwb_printf(kctx, &wb, "%s.%s%s", Method_t(trace));
	KSETv_AND_WRITE_BARRIER(NULL, lsfp[K_CALLDELTA+1].s, KLIB new_kString(kctx, KLIB Kwb_top(kctx, &wb, 0), Kwb_bytesize(&wb), SPOL_POOL), GC_NO_WRITE_BARRIER);
	KLIB Kwb_free(&wb);
	kArray *a = new_(Array, pa->psize);
	size_t i;
	for(i = 0; i < pa->psize; i++) {
		ktype_t paramType = pa->paramtypeItems[i].ty;
		/* TODO: correct type */
		if(TY_isUnbox(paramType)) {
			KLIB kArray_add(kctx, a, KLIB new_kObject(kctx, CT_(paramType), sfp[i+1].unboxValue));
		}
		else {
			KLIB kArray_add(kctx, a, sfp[i+1].o);
		}
	}
	KSETv_AND_WRITE_BARRIER(NULL, lsfp[K_CALLDELTA+2].asArray, a, GC_NO_WRITE_BARRIER);
	KCALL(lsfp, 0, mtd, 3, KNULL(Boolean));
	END_LOCAL();
	((KonohaContextVar*)kctx)->esp -= K_CALLDELTA;
	RESET_GCSTACK();
	return lsfp[0].boolValue == true ? 0 : -1;
}

static KMETHOD DEFAULT_Before(KonohaContext *kctx, KonohaStack *sfp)
{
	DBG_P("default before");
}

/* ------------------------------------------------------------------------ */

//## void Trace.setBefore(Func[Boolean,String,Object[]] beforefunc);
static KMETHOD Trace_setBefore(KonohaContext *kctx, KonohaStack *sfp)
{
	kFunc *fo = sfp[1].asFunc;
	KSETv_AND_WRITE_BARRIER(NULL, kmodtrace->before, fo->mtd, GC_NO_WRITE_BARRIER);
	RETURNvoid_();
}

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
	KDEFINE_CLASS TraceDef = {
		STRUCTNAME(Trace),
		.cflag = kClass_Final,
	};
	base->cTrace = KLIB Konoha_defineClass(kctx, ns->packageId, ns->packageDomain, NULL, &TraceDef, pline);
	KLIB Konoha_setModule(kctx, MOD_tracevm, &base->h, pline);
	KINITv(base->before, KLIB new_kMethod(kctx, 0, 0, 0, DEFAULT_Before));

	/* Array[Object] */
	kparamtype_t P_ObjectArray[] = {{TY_Object}};
	int TY_ObjectArray = (KLIB KonohaClass_Generics(kctx, CT_Array, TY_void, 1, P_ObjectArray))->typeId;
	/* Func[Boolean,String,Object[]] */
	kparamtype_t P_FuncBefore[] = {{TY_String}, {TY_ObjectArray}};
	int TY_FuncBefore = (KLIB KonohaClass_Generics(kctx, CT_Func, TY_Boolean, 2, P_FuncBefore))->typeId;

	KDEFINE_METHOD MethodData[] = {
		_Public, _F(Trace_setBefore), TY_void, TY_Trace, MN_("setBefore"), 1, TY_FuncBefore, FN_("func"),
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

#endif /* TRACEVM_GLUE_H */
