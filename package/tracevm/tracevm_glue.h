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

/* ------------------------------------------------------------------------ */

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Im       kMethod_Immutable
#define _Coercion kMethod_Coercion
#define _Static   kMethod_Static
#define _F(F)   (intptr_t)(F)

/****************************************************************/
static void kGenTraceCode(KonohaContext *kctx, kMethod *mtd, kBlock *bk)
{
	DBG_P("start trace code generation");

}

static kbool_t tracevm_initPackage(KonohaContext *kctx, kNameSpace *ns, int argc, const char **args, kfileline_t pline)
{
	return true;
}

static kbool_t tracevm_setupPackage(KonohaContext *kctx, kNameSpace *ns, kfileline_t pline)
{

	LibKonohaApiVar *l = (LibKonohaApiVar*)kctx->klib;
	l->kMethod_genCode = GenTraceCode;
	kNameSpace_syncMethods();
	return true;
}

static kbool_t tracevm_initNameSpace(KonohaContext *kctx, kNameSpace *ns, kfileline_t pline)
{
	USING_SUGAR;
	return true;
}

static kbool_t tracevm_setupNameSpace(KonohaContext *kctx, kNameSpace *ns, kfileline_t pline)
{
	return true;
}

#endif /* TRACEVM_GLUE_H */
