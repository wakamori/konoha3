K.import("apache");
K.import("konoha.while");
K.import("konoha.array");
K.import("konoha.assignment");

@Static int Apache.handler(Request req) {
	req.setContentType("text/html");
	req.setContentEncoding("utf-8");
	req.logError(APLOG_WARNING, 0, "log error's test");
	req.puts("<p><strong>hello world!</strong></p>");
	req.puts("<p>METHOD: " + req.getMethod() + "</p>");
	req.puts("<p>ARGS: " + req.getArgs() + "</p>");
	req.puts("<p>URI: " + req.getUri() + "</p>");
	req.puts("<p>PATH_INFO: " + req.getPathInfo() + "</p>");
	req.puts("<p>HANDLER: " + req.getHandler() + "</p>");
	AprTable reqHeaderTable = req.getHeadersIn();
	AprTableEntry[] reqHeaders = reqHeaderTable.getElts();
	int i = 0;
	while (i < reqHeaders.getSize()) {
		req.puts("<p>REQUEST_HEADERS[" + reqHeaders[i].getKey() + "] =>" + reqHeaders[i].getVal() + "</p>");
		i = i + 1;
	}
	AprTable resHeaderTable = req.getHeadersOut();
	resHeaderTable.add("key1", "val1");
	resHeaderTable.add("key2", "val2");
	resHeaderTable.set("key1", "val3");
	resHeaderTable.add("key1", "val4");
	resHeaderTable.add("key2", "値5");
	AprTableEntry[] resHeaders = resHeaderTable.getElts();
	i = 0;
	while (i < resHeaders.getSize()) {
		req.puts("<p>RESPONSE_HEADERS[" + resHeaders[i].getKey() + "] =>" + resHeaders[i].getVal() + "</p>");
		i = i + 1;
	}
	return APACHE_OK;
}
