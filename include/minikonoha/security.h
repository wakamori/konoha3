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

#ifndef MODSECURITY_H_
#define MODSECURITY_H_

#ifndef MINIOKNOHA_H_
#error Do not include security.h without minikonoha.h.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

#define ksecuritymod        ((ksecuritymod_t*)kctx->mod[MOD_security])
#define kmodsecurity        ((kmodsecurity_t*)kctx->modshare[MOD_security])
#define IS_defineSecurity() (kctx->modshare[MOD_security] != NULL)
#define CT_Security         kmodsecurity->cSecurity
#define CT_Role             kmodsecurity->cRole
#define TY_Security         CT_Security->typeId
#define TY_Role             CT_Role->typeId
#define IS_Security(O)      ((O)->h.ct == CT_Security)
#define IS_Role(O)          ((O)->h.ct == CT_Role)

typedef struct {
	KonohaModule h;
	KUtilsHashMap *acl; // access control list (map)
	kMethod *checkPermission;
	KonohaClass *cSecurity;
	KonohaClass *cRole;
} kmodsecurity_t;

typedef struct {
	KonohaContextModule h;
} ksecuritymod_t;

typedef const struct _kSecurity kSecurity;
struct _kSecurity {
	KonohaObjectHeader h;
};

typedef const struct _kRole kRole;
struct _kRole {
	KonohaObjectHeader h;
	kString *name;
};

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* MODSECURITY_H_ */
