Load("testClient.k");

void test() {
	HttpClient client = new CurlHttpClient("http://localhost/DCaseDB/application/api/api.cgi");
	Map[String] map = new Map[String]();
	map.set("JSON", "{\"method\":\"getArgumentList\",\"params\":{}}");
	System.p(client.post(map));
}

test();
