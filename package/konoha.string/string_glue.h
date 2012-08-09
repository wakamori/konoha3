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

/* ************************************************************************ */

#ifndef STRING_GLUE_H_
#define STRING_GLUE_H_

#define SPOL_sub(s0) (S_isASCII(s0)? SPOL_ASCII : 0)


/* ************************************************************************ */

#include <minikonoha/klib.h>

/* ------------------------------------------------------------------------ */

static size_t text_mlen(const char *s_text, size_t s_size)
{
#ifdef K_USING_UTF8
	size_t size = 0;
	const unsigned char *start = (const unsigned char*)s_text;
	const unsigned char *end = start + s_size;
	while (start < end) {
		size_t ulen = utf8len(start[0]);
		size++;
		start += ulen;
	}
	return size;
#else
	return s_size;
#endif
}

/* // The function below must not use for ASCII string (nakata) */
/* static kString *new_MultiByteSubString(KonohaContext *kctx, kString *s, size_t moff, size_t mlen) */
/* { */
/* 	DBG_ASSERT(!S_isASCII(s)); */
/* 	const unsigned char *start = (unsigned char *)S_text(s); */
/* 	const unsigned char *itr = start; */
/* 	size_t i; */
/* 	for(i = 0; i < moff; i++) { */
/* 		itr += utf8len(itr[0]); */
/* 	} */
/* 	start = itr; */
/* 	for(i = 0; i < mlen; i++) { */
/* 		itr += utf8len(itr[0]); */
/* 	} */
/* 	size_t len = itr - start; */
/* 	s = KLIB new_kString(kctx, (const char *)start, len, SPOL_UTF8); */
/* 	return s; */
/* } */


/* ------------------------------------------------------------------------ */
//## method @Const Int String.getSize();

static KMETHOD String_getSize(KonohaContext *kctx, KonohaStack *sfp)
{
	kString *s = sfp[0].s;
	size_t size = (S_isASCII(s) ? S_size(s) : text_mlen(S_text(s), S_size(s)));
	RETURNi_(size);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.startsWith(String s);

static KMETHOD String_startsWith(KonohaContext *kctx, KonohaStack *sfp)
{
	// @TEST "A".startsWith("ABC");
	RETURNb_(strncmp(S_text(sfp[0].s), S_text(sfp[1].asString), S_size(sfp[1].asString)) == 0);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.endsWith(String s);

static KMETHOD String_endsWith(KonohaContext *kctx, KonohaStack *sfp)
{
	kString *s0 = sfp[0].s;
	kString *s1 =  sfp[1].asString;
	int ret;
	if (S_size(s0) < S_size(s1)) {
		ret = 0;
	}
	else {
		const char *p = S_text(s0) + (S_size(s0) - S_size(s1));
		ret = (strncmp(p, S_text(s1), S_size(s1)) == 0);
	}
	RETURNb_(ret);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int String.indexOf(String s);

static KMETHOD String_indexOf(KonohaContext *kctx, KonohaStack *sfp)
{
	kString *s0 = sfp[0].s, *s1 = sfp[1].asString;
	long loc = -1;
	if (IS_NOTNULL(s1)) {
		const char *t0 = S_text(s0);
		const char *t1 =  S_text(s1);
		char *p = strstr(t0, t1);
		if (p != NULL) {
			loc = p - t0;
			if (!S_isASCII(s0)) {
				loc = text_mlen(t0, (size_t)loc);
			}
		}
	}
	RETURNi_(loc);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int String.lastIndexOf(String s);

static KMETHOD String_lastIndexOf(KonohaContext *kctx, KonohaStack *sfp)
{
	kString *s0 = sfp[0].s;
	kString *s1 = sfp[1].asString;
	const char *t0 = S_text(s0);
	const char *t1 = S_text(s1);
	intptr_t loc = S_size(s0) - S_size(s1);
	int len = S_size(s1);
	if(S_size(s1) == 0) loc--;
	for(; loc >= 0; loc--) {
		if(t0[loc] == t1[0]) {
			if(strncmp(t0 + loc, t1, len) == 0) break;
		}
	}
	if (loc >= 0 && !S_isASCII(s0)) {
		loc = text_mlen(t0, (size_t)loc);
	}
	RETURNi_(loc);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.trim();

static KMETHOD String_trim(KonohaContext *kctx, KonohaStack *sfp)
{
	const char *s = S_text(sfp[0].s);
	int len = S_size(sfp[0].s);
	kString *ret = NULL;
	while(isspace(s[0])) {
		s++;
		len--;
	}
	if(len != 0) {
		while(isspace(s[len-1])) {
			len--;
			if(len == 0) break;
		}
	}
	if(S_size(sfp[0].s) > len) {
		ret = KLIB new_kString(kctx, s, len, SPOL_sub(sfp[0].s));
	}
	else {
		ret = sfp[0].s;
	}
	RETURN_(ret);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.opHAS(String s);

static KMETHOD String_opHAS(KonohaContext *kctx, KonohaStack *sfp)
{
	RETURNb_(strstr(S_text(sfp[0].s), S_text(sfp[1].asString)) != NULL);
}

//## @Const method String String.charCodeAt(Int n);
static KMETHOD String_charCodeAt(KonohaContext *kctx, KonohaStack *sfp)
{
	kString *s = sfp[0].asString;
	int index = check_index(kctx, sfp[1].intValue, S_size(s), sfp[K_RTNIDX].uline);
	RETURNi_(S_text(s)[index]);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.get(Int n);

static kString *S_mget(KonohaContext *kctx, kString *s, size_t n)
{
	DBG_ASSERT(!S_isASCII(s));
	kString *ret = NULL;
	const unsigned char *text = (const unsigned char *)S_text(s);
	const unsigned char *start = text;
	size_t size = S_size(s);
	size_t i;

	if ((int)n < 0) {
		return ret;
	}
	for (i = 0; i < n; i++) {
		start += utf8len(start[0]);
	}
	if (start < text + size) {
		const unsigned char *end = start;
		end += utf8len(end[0]);
		ret = KLIB new_kString(kctx, (const char *)start, end - start, SPOL_POOL|SPOL_UTF8);
	}
	return ret;
}

static KMETHOD String_get(KonohaContext *kctx, KonohaStack *sfp)
{
	kString *s = sfp[0].s;
	size_t n = (size_t)sfp[1].intValue;
	if (S_isASCII(s)) {
		n = check_index(kctx, sfp[1].intValue, S_size(s), sfp[K_RTNIDX].uline);
		s = KLIB new_kString(kctx, S_text(s) + n, 1, SPOL_POOL|SPOL_ASCII);
	}
	else {
		s = S_mget(kctx, s, n);
		if (unlikely(s == NULL)) {
			kreportf(CritTag, sfp[K_RTNIDX].uline, "Script!!: out of array index %ld", (int)n);
			KLIB Kraise(kctx, EXPT_("OutOfStringBoundary"), sfp, sfp[K_RTNIDX].uline);
		}
	}
	RETURN_(s);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.substring(Int offset, Int length);

static kString *S_msubstring(KonohaContext *kctx, kString *s, size_t moff, size_t mlen)
{
	DBG_ASSERT(!S_isASCII(s));
	const unsigned char *text = (const unsigned char *)S_text(s);
	const unsigned char *start = text;
	size_t size = S_size(s);
	kString *ret = NULL;
	size_t i;

	if ((int)moff < 0) {
		return ret;
	}
	for (i = 0; i < moff; i++) {
		start += utf8len(start[0]);
	}
	if (start < text + size) {
		const unsigned char *end = NULL;
		if ((int)mlen <= 0) {
			end = text + size;
			ret = KLIB new_kString(kctx, (const char *)start, end - start, SPOL_POOL|SPOL_UTF8);
		}
		else {
			end = start;
			for (i = 0; i < mlen; i++) {
				end += utf8len(end[0]);
			}
			if (end < text + size) {
				ret = KLIB new_kString(kctx, (const char *)start, end - start, SPOL_POOL|SPOL_UTF8);
			}
			else {
				ret = KLIB new_kString(kctx, (const char *)start, text + size - start, SPOL_POOL|SPOL_UTF8);
			}
		}
	}
	return ret;
}

static KMETHOD String_substring(KonohaContext *kctx, KonohaStack *sfp)
{
	kString *s0 = sfp[0].s;
	size_t offset = (size_t)sfp[1].intValue;
	size_t length = (size_t)sfp[2].intValue;
	kString *ret = NULL;
	if (S_isASCII(s0)) {
		offset = check_index(kctx, offset, S_size(s0), sfp[K_RTNIDX].uline);
		const char *new_text = S_text(s0) + offset;
		size_t new_size = S_size(s0) - offset;
		if (length != 0 && length < new_size) {
			new_size = length;
		}
		ret = KLIB new_kString(kctx, new_text, new_size, SPOL_ASCII|SPOL_POOL); // FIXME SPOL
	}
	else {
		ret = S_msubstring(kctx, s0, offset, length);
		if (unlikely(ret == NULL)) {
			kreportf(CritTag, sfp[K_RTNIDX].uline, "Script!!: out of array index %ld", (int)offset);
			KLIB Kraise(kctx, EXPT_("OutOfStringBoundary"), sfp, sfp[K_RTNIDX].uline);
		}
	}
	RETURN_(ret);
}

/* ------------------------------------------------------------------------ */

static kString* S_toupper(KonohaContext *kctx, kString *s0, size_t start)
{
	size_t i, size = S_size(s0);
	kString *s = KLIB new_kString(kctx, NULL, size, SPOL_sub(s0)|SPOL_NOCOPY);
	memcpy(s->buf, s0->buf, size);
	for(i = start; i < size; i++) {
		int ch = s->buf[i];
		if('a' <= ch && ch <= 'z') {
			s->buf[i] = toupper(ch);
		}
	}
	return s;
}

//## @Const method String String.toLower()
static KMETHOD String_toUpper(KonohaContext *kctx, KonohaStack *sfp)
{
	kString *s0 = sfp[0].s;
	size_t i, size = S_size(s0);
	for(i = 0; i < size; i++) {
		int ch = s0->buf[i];
		if('a' <= ch && ch <= 'z') {
			RETURN_(S_toupper(kctx, s0, i));
		}
	}
	RETURN_(s0);
}

/* ------------------------------------------------------------------------ */

static kString* S_tolower(KonohaContext *kctx, kString *s0, size_t start)
{
	size_t i, size = S_size(s0);
	kString *s = KLIB new_kString(kctx, NULL, size, SPOL_sub(s0)|SPOL_NOCOPY);
	memcpy(s->buf, s0->buf, size);
	for(i = start; i < size; i++) {
		int ch = s->buf[i];
		if('A' <= ch && ch <= 'Z') {
			s->buf[i] = tolower(ch);
		}
	}
	return s;
}

//## @Const method String String.toLower()
static KMETHOD String_toLower(KonohaContext *kctx, KonohaStack *sfp)
{
	kString *s0 = sfp[0].s;
	size_t i, size = S_size(s0);
	for(i = 0; i < size; i++) {
		int ch = s0->buf[i];
		if('A' <= ch && ch <= 'Z') {
			RETURN_(S_tolower(kctx, s0, i));
		}
	}
	RETURN_(s0);
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _Im kMethod_Immutable
#define _F(F)   (intptr_t)(F)

static kbool_t string_initPackage(KonohaContext *kctx, kNameSpace *ns, int argc, const char**args, kfileline_t pline)
{
	int FN_s = FN_("s");
	int FN_n = FN_("n");
	KDEFINE_METHOD MethodData[] = {
		_Public|_Const|_Im, _F(String_opHAS),       TY_Boolean, TY_String, MN_("opHAS"), 1, TY_String, FN_s,
		_Public|_Const|_Im, _F(String_trim),        TY_String, TY_String, MN_("trim"), 0,
		_Public|_Const|_Im, _F(String_get),         TY_String, TY_String, MN_("get"), 1, TY_Int, FN_n,
		_Public|_Const|_Im, _F(String_startsWith),  TY_Boolean, TY_String, MN_("startsWith"), 1, TY_String, FN_s,
		_Public|_Const|_Im, _F(String_endsWith),    TY_Boolean, TY_String, MN_("endsWith"),   1, TY_String, FN_s,
		_Public|_Const|_Im, _F(String_getSize),     TY_Int, TY_String, MN_("getSize"), 0,
		_Public|_Const|_Im, _F(String_indexOf),     TY_Int, TY_String, MN_("indexOf"), 1, TY_String, FN_n,
		_Public|_Const|_Im, _F(String_lastIndexOf), TY_Int, TY_String, MN_("lastIndexOf"), 1, TY_String, FN_n,
		_Public|_Const|_Im, _F(String_toUpper),     TY_String, TY_String, MN_("toUpper"), 0,
		_Public|_Const|_Im, _F(String_toLower),     TY_String, TY_String, MN_("toLower"), 0,
		_Public|_Const|_Im, _F(String_substring),   TY_String, TY_String, MN_("substring"), 2, TY_Int, FN_("offset"), TY_Int, FN_("length"),
		_Public|_Const|_Im, _F(String_charCodeAt),   TY_Int, TY_String, MN_("charCodeAt"), 1, TY_Int, FN_("index"),

		DEND,
	};
	KLIB kNameSpace_loadMethodData(kctx, ns, MethodData);
	return true;
}

static kbool_t string_setupPackage(KonohaContext *kctx, kNameSpace *ns, isFirstTime_t isFirstTime, kfileline_t pline)
{
	return true;
}

static kbool_t string_initNameSpace(KonohaContext *kctx, kNameSpace *ns, kfileline_t pline)
{
	return true;
}

static kbool_t string_setupNameSpace(KonohaContext *kctx, kNameSpace *ns, kfileline_t pline)
{
	return true;
}

#endif /* STRING_GLUE_H_ */

