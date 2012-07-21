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

#ifndef CLEARSILVER_GLUE_H_
#define CLEARSILVER_GLUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <ClearSilver/ClearSilver.h>
#include <ClearSilver/cgi/cgi.h>
#include <ClearSilver/util/neo_err.h>
#include <ClearSilver/util/neo_str.h>
#include <minikonoha/minikonoha.h>
#include <minikonoha/logger.h>

/* ======================================================================== */

typedef struct hdf_t {
	HDF *hdf;
	int refer_cnt;
} hdf_t;

typedef struct hdf_conf_t {
	HDF *hdf;
	hdf_t *root_hdf_obj;
} hdf_conf_t;

typedef struct kHdf {
	KonohaObjectHeader h;
	HDF *hdf;
	hdf_t *root_hdf_obj;
} kHdf;

#define S_kHdf(a)         ((kHdf *)a.o)
#define S_HDF(a)          (((kHdf *)a.o)->hdf)

#define TRACE_NEOERR(POLICY, ERR_AT)	do {\
		STRING err_str;\
		string_init(&err_str);\
		nerr_error_string(err, &err_str);\
		ktrace(_DeveloperFault,\
				KEYVALUE_s("@", "hdf_set_value"),\
				KEYVALUE_s("error_string", err_str.buf));\
		nerr_ignore(&err);\
		string_clear(&err_str);\
} while(0)\


// free hdf_t only when no reference
static void hdf_t_free(hdf_t *h)
{
	if (h == NULL) return;
	h->refer_cnt--;
	if (h->refer_cnt == 0) {
		hdf_destroy(&h->hdf);
		free(h);
	}
}

static hdf_t* hdf_t_init()
{
	hdf_t *hdf_obj = (hdf_t *)(malloc(sizeof(hdf_t)));
	hdf_init(&hdf_obj->hdf);
	hdf_obj->refer_cnt = 0;
	return hdf_obj;
}

static void attach_root_hdf_t(hdf_t *org, hdf_t **dest)
{
	org->refer_cnt++;
	*dest = org;
}

#define new_kHdf(C, H, P)    Knew_Hdf(kctx, C, H, P)
static kHdf* Knew_Hdf(KonohaContext *kctx, KonohaClass *ct, HDF *hdf, kHdf *parent)
{
	if (hdf != NULL && parent != NULL) {
		hdf_conf_t conf = {
			.hdf = hdf,
			.root_hdf_obj = parent->root_hdf_obj
		};
		return (kHdf*)KLIB new_kObject(kctx, ct, (intptr_t)&conf);
	} else {
		return (kHdf*)KLIB Knull(kctx, ct);
	}
}

static void kHdf_init(KonohaContext *kctx, kObject *o, void *conf)
{
	kHdf *h = (kHdf *)o;
	// printf("kHdf_init: %p\n", o);
	if (conf != NULL) {
		hdf_conf_t *c = (hdf_conf_t *)conf;
		h->hdf = c->hdf;
		attach_root_hdf_t(c->root_hdf_obj, &h->root_hdf_obj);
	} else {
		hdf_t *hdf_obj = hdf_t_init();
		h->hdf = hdf_obj->hdf;
		attach_root_hdf_t(hdf_obj, &h->root_hdf_obj);
	}
}

static void kHdf_free(KonohaContext *kctx, kObject *o)
{
	kHdf *self = (kHdf *)o;
	// printf("kHdf_free: %p\n", self);
	// not free when self->hdf is same as self->root_hdf_obj->hdf
	if (self->hdf != self->root_hdf_obj->hdf) {
		hdf_destroy(&self->hdf);
	}
	hdf_t_free(self->root_hdf_obj);
	self->root_hdf_obj = NULL;
	self->hdf = NULL;
}


#define S_kCs(a)              ((kCs *)a.o)
#define S_CSPARSE(a)          (((kCs *)a.o)->cs)

typedef struct kCs {
	KonohaObjectHeader h;
	CSPARSE *cs;
	hdf_t *root_hdf_obj;
} kCs;

static void kCs_init(KonohaContext *kctx, kObject *o, void *conf)
{
	kCs *c = (kCs *)o;
	kHdf *h = (kHdf *)conf;
	if (h != NULL) {
		attach_root_hdf_t(h->root_hdf_obj, &c->root_hdf_obj);
		cs_init(&c->cs, h->hdf);
		cgi_register_strfuncs(c->cs);
	}
}

static void kCs_free(KonohaContext *kctx, kObject *o)
{
	kCs *c = (kCs *)o;
	if (c->root_hdf_obj != NULL) {
		hdf_t_free(c->root_hdf_obj);
		c->root_hdf_obj = NULL;
	}
	if (c->cs != NULL) {
		cs_destroy(&c->cs);
		c->cs = NULL;
	}
}

typedef struct kCgi {
	KonohaObjectHeader h;
} kCgi;

/* ======================================================================== */
/* [API bindings] */

//## Hdf Hdf.new();
static KMETHOD Hdf_new(KonohaContext *kctx, KonohaStack *sfp)
{
	// printf("hogehoge");
	// printf("Hdf_new: %p\n", sfp[0].o);
	RETURN_(KLIB new_kObject(kctx, O_ct(sfp[K_RTNIDX].o), 0));
}

//## void Hdf.setValue(String name, String value);
static KMETHOD Hdf_setValue(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	const char *name = S_text(sfp[1].s);
	const char *value = S_text(sfp[2].s);

	NEOERR *err = hdf_set_value(hdf, name, value);

	if (err != STATUS_OK) {
		TRACE_NEOERR(_DeveloperFault, "hdf_set_value");
	}
	RETURNvoid_();
}

//## void Hdf.setIntValue(String name, Int value);
static KMETHOD Hdf_setIntValue(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	const char *name = S_text(sfp[1].s);
	int value = sfp[2].intValue;

	NEOERR *err = hdf_set_int_value(hdf, name, value);

	if (err != STATUS_OK) {
		TRACE_NEOERR(_DeveloperFault, "hdf_set_int_value");
	}
	RETURNvoid_();
}


//## String Hdf.getValue(String name, String defaultValue)
// This method retrieves a string value from the HDF dataset. The hdfpath is a dotted path of the form "A.B.C".
static KMETHOD Hdf_getValue(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	const char *name = S_text(sfp[1].s);
	const char *defaultValue = S_text(sfp[2].s);
	const char *value = hdf_get_value(hdf, name, defaultValue);
	RETURN_(KLIB new_kString(kctx, value, strlen(value), 0));
}


//## String Hdf.writeString()
// serializes HDF contents to a string
static KMETHOD Hdf_writeString(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	char *ret = NULL;
	NEOERR* err = hdf_write_string(hdf, &ret);
	if (err != STATUS_OK) {
		TRACE_NEOERR(_SystemFault, "hdf_write_string");
	}
	kString *string = KLIB new_kString(kctx, ret, strlen(ret), 0);
	free(ret);
	RETURN_(string);
}

//## String Hdf.dump()
// Serializes the HDF tree to a String in a slightly different format than writeString().
// NOTE: this method dump text into stdouts directly
static KMETHOD Hdf_dump(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	const char *prefix = S_text(sfp[1].s);
	NEOERR* err = hdf_dump(hdf, prefix);
	if (err != STATUS_OK) {
		TRACE_NEOERR(_SystemFault, "hdf_dump");
	}
	RETURNvoid_();
}

//## HDF Hdf.getObj(String name)
// This method allows you to retrieve the HDF object which represents the HDF subtree ad the named hdfpath.
static KMETHOD Hdf_getObj(KonohaContext *kctx, KonohaStack *sfp)
{
	kHdf *self = (kHdf *)sfp[0].o;
	HDF *hdf = S_HDF(sfp[0]);
	const char *name = S_text(sfp[1].s);
	HDF *retHdf = hdf_get_obj(hdf, name);
	kHdf *obj = (kHdf*)new_kHdf(O_ct(sfp[K_RTNIDX].o), retHdf, self);
	RETURN_(obj);
}

//## String Hdf.objValue()
// This method retrieves the value of the current HDF node. Here is a sample code snippit:
//   HDF hdf = new HDF();
//   hdf.setValue("A.B.C","1");
//   HDF hdf_subnode = hdf.getObj("A.B.C");
// 
//   // this will print "1"
//   System.p(hdf_subnode.objValue());
static KMETHOD Hdf_objValue(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	char *value = hdf_obj_value(hdf);

	if (value == NULL) RETURN_(K_NULL);
	RETURN_(KLIB new_kString(kctx, value, strlen(value), 0));
}

//## int getIntValue(String hdfpath, int defaultValue)
// This method retrieves an integer value from the HDF dataset. The hdfpath is a dotted path of the form "A.B.C".
static KMETHOD Hdf_getIntValue(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	const char *name = S_text(sfp[1].s);
	int defaultValue = sfp[2].intValue;

	RETURNi_(hdf_get_int_value(hdf, name, defaultValue));
}

//## void Hdf.copy(String name, HDF src)
// Copy the HDF tree src to the destination path hdfpath in this HDF tree. src may be in this path or not. Result is undefined for overlapping source and destination.
static KMETHOD Hdf_copy(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	const char *name = S_text(sfp[1].s);
	HDF *src = S_HDF(sfp[2]);
	NEOERR *err = hdf_copy(hdf, name, src);

	if (err != STATUS_OK) {
		TRACE_NEOERR(_DeveloperFault, "hdf_get_node");
	}
	RETURNvoid_();
}

//## Hdf Hdf.getNode(String name)
// Retrieves the HDF object that is the root of the subtree at hdfpath, create the subtree if it doesn't exist
// NOTE: this method is named getOrCreateObj in Java version
static KMETHOD Hdf_getNode(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	const char *name = S_text(sfp[1].s);
	HDF *retHdf;
	NEOERR *err = hdf_get_node(hdf, name, &retHdf);

	if (err != STATUS_OK) {
		TRACE_NEOERR(_DeveloperFault, "hdf_get_node");
	}
	RETURN_(new_kHdf(O_ct(sfp[K_RTNIDX].o), retHdf, S_kHdf(sfp[0])));
}

//## void readString(String data)
// parses/loads the contents of the given string as HDF into the current HDF object
static KMETHOD Hdf_readString(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	const char *data = S_text(sfp[1].s);
	NEOERR *err = hdf_read_string(hdf, data);

	if (err != STATUS_OK) {
		TRACE_NEOERR(_DeveloperFault, "hdf_read_string");
	}
	RETURNvoid_();
}

//## Hdf Hdf.objChild()
// This method is used to walk the HDF tree. Keep in mind that every node in the tree can have a value, a child, and a next peer.
static KMETHOD Hdf_objChild(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	HDF *retHdf = hdf_obj_child(hdf);
	RETURN_(new_kHdf(O_ct(sfp[K_RTNIDX].o), retHdf, S_kHdf(sfp[0])));
}

//## Hdf Hdf.getChild(String name)
// Retrieves the HDF for the first child of the root of the subtree at hdfpath, or null if no child exists of that path or if the path doesn't exist.
static KMETHOD Hdf_getChild(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	const char *name = S_text(sfp[1].s);
	HDF *retHdf = hdf_get_child(hdf, name);
	RETURN_(new_kHdf(O_ct(sfp[K_RTNIDX].o), retHdf, S_kHdf(sfp[0])));
}

//## Hdf Hdf.objTop()
// Return the root of the tree that this node is in.
// NOTE: this method is named getRootObj in Java version
static KMETHOD Hdf_objTop(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	HDF *retHdf = hdf_obj_top(hdf);
	RETURN_(new_kHdf(O_ct(sfp[K_RTNIDX].o), retHdf, S_kHdf(sfp[0])));
}

//## Hdf Hdf.objNext()
// This method is used to walk the HDF tree to the next peer.
static KMETHOD Hdf_objNext(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	HDF *retHdf = hdf_obj_next(hdf);
	RETURN_(new_kHdf(O_ct(sfp[K_RTNIDX].o), retHdf, S_kHdf(sfp[0])));
}

//## String objName()
// This method retrieves the name of the current HDF node. The name only includes the current level. Here is a sample code snippit:
//   HDF hdf = new HDF();
//   hdf.setValue("A.B.C","1");
//   HDF hdf_subnode = hdf.getObj("A.B.C");
// 
//   // this will print "C"
//   System.out.println(hdf_subnode.objName());
static KMETHOD Hdf_objName(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	char *name = hdf_obj_name(hdf);

	if (name == NULL) RETURN_(K_NULL);
	RETURN_(KLIB new_kString(kctx, name, strlen(name), 0));
}

//## void Hdf.setCopy(String name, String srcName)
static KMETHOD Hdf_setCopy(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	const char *name = S_text(sfp[1].s);
	const char *srcName = S_text(sfp[2].s);
	NEOERR *err = hdf_set_copy(hdf, name, srcName);
	if (err != STATUS_OK) {
		TRACE_NEOERR(_DeveloperFault, "hdf_set_copy");
	}
	RETURNvoid_();
}

//## void removeTree(String name)
// Remove all nodes of the HDF subtree at name
static KMETHOD Hdf_removeTree(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	const char *name = S_text(sfp[1].s);
	NEOERR *err = hdf_remove_tree(hdf, name);
	if (err != STATUS_OK) {
		TRACE_NEOERR(_DeveloperFault, "hdf_remove_tree");
	}
	RETURNvoid_();
}

//## void writeFileAtomic(String path
// like writeFile, but first writes to a temp file then uses rename(2) to ensure updates are Atomic
static KMETHOD Hdf_writeFileAtomic(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	const char *path = S_text(sfp[1].s);
	NEOERR *err = hdf_write_file_atomic(hdf, path);
	if (err != STATUS_OK) {
		TRACE_NEOERR(_DeveloperFault, "hdf_write_file_atomic");
	}
	RETURNvoid_();
}

//## void Hdf.writeFile(String path)
// writes/serializes HDF dataset to file
static KMETHOD Hdf_writeFile(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	const char *path = S_text(sfp[1].s);
	NEOERR *err = hdf_write_file(hdf, path);
	if (err != STATUS_OK) {
		TRACE_NEOERR(_DeveloperFault, "hdf_write_file");
	}
	RETURNvoid_();
}

//## void Hdf.readFile(String path)
// This method reads the contends of an on-disk HDF dataset into the current HDF object.
static KMETHOD Hdf_readFile(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	const char *path = S_text(sfp[1].s);
	NEOERR *err = hdf_read_file(hdf, path);
	if (err != STATUS_OK) {
		TRACE_NEOERR(_DeveloperFault, "hdf_read_file");
	}
	RETURNvoid_();
}

//## void Hdf.setSymLink(String name, destName)
// Links the src hdfpath to the dest
static KMETHOD Hdf_setSymLink(KonohaContext *kctx, KonohaStack *sfp)
{
	HDF *hdf = S_HDF(sfp[0]);
	const char *name = S_text(sfp[1].s);
	const char *destName = S_text(sfp[2].s);
	NEOERR *err = hdf_set_symlink(hdf, name, destName);
	if (err != STATUS_OK) {
		TRACE_NEOERR(_DeveloperFault, "hdf_set_symlink");
	}
	RETURNvoid_();
}

/* ======================================================================== */

//## Cs Cs.new();
static KMETHOD Cs_new(KonohaContext *kctx, KonohaStack *sfp)
{
	// kCs *cs = S_kCs(sfp[0]);
	kHdf *hdf = S_kHdf(sfp[1]);
	RETURN_(KLIB new_kObject(kctx, O_ct(sfp[K_RTNIDX].o), (uintptr_t)hdf));
}

//## void Cs.parseString(String template)
static KMETHOD Cs_parseString(KonohaContext *kctx, KonohaStack *sfp)
{
	CSPARSE *cs = S_CSPARSE(sfp[0]);
	char *t = strdup(S_text(sfp[1].s));
	size_t len = S_size(sfp[1].s);
	NEOERR *err = cs_parse_string(cs, t, len);
	if (err != STATUS_OK) {
		TRACE_NEOERR(_DeveloperFault, "cs_parse_string");
	}
	RETURNvoid_();
}

//## void Cs.parseFile(String path)
static KMETHOD Cs_parseFile(KonohaContext *kctx, KonohaStack *sfp)
{
	CSPARSE *cs = S_CSPARSE(sfp[0]);
	const char *path = S_text(sfp[1].s);
	NEOERR *err = cs_parse_file(cs, path);
	if (err != STATUS_OK) {
		TRACE_NEOERR(_DeveloperFault, "cs_parse_file");
	}
	RETURNvoid_();
}

typedef struct render_arg {
	KonohaContext *kctx;
	kFunc *fo;
} render_arg_t;

static NEOERR *render_cb(void *v, char *s)
{
	render_arg_t *arg = (render_arg_t *)v;
	KonohaContext *kctx = arg->kctx;
	kFunc *fo = arg->fo;
	BEGIN_LOCAL(lsfp, K_CALLDELTA + 2);
	KSETv(lsfp[K_CALLDELTA+1].s, KLIB new_kString(kctx, s, strlen(s), 0));
	KCALL(lsfp, 0, fo->mtd, 1, KNULL(Int));
	END_LOCAL();

	switch (lsfp[0].intValue) {
	case STATUS_OK_INT:
		return STATUS_OK;
	case INTERNAL_ERR_INT:
		return INTERNAL_ERR;
	default:
		/* unknow return value */
		return STATUS_OK;
	}
}

//## void Cs.render(Func<String=>int> cb);
static KMETHOD Cs_render(KonohaContext *kctx, KonohaStack *sfp)
{
	CSPARSE *cs = S_CSPARSE(sfp[0]);
	kFunc *fo = sfp[1].asFunc;
	render_arg_t arg = {
		.kctx = kctx,
		.fo = fo
	};
	NEOERR *err = cs_render(cs, &arg, render_cb);
	if (err != STATUS_OK) {
		TRACE_NEOERR(_DeveloperFault, "cs_render");
	}
	RETURNvoid_();
}

//## void Cs.dump(Func<String=>int> cb);
static KMETHOD Cs_dump(KonohaContext *kctx, KonohaStack *sfp)
{
	CSPARSE *cs = S_CSPARSE(sfp[0]);
	kFunc *fo = sfp[1].asFunc;
	render_arg_t arg = {
		.kctx = kctx,
		.fo = fo
	};
	NEOERR *err = cs_dump(cs, &arg, render_cb);
	if (err != STATUS_OK) {
		TRACE_NEOERR(_DeveloperFault, "cs_dump");
	}
	RETURNvoid_();
}

/* ======================================================================== */
//## @Static String Cgi.urlEscape(String url);
static KMETHOD Cgi_urlEscape(KonohaContext *kctx, KonohaStack *sfp)
{
	char *buf = NULL;
	cgi_url_escape(S_text(sfp[1].s), &buf);
	kString *string = KLIB new_kString(kctx, buf, strlen(buf), 0);
	free(buf);
	RETURN_(string);
}

//## @Static String Cgi.htmlEscape(String html);
static KMETHOD Cgi_htmlEscape(KonohaContext *kctx, KonohaStack *sfp)
{
	char *buf = NULL;
	cgi_html_escape_strfunc(S_text(sfp[1].s), &buf);
	kString *string = KLIB new_kString(kctx, buf, strlen(buf), 0);
	free(buf);
	RETURN_(string);
}

/* ======================================================================== */

#define CT_Hdf cHdf
#define TY_Hdf cHdf->classId
#define CT_Cs  cCs
#define TY_Cs  cCs->classId
#define CT_Cgi cCgi
#define TY_Cgi cCgi->classId

static kbool_t clearsilver_initPackage(KonohaContext *kctx, kNameSpace *ns, int argc, const char **args, kfileline_t pline)
{
	KDEFINE_CLASS defHdf = {
		STRUCTNAME(Hdf),
		.cflag = kClass_Final,
		.init = kHdf_init,
		.free = kHdf_free,
	};
	KonohaClass *cHdf = KLIB Konoha_defineClass(kctx, ns->packageId, ns->packageDomain, NULL, &defHdf, pline);

	KDEFINE_CLASS defCs = {
		STRUCTNAME(Cs),
		.cflag = kClass_Final,
		.init = kCs_init,
		.free = kCs_free,
	};
	KonohaClass *cCs = KLIB Konoha_defineClass(kctx, ns->packageId, ns->packageDomain, NULL, &defCs, pline);

	KDEFINE_CLASS defCgi = {
		STRUCTNAME(Cgi),
		.cflag = kClass_Final,
	};
	KonohaClass *cCgi = KLIB Konoha_defineClass(kctx, ns->packageId, ns->packageDomain, NULL, &defCgi, pline);

#define _Public   kMethod_Public
#define _Static   kMethod_Static
#define _F(F)   (intptr_t)(F)

	kparamtype_t ps = {TY_String, FN_("str")};
	KonohaClass *CT_Func_IntString = KLIB KonohaClass_Generics(kctx, CT_Func, TY_Int, 1, &ps);
	ktype_t TY_Func_IntString = CT_Func_IntString->classId;
	KDEFINE_METHOD MethodData[] = {
		_Public, _F(Hdf_new)     	, TY_Hdf 	, TY_Hdf, MN_("new")		, 0,
		_Public, _F(Hdf_setValue)	, TY_void	, TY_Hdf, MN_("setValue")	, 2, TY_String, FN_("name"), TY_String, FN_("value"),
		_Public, _F(Hdf_setIntValue), TY_void	, TY_Hdf, MN_("setIntValue"), 2, TY_String, FN_("name"), TY_Int, FN_("value"),
		_Public, _F(Hdf_getValue)	, TY_String	, TY_Hdf, MN_("getValue")	, 2, TY_String, FN_("name"), TY_String, FN_("defaultValue"),
		_Public, _F(Hdf_writeString), TY_String	, TY_Hdf, MN_("writeString"), 0, 
		_Public, _F(Hdf_readString)	, TY_void	, TY_Hdf, MN_("readString")	, 1, TY_String, FN_("data"),
		_Public, _F(Hdf_dump)		, TY_void	, TY_Hdf, MN_("dump")		, 1, TY_String, FN_("prefix"),
		_Public, _F(Hdf_getObj)		, TY_Hdf	, TY_Hdf, MN_("getObj")		, 1, TY_String, FN_("name"),
		_Public, _F(Hdf_objValue)	, TY_String	, TY_Hdf, MN_("objValue")	, 0, 
		_Public, _F(Hdf_objName)	, TY_String	, TY_Hdf, MN_("objName")	, 0, 
		_Public, _F(Hdf_getIntValue), TY_Int	, TY_Hdf, MN_("getIntValue"), 2, TY_String, FN_("name"), TY_Int, FN_("defaultValue"),
		_Public, _F(Hdf_copy)		, TY_void	, TY_Hdf, MN_("copy")		, 2, TY_String, FN_("name"), TY_Hdf, FN_("src"),
		_Public, _F(Hdf_setCopy)	, TY_void	, TY_Hdf, MN_("setCopy")	, 2, TY_String, FN_("name"), TY_String, FN_("srcName"),
		_Public, _F(Hdf_getNode)	, TY_Hdf	, TY_Hdf, MN_("getNode")	, 1, TY_String, FN_("name"),
		_Public, _F(Hdf_objChild)   , TY_Hdf 	, TY_Hdf, MN_("objChild")	, 0,
		_Public, _F(Hdf_getChild)   , TY_Hdf 	, TY_Hdf, MN_("getChild")	, 1, TY_String, FN_("name"),
		_Public, _F(Hdf_objTop)   	, TY_Hdf 	, TY_Hdf, MN_("objTop")		, 0,
		_Public, _F(Hdf_objNext)   	, TY_Hdf 	, TY_Hdf, MN_("objNext")	, 0,
		_Public, _F(Hdf_removeTree)	, TY_void	, TY_Hdf, MN_("removeTree")	, 1, TY_String, FN_("name"),
		_Public, _F(Hdf_writeFile)	, TY_void	, TY_Hdf, MN_("writeFile")	, 1, TY_String, FN_("path"),
		_Public, _F(Hdf_writeFileAtomic), TY_void, TY_Hdf, MN_("writeFileAtomic")	, 1, TY_String, FN_("path"),
		_Public, _F(Hdf_readFile)	, TY_void	, TY_Hdf, MN_("readFile")	, 1, TY_String, FN_("path"),
		_Public, _F(Hdf_setSymLink)	, TY_void	, TY_Hdf, MN_("setSymLink")	, 2, TY_String, FN_("name"), TY_String, FN_("destName"),
		_Public, _F(Cs_new)     	, TY_Cs 	, TY_Cs	, MN_("new")		, 1, TY_Hdf, FN_("hdf"),
		_Public, _F(Cs_parseString) , TY_void 	, TY_Cs	, MN_("parseString"), 1, TY_String, FN_("template"),
		_Public, _F(Cs_parseFile) 	, TY_void 	, TY_Cs	, MN_("parseFile")	, 1, TY_String, FN_("path"),
		_Public, _F(Cs_render) 		, TY_void 	, TY_Cs	, MN_("render")		, 1, TY_Func_IntString, FN_("fo"),
		_Public, _F(Cs_dump) 		, TY_void 	, TY_Cs	, MN_("dump")		, 1, TY_Func_IntString, FN_("fo"),
		_Public|_Static, _F(Cgi_urlEscape), TY_String, TY_Cgi, MN_("urlEscape")		, 1, TY_String, FN_("url"),
		_Public|_Static, _F(Cgi_htmlEscape), TY_String, TY_Cgi, MN_("htmlEscape")		, 1, TY_String, FN_("html"),
		DEND,
	};
	KLIB kNameSpace_loadMethodData(kctx, ns, MethodData);

	KDEFINE_INT_CONST IntData[] = {
		{"STATUS_OK", TY_Int, STATUS_OK_INT},
		{"INTERNAL_ERR", TY_Int, INTERNAL_ERR_INT},
		{}
	};
	KLIB kNameSpace_loadConstData(kctx, ns, KonohaConst_(IntData), pline);

	return true;
}

static kbool_t clearsilver_setupPackage(KonohaContext *kctx, kNameSpace *ns, isFirstTime_t isFirstTime, kfileline_t pline)
{
	return true;
}

static kbool_t clearsilver_initNameSpace(KonohaContext *kctx,  kNameSpace *ns, kfileline_t pline)
{
	return true;
}

static kbool_t clearsilver_setupNameSpace(KonohaContext *kctx, kNameSpace *ns, kfileline_t pline)
{
	return true;
}

/* ======================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* CLEARSILVER_GLUE_H_ */
