K.import("konoha");
K.import("konoha.hashmap");
K.import("konoha.pcre");
K.import("apache");

DEBUG = true;

//String util_read(Request req)
//{
//	req.logError(APLOG_WARNING, 0, "TODO");
//	assert(false);
//	//if (req.getMethodNumber() != M_POST) {
//	//	req.logError(APLOG_WARNING, 0, "DECLINED");
//	//	return null;
//	//}
//}

class HttpRequest {
	String path;
	String method;
	HashMap[String] params;
	HashMap[String] cookies;

	HttpRequest(String path, String method, String query, String cookie) {
		this.path = path;
		this.method = method;
		HashMap[String] m = new HashMap[String]();
		String[] params = query.split(/&/);
		int size = params.getSize();
		int i = 0;
		while (i < size) {
			String param = params[i];
			String[] k_v = param.split(/=/);
			if (k_v.getSize() == 2) {
				m.set(k_v[0], k_v[1]);
			}
			else {
				m.set(k_v[0], "");
			}
			i = i + 1;
		}
	}
}

class Tuple {
	String s1;
	String s2;

	Tuple(String s1, String s2) {
		this.s1 = s1;
		this.s2 = s2;
	}

	void set(int n, String s) {
		if (n == 0) {
			this.s1 = s;
		}
		else if (n == 1) {
			this.s2 = s;
		}
	}

	String get(int n) {
		if (n == 0) {
			return this.s1;
		}
		else if (n == 1) {
			return this.s2;
		}
	}
}

class HttpResponse {
	String content;
	int status;
	Tuple[] h;

	HttpResponse(String content, Tuple[] headers) {
		this.content = content;
		this.h = headers;
	}

	void set(String header, String value) {
		this.h.add(new Tuple(header, value));
	}

	String[] get(String header) {
		String[] ret = new String[0];
		int i = 0;
		while (i < this.h.getSize()) {
			Tuple t = this.h[i];
			if (t[0] == header) {
				ret.add(t[1]);
			}
			i = i + 1;
		}
		return ret;
	}
}

@Singleton class Url {
	Regex[] patterns;
	Func[HttpResponse,HttpRequest][] handlers;

	void addPattern(String pattern, Func[HttpResponse,HttpRequest] handler) {
		if (this.patterns.getSize() == 0) {
			this.patterns = new Regex[0];
			this.handlers = new Func[HttpResponse,HttpRequest][0];
		}
		this.patterns.add(new Regex(pattern, ""));
		this.handlers.add(handler);
	}

	Regex[] getPatterns() {
		return patterns;
	}

	Func[HttpResponse,HttpRequest][] getHandlers() {
		return handlers;
	}
}

/* ======================================================================== */

HttpRequest g_req;
HttpResponse g_res;
Func[HttpResponse,HttpRequest] g_handler;

String getCookiefromHeader(Request req) {
	AprTableEntry[] reqHeaders = req.getHeadersIn().getElts();
	int i = 0;
	while (i < reqHeaders.getSize()) {
		if (reqHeaders[i].getKey() == "Cookie") {
			return reqHeaders[i].getVal();
		}
		i = i + 1;
	}
	return "";
}

int handler(Request req) {
	String path = req.getUri();
	String method = req.getMethod();
	String cookie = getCookiefromHeader(req);
	String query;
	//if (method == "POST") {
	//	query = util_read(req);
	//}
	//else {
		query = req.getArgs();
	//}
	if (DEBUG) {
		req.logError(APLOG_WARNING, 0, "path      : " + path);
		req.logError(APLOG_WARNING, 0, "method    : " + method);
		req.logError(APLOG_WARNING, 0, "query     : " + query);
		req.logError(APLOG_WARNING, 0, "h[Cookie] : " + cookie);
	}
	g_req = new HttpRequest(path, method, query, cookie);
	int i = 0;
	while (i < Url.getPatterns().getSize()) {
		Regex pattern = Url.getPatterns()[i];
		Func[HttpResponse,HttpRequest] handler = Url.getHandlers()[i];
		if (path.search(pattern) >= 0) {
			g_handler = handler;
			//if (DEBUG) {
			//	req.logError(APLOG_WARNING, 0, "handler : " + g_handler);
			//}
			g_res = g_handler(g_req);
			break;
		}
		i = i + 1;
	}
	if (g_res != null) {
		Tuple[] headers = g_res.h;
		AprTable resHeaderTable = req.getHeadersOut();
		i = 0;
		boolean existsContentType = false;
		while (i < headers.getSize()) {
			if (headers[0] == "Content-Type") {
				existsContentType = true;
				req.setContentType(headers[1]);
			}
			resHeaderTable.add(headers[0], headers[1]);
			i = i + 1;
		}
		if (!existsContentType) {
			req.setContentType("text/html");
			req.setContentEncoding("utf-8");
		}
		req.puts(g_res.content);
		return g_res.status;
	}
	req.puts("no handler matched");
	return APACHE_OK;
}
