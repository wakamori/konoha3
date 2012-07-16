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
#define TRACEVM_GLUE_H_
#include <minikonoha/minikonoha.h>
#include <minikonoha/sugar.h>
#define MOD_tracevm 97 /* TODO */
#define ktracemod ((ktracemod_t*)kctx->mod[MOD_tracevm])
#define kmodtrace ((kmodtrace_t*)kctx->modshare[MOD_tracevm])
#define GenCodeMtd (kmodtrace)->genCode
#define GenCodeDefault (kmodtrace)->defaultCodeGen

typedef void (*FgenCode)(KonohaContext *, kMethod *, kBlock * );
typedef struct {
	KonohaModule h;
	kMethod *genCode;
	kArray *constPool;
	FgenCode defaultCodeGen;
} kmodtrace_t;

typedef struct {
	KonohaContextModule h;
} ktracemod_t;

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
	KREFTRACEv(mod->genCode);
	END_REFTRACE();
}

static void kmodtrace_free(KonohaContext *kctx, struct KonohaModule *baseh)
{
	//kmodtrace_t *modshare = (kmodtrace_t *)baseh;
	KFREE(baseh, sizeof(kmodtrace_t));
}
/* ------------------------------------------------------------------------ */

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Im       kMethod_Immutable
#define _Coercion kMethod_Coercion
#define _Static   kMethod_Static
#define _F(F)   (intptr_t)(F)

/****************************************************************/
//static void kGenTraceCode(KonohaContext *kctx, kMethod *mtd, kBlock *bk)
//{
//	DBG_P("start trace code generation");
//	BEGIN_LOCAL(lsfp, 8);
//	KSETv(lsfp[K_CALLDELTA+1].asMethod, mtd);
//	KSETv(lsfp[K_CALLDELTA+2].asObject, (kObject*)bk);
//	KCALL(lsfp, 0, GenCodeMtd, 2, K_NULL);
//	END_LOCAL();
//}

static kbool_t tracevm_initPackage(KonohaContext *kctx, kNameSpace *ns, int argc, const char **args, kfileline_t pline)
{
	kmodtrace_t *base = (kmodtrace_t*)KCALLOC(sizeof(kmodtrace_t), 1);
	base->h.name         = "tracevm";
	base->h.setup        = kmodtrace_setup;
	base->h.reftrace     = kmodtrace_reftrace;
	base->h.free         = kmodtrace_free;
	base->defaultCodeGen = kctx->klib->kMethod_genCode;

	KLIB Konoha_setModule(kctx, MOD_tracevm, &base->h, pline);
	return true;
}

static kbool_t tracevm_setupPackage(KonohaContext *kctx, kNameSpace *ns, kfileline_t pline)
{
	//kMethod *mtd = KLIB kNameSpace_getMethodNULL(kctx, ns, TY_System, MN_("genCode"), 0, MPOL_FIRST);
	KonohaLibVar *l = (KonohaLibVar*)kctx->klib;
	l->kMethod_genCode = GenCodeDefault;
	KLIB kNameSpace_compileAllDefinedMethods(kctx);
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
