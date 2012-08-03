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

#ifndef MODTRACEVM_H_
#define MODTRACEVM_H_

#ifndef MINIOKNOHA_H_
#error Do not include tracevm.h without minikonoha.h.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

#define ktracemod        ((ktracemod_t*)kctx->mod[MOD_tracevm])
#define kmodtrace        ((kmodtrace_t*)kctx->modshare[MOD_tracevm])
#define IS_defineTrace() (kctx->modshare[MOD_tracevm] != NULL)
#define CT_Trace         kmodtrace->cTrace
#define TY_Trace         CT_Trace->typeId
#define IS_Trace(O)      ((O)->h.ct == CT_Trace)

typedef int (*FTrace)(KonohaContext *, KonohaStack *, kfileline_t);
typedef struct {
	KonohaModule h;
	FTrace       beforeTrace;
	FTrace       afterTrace;
	kMethod      *before;
	kMethod      *after;
	KonohaClass  *cTrace;
} kmodtrace_t;

typedef struct {
	KonohaContextModule h;
} ktracemod_t;

typedef const struct _kTrace kTrace;
struct _kTrace {
	KonohaObjectHeader h;
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MODTRACE_H_ */
