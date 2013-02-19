Load("testClient.k");

void test() {
	HttpClient client = new CurlHttpClient("http://localhost/DCaseDB/application/api/api.cgi");
	Map[String] map = new Map[String]();
	map.set("JSON", "{\"jsonrpc\":\"2.0\",\"version\":\"1.0\",\"method\":\"getNodeTree\",\"params\":{ \"BelongedArgumentId\": 1 }}");
	System.p(client.post(map));
}

test();
