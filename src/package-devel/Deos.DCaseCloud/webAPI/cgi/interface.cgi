#! /usr/local/bin/minikonoha

Load("webapi.k");

void main() {
	Json j = Json.parse(getMsg());
	WebAPI api = new WebAPI();
	if (!checkVersion(j.getInt("version"))) {
		throw_VersionMissMatch(j);
		return;
	}
	if (!checkID(j)) {
		// add id
	}
	API_Method method = api.api[j.getString("method")];
	if (!method.paramCheck(j.get("params"))) {
		// error handling
	}
	method.lock();
	String result = method.run(j.get("params"));
	flush(result);
	method.unlock();
}

main();
