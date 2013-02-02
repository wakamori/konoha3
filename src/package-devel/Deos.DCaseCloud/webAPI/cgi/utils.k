Import("Type.Json");
Import("Syntax.CStyleWhile");
Import("Syntax.Null");
Import("Java.Class");
Import("posix.process");
Import("Type.File");

boolean checkVersion(int version) {
	//boolean ret = false;
	//if (version == 1) {
	//	ret = true;
	//}
	//else {
	//}
	//return ret;
	return true;
}

boolean checkID(Json j) {
	return j.hasKey("id");
}

String getMsg() {
	String mtd_type = System.getenv("REQUEST_METHOD");
	if (mtd_type != "POST") {
		// ERROR Handling: only POST Method is available
	}
	String query;
	String ln;
	FILE f = stdin;
	while ((ln = f.readLine()) != null) {
		query = query + ln;
	}
	return query;
	//return "{\"jsonrpc\": \"2.0\", \"version\": 1, \"method\": \"update\", \"params\": {\"argument_id\": \"10\", \"node_id\": \"46\", \"description\": \"mosh!\"}}";
	//return "{\"jsonrpc\": \"2.0\", \"version\": 1, \"method\": \"delete\", \"params\": {\"argument_id\": \"10\", \"node_id\": \"46\"}}";
	//return "{\"jsonrpc\": \"2.0\", \"version\": 1, \"method\": \"create\", \"params\": {\"type\": \"Goal\", \"description\": \"hoge\"}}";
	//return "{\"jsonrpc\": \"2.0\", \"version\": 1, \"method\": \"get\", \"params\": {\"filter\": [\"Argument\"] }}";
	//return "{\"jsonrpc\": \"2.0\", \"version\": 1, \"method\": \"get\", \"params\": {\"argument_id\": \"0\" }}";
	//return "{\"jsonrpc\": \"2.0\", \"version\": 1, \"method\": \"insert\", \"params\": {\"type\": \"Goal\", \"description\": \"hello, matz!!\", \"parent\": {\"argument_id\": \"0\", \"node_id\": \"7\"} }}";
}

void flush(String str) {
	String result = "Content-Type: application/json; charset=utf-8\n\n";
	result = result + str;
	stdout.write(result);
}

String JsonRPC_fmt(Json j, String key, String value) {
	if (j.hasKey("id")) {
		return JsonRPC_formattingWithID(key, value, j.getInt("id"));
	}
	else {
		return JsonRPC_formatting(key, value);
	}
}

String JsonRPC_formatting(String key, String value) {
	String ret = "{ \"jsonrpc\": \"2.0\", \"" + key + "\": " + value + "}";
	return ret;
}

String JsonRPC_formattingWithID(String key, String value, int id) {
	String ret = "{ \"jsonrpc\": \"2.0\", \"id\": " + id + ", \"" + key + "\": " + value + "}";
	return ret;
}

void throw_parseError(Json j) {
	String result = "Content-Type: application/json; charset=utf-8\n\n";
	if (j.hasKey("id")) {
		result = result + JsonRPC_formattingWithID("{\"code\": -32700, \"message\": \"Parse error\", \"data\": \"Requested data can not parse as Json.\"}");
	}
	else {
		result = result + JsonRPC_formatting("{\"code\": -32700, \"message\": \"Parse error\", \"data\": \"Requested data can not parse as Json.\"}");
	}
	stdout.write(result);
}

void throw_invalidRequest(Json j) {
	String result = "Content-Type: application/json; charset=utf-8\n\n";
	if (j.hasKey("id")) {
		result = result + JsonRPC_formattingWithID("{\"code\": -32600, \"message\": \"Invalid request\", \"data\": \"Requested data is invalid.\"}");
	}
	else {
		result = result + JsonRPC_formatting("{\"code\": -32600, \"message\": \"Invalid request\", \"data\": \"Requested data is invalid.\"}");
	}
	stdout.write(result);
}

void throw_methodNotFound(Json j) {
	String result = "Content-Type: application/json; charset=utf-8\n\n";
	if (j.hasKey("id")) {
		result = result + JsonRPC_formattingWithID("{\"code\": -32601, \"message\": \"Method not found\", \"data\": \"Requested method is not exists or not available.\"}");
	}
	else {
		result = result + JsonRPC_formatting("{\"code\": -32601, \"message\": \"Method not found\", \"data\": \"Requested method is not exists or not available.\"}");
	}
	stdout.write(result);
}

void throw_invalidParams(Json j) {
	String result = "Content-Type: application/json; charset=utf-8\n\n";
	if (j.hasKey("id")) {
		result = result + JsonRPC_formattingWithID("{\"code\": -32602, \"message\": \"Invalid Params\", \"data\": \"Requested parameter is invalid.\"}");
	}
	else {
		result = result + JsonRPC_formatting("{\"code\": -32602, \"message\": \"Invalid Params\", \"data\": \"Requested parameter is invalid.\"}");
	}
	stdout.write(result);
}

void throw_internalError(Json j) {
	String result = "Content-Type: application/json; charset=utf-8\n\n";
	if (j.hasKey("id")) {
		result = result + JsonRPC_formattingWithID("{\"code\": -32603, \"message\": \"Internal Error\", \"data\": \"Json-RPC's internal error.\"}");
	}
	else {
		result = result + JsonRPC_formatting("{\"code\": -32603, \"message\": \"Internal Error\", \"data\": \"Json-RPC's internal error.\"}");
	}
	stdout.write(result);
}

void throw_VersionMissMatch(Json j) {
	String result = "Content-Type: application/json; charset=utf-8\n\n";
	result = result + JsonRPC_fmt(j, "error", "{\"code\": -32604, \"message\": \"Version miss match\", \"data\": \"Json-RPC requires version 2.0.\"}");
	stdout.write(result);
}

