import("Syntax.Null");
import("Syntax.CStyleWhile");
import("Syntax.GoStyleIncrement");
import("Syntax.GlobalVariable");
import("Type.Bytes");
import("Type.File");
import("Type.Json");
import("Java.Class");
import("JavaScript.String");
import("JavaScript.Array");
import("JavaScript.Date");
import("Lib.Curl");
import("MiniKonoha.NameSpace");
import("MiniKonoha.Map");
import("MiniKonoha.EventListener");
import("posix.process");

int cid = new Date().getTime();
int tid = 1;
String localhost;
Json DTaskStatusMap = new Json(); // TODO replace it to Map[Json]

const running = 0;
const done    = 1

void DTaskWatcher(Event event) {
	String id = event.getProperty("cid") + "+" + event.getInt("tid");
	Json response = new Json();
	response.setInt("code", event.getInt("code"));
	global.DTaskStatusMap.set(id, response);
}

class DControl {
	@Public @Static void start(String host, int port) {
		global.localhost = "http://" + host + ":" + port;
		EventHandler.setCallBack("dse", DTaskWatcher);
		HttpEventListener.start(host, port);
	}

	@Public @Static void start(int port) {
		global.localhost = "http://" + "0.0.0.0:" + port;
		EventHandler.setCallBack("dse", DTaskWatcher);
		HttpEventListener.start("0.0.0.0", port);
	}
}

String readFile(String path) {
	FILE file = new FILE(path, "rb");
	int len = 0;
	String tmp = "";
	while(true) {
		int ch = file.getc();
		if(ch == -1) {
			break;
		}
		len = len + 1;
	}
	file.close();

	if (len == 0) {
		return "";
	}

	Bytes buf = new Bytes(len);
	file = new FILE(path, "r");
	file.read(buf, 0, len);
	file.close();
	return new String(buf);
}

class DTask {
	String id;
	String script;
	DTask next;

	DTask(String filePath) {
		this.id = null;
		this.script = readFile(filePath).replace("\"", "\\\"");
		this.next = null;
	}

	boolean execute(String IP) {
		Json request = new Json();
		request.setString("method", "ExecuteScript");
		request.setString("from", global.localhost);
		Json params = new Json();
		params.setString("scripttype", "D-Task");
		params.setString("script", this.script);
		params.setString("cid", "" + global.cid);
		params.setInt("tid", global.tid);
		this.id = "" + global.cid + "+" + global.tid;
		global.tid += 1;
		request.set("params", params);
		Json response = null;
		global.DTaskStatusMap.set(this.id, response);
		Curl curl = new Curl();
		curl.setOpt(CURLOPT_URL, IP);
		curl.setOpt(CURLOPT_POSTFIELDS, request.toString());
		curl.perform();
		while(true) {
			response = global.DTaskStatusMap.get(this.id);
			if(response.toString() != "{}") {
				System.p("response : " + response);
				break;
			}
			System.usleep(10000);
		}
		return true; // FIXME
	}
}

class Dse {
	String IP;
	DTask[] dtasks;

	@Public Dse(String host, int port) {
		this.IP = "http://" + host + ":" + port;
		dtasks = [];
	}

	@Public Dse(String host) {
		this.IP = "http://" + host + ":" + 8080;
		dtasks = [];
	}

	@Public void appendDTask(DTask dtask) {
		this.dtasks.add(dtask);
	}

	@Public void appendDTask(String filePath) {
		this.dtasks.add(new DTask(filePath));
	}

	@Public boolean dispatchDTasks() {
		int i = 0;
		int size = this.dtasks.getSize();
		while(i < size) {
			this.dtasks[i].execute(this.IP);
			i++;
		}
	}
}

class DseGroup {
	Dse[] dses;

	@Public DseGroup() {
		this.dses = [];
	}

	@Public void appendDse(Dse dse) {
		this.dses.add(dse);
	}

	@Public void appendDse(String host, int port) {
		this.dses.add(new Dse(host, port));
	}

	@Public void appendDTask(String filePath) {
		this.dses.add(new Dse(host));
	}

	@Public void appendDTask(DTask dtask) {
	}

//	@Public void appendDTask(String filePath) {
//	}
}

void test() {
	DControl.start("0.0.0.0", 8090);
	Dse dse = new Dse("0.0.0.0");
	dse.appendDTask("hoge.k");
	dse.dispatchDTasks();
}

test();
