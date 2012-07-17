K.import("konoha");
K.import("apache");

DEBUG = true;

String util_read(Request req)
{
}

@Static int Apache.handler(Request req)
{
	String path = req.getUri();
	String method = req.getMethod();
	String query;
	if (method == "POST") {
		query = util_read(req);
	}
	else {
		query = req.getArgs();
	}
	if (DEBUG) {
		req.logError(APLOG_WARNING, 0, "path   : " + path);
		req.logError(APLOG_WARNING, 0, "method : " + method);
		req.logError(APLOG_WARNING, 0, "query  : " + query);
	}
	return APACHE_OK;
}
