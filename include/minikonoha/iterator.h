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

#ifndef MODITERATOR_H_
#define MODITERATOR_H_

#ifndef MINIOKNOHA_H_
#error Do not include iterator.h without minikonoha.h.
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define kiteratormod           ((kiteratormod_t*)kctx->mod[MOD_iterator])
#define kmoditerator           ((kmoditerator_t*)kctx->modshare[MOD_iterator])
#define IS_defineIterator()    (kctx->modshare[MOD_iterator] != NULL)

#define CFLAG_Iterator         kClass_Final
#define CT_Iterator            kmoditerator->cIterator
#define TY_Iterator            kmoditerator->cIterator->typeId
#define CT_StringIterator      kmoditerator->cStringIterator
#define TY_StringIterator      kmoditerator->cStringIterator->typeId

#define IS_Iterator(O)         (O_ct(O)->baseTypeId == TY_Iterator)

typedef struct {
	KonohaModule h;
	KonohaClass *cIterator;
	KonohaClass *cStringIterator;
	KonohaClass *cGenericIterator;
} kmoditerator_t;

typedef struct {
	KonohaContextModule h;
} kiteratormod_t;

typedef struct _kIterator kIterator;

struct _kIterator {
	KonohaObjectHeader h;
	kbool_t (*hasNext)(KonohaContext *kctx, KonohaStack *);
	void (*setNextResult)(KonohaContext *kctx, KonohaStack*);
	size_t current_pos;
	union {
		kObject  *source;
		kArray   *arrayList;
		kFunc    *funcHasNext;
	};
	kFunc        *funcNext;
};

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* MODITERATOR_H_ */
