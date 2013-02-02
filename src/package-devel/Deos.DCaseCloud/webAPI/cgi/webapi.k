Import("Deos.DCaseCloud");
Import("Type.Json");
Import("Type.File");
Import("Type.Bytes");
Import("posix.fd");
Import("MiniKonoha.Map");
Import("Syntax.Null");
Import("Syntax.GlobalVariable");
Import("Syntax.CStyleWhile");
Import("JavaStyle.Object");
Import("JavaScript.String");
Import("JavaScript.Array");
Import("JavaScript.Regexp");
Import("dscript.subproc");
Import("MiniKonoha.Syntax"); // for debug

Load("utils.k");

API repo = new API("dcasedb", "dcasedb");

class API_Method {
	File file;
	API_Method(){}
	boolean paramCheck(Json j) {}
	String run(Json j) {}

	boolean lock() {
		this.file = new FILE("./lock","a+");
		return System.flock(this.file.getfileno(),LOCK_EX);
	}

	void write(Json j, String method) {
		j.setString("method",method);
		this.file.write(j.toString().toBytes());
		this.file.write("\n".toBytes());
	}

	void unlock() {
		System.flock(this.file.getfileno(),LOCK_UN);
	}
}

class WebAPI {
	Map[API_Method] api;

	WebAPI() {
		this.api = new Map[API_Method];
		this.setCheckAPIMethod();
	}

	void setCheckAPIMethod() {
		api.set("create",   new API_Create());
		api.set("insert",   new API_Insert());
		api.set("update",   new API_Update());
		api.set("delete",   new API_Delete());
		api.set("get",      new API_Get());
		api.set("commit",   new API_Commit());
		api.set("revert",   new API_Revert());
		api.set("diff",     new API_Diff());
		api.set("log",      new API_Log());
		api.set("search",   new API_Search());
		api.set("traverse", new API_Traverse());
		api.set("run",      new API_Run());
	}
}
/* ----------------------------------------------------------------------- */
/* Create */

class API_Create extends API_Method {
	boolean paramCheck(Json j) {
		//boolean ret = true;
		//if (!(/Context/.test(j.getString("type")))) {
		//	if (j.hasKey("properties")) {
		//		ret = false;
		//	}
		//}
		//return ret;
		return true;
	}

	String run(Json j) {
		DBNode node = global.repo.CreateDBNode(j);
		return JsonRPC_fmt(j, "result", "{ \"id\": \"" + node.node_id.toString() + "\"}");
	}
}

/* ----------------------------------------------------------------------- */
/* Insert */

class API_Insert extends API_Method {
	boolean paramCheck(Json j) {
		return true;
	}

	String run(Json j) {
		this.write(j,"Insert");
		Json jnode = new Json();
		DBNode node;
		DBNode dist_node;
		int argument_id = j.getString("argument_id").toint();
		if (j.hasKey("new")) {
			jnode = j.get("new");
			node = global.repo.CreateDBNode(jnode);
		}
		else {
			int node_id = j.getString("node_id").toint();
			node = global.repo.GetArgument(argument_id).GetDBNode(node_id);
		}

		if (j.hasKey("parent")) {
			Json p = j.get("parent");
			int dist_args_id = p.getString("argument_id").toint();
			int dist_node_id = p.getString("node_id").toint();
			dist_node = global.repo.GetArgument(dist_args_id).GetDBNode(dist_node_id);
			global.repo.InsertLink(argument_id, dist_node, node);
		}
		else if (j.hasKey("child")) {
			Json p = j.get("child");
			int dist_args_id = p.getString("argument_id").toint();
			int dist_node_id = p.getString("node_id").toint();
			dist_node = global.repo.GetArgument(dist_args_id).GetDBNode(dist_node_id);
			global.repo.InsertLink(argument_id, node, dist_node);
		}
		Json ret = new Json();
		ret.setString("argument_id", argument_id.toString());
		ret.setString("node_id", node.node_id.toString());
		return JsonRPC_fmt(j, "result", ret.toString());
	}
}
/* ----------------------------------------------------------------------- */
/* Update */

class API_Update extends API_Method {
	boolean paramCheck(Json j) {
		return true;
	}

	String run(Json j) {
		this.write(j,"Update");
		int argument_id = j.getString("argument_id").toint();
		int node_id = j.getString("node_id").toint();
		DBNode node = global.repo.GetArgument(argument_id).GetDBNode(node_id);
		global.repo.UpdateDBNode(argument_id, node, j);
		return JsonRPC_fmt(j, "result", "\"success\"");
	}
}

/* ----------------------------------------------------------------------- */
/* Delete */

class API_Delete extends API_Method {
	boolean paramCheck(Json j) {
		return true;
	}

	String run(Json j) {
		this.write(j,"Delete");
		int argument_id = j.getString("argument_id").toint();
		int node_id = j.getString("node_id").toint();
		boolean result = global.repo.DeleteDBNode(argument_id, node_id);
		//boolean result = true; // stab
		if (result) {
			return JsonRPC_fmt(j, "result", "\"success\"");
		}
		else {
			return JsonRPC_fmt(j, "error", "{\"code\": -32004, \"Cannot delete node\", \"[webapi:Delete] cannot delete node\"}");
		}
	}
}
/* ----------------------------------------------------------------------- */
/* Get */

class API_Get extends API_Method {
	boolean paramCheck(Json j) {
		return true;
	}

	String run(Json j) {
		Json f = j.get("filter");
		String ret;
		if (j.hasKey("argument_id")) {
			System.p(f.getSize())
				ret = global.repo.getArgument(j.getString("argument_id").toint()).toJson().toString();
		}
		else {
			//if (f.getSize() == 0) {
			//	throw_invalidParams(j);
			//}
			int i = 0;
			while (i < f.getSize()){
				if (/Argument/.test(f[i].toString())) {
					//ret = "[\"0\"]";
					ret = global.repo.GetArgumentIds().toString();
				}
				i = i + 1;
			}
		}
		if (ret == "") {
			ret = "[{ \"argument_id\": \"A1\", \"node_id\": \"G1\", \"type\": \"Goal\", \"description\": \"hello, world\" },{ \"argument_id\": \"A1\", \"node_id\": \"C1\", \"type\": \"Context\", \"description\": \"hello, world\", \"properties\": [\"foo\": \"bar\", \"hoge\": \"piyo\"] }]"
		}
		return JsonRPC_fmt(j, "result", ret);
	}
}
/* ----------------------------------------------------------------------- */
/* Commit */

class API_Commit extends API_Method {
	boolean paramCheck(Json j) {
		return true;
	}

	String run(Json j) {
		if (j.hasKey("id")) {
			return JsonRPC_formattingWithID("result", "\"success\"", j.getInt("id"));
		}
		else {
			return JsonRPC_formatting("result", "\"success\"");
		}
	}
}
/* ----------------------------------------------------------------------- */
/* Revert */

class API_Revert extends API_Method {
	boolean paramCheck(Json j) {
		return true;
	}

	String run(Json j) {
		this.write(j,"Revert");
		if (j.hasKey("id")) {
			return JsonRPC_formattingWithID("result", "\"success\"", j.getInt("id"));
		}
		else {
			return JsonRPC_formatting("result", "\"success\"");
		}
	}
}
/* ----------------------------------------------------------------------- */
/* Diff */

class API_Diff extends API_Method {
	boolean paramCheck(Json j) {
		return true;
	}

	String run(Json j) {
		if (j.hasKey("id")) {
			return JsonRPC_formattingWithID("result", "{ \"TODO\": \"nikuuchi\" }", j.getInt("id"));
		}
		else {
			return JsonRPC_formatting("result", "{ \"TODO\": \"nikuuchi\" }");
		}
	}
}

/* ----------------------------------------------------------------------- */
/* Log */

class API_Log extends API_Method {
	boolean paramCheck(Json j) {
		return true;
	}

	String run(Json j) {
		if (j.hasKey("id")) {
			return JsonRPC_formattingWithID("result", "[{ \"log\": \"xxxxxx\", \"comment\": \"2nd commit\" },{ \"log\": \"yyyyyy\", \"comment\": \"1st commit\" }]", j.getInt("id"));
		}
		else {
			return JsonRPC_formatting("result", "[{ \"log\": \"xxxxxx\", \"comment\": \"2nd commit\" },{ \"log\": \"yyyyyy\", \"comment\": \"1st commit\" }]");
		}
	}
}
/* ----------------------------------------------------------------------- */
/* Search */

class API_Search extends API_Method {
	boolean paramCheck(Json j) {
		return true;
	}

	String run(Json j) {
		String keyword = j.getString("keyword");
		Json f = j.get("filter");
		int i = 0;
		Json ret = Json.parse("[]");
		while (i < f.getSize()) {
			if (/Context/.test(f[i].toString())) {
				ret.add(global.repo.SearchContext(keyword));
			}
			else {
				ret.add(global.repo.Search(keyword, f[i].toString()));
			}
			i = i + 1;
		}
		return JsonRPC_fmt(j, "result", ret.toString());
	}
}

/* ----------------------------------------------------------------------- */
/* Traverse */

class API_Traverse extends API_Method {
	boolean paramCheck(Json j) {
		return true;
	}

	String run(Json j) {
		if (j.hasKey("id")) {
			return JsonRPC_formattingWithID("result", "[{ \"argument_id\": \"A1\", \"node_id\": \"G1\", \"type\": \"Goal\", \"description\": \"hello, world\" },{ \"argument_id\": \"A1\", \"node_id\": \"C1\", \"type\": \"Context\", \"description\": \"hello, world\", \"properties\": [\"foo\": \"bar\", \"hoge\": \"piyo\"] }]", j.getInt("id"));
		}
		else {
			return JsonRPC_formatting("result", "[{ \"argument_id\": \"A1\", \"node_id\": \"G1\", \"type\": \"Goal\", \"description\": \"hello, world\" },{ \"argument_id\": \"A1\", \"node_id\": \"C1\", \"type\": \"Context\", \"description\": \"hello, world\", \"properties\": [\"foo\": \"bar\", \"hoge\": \"piyo\"] }]");
		}
	}
}
/* ----------------------------------------------------------------------- */
/* Run */

class API_Run extends API_Method {
	boolean paramCheck(Json j) {
		return true;
	}

	String run(Json j) {
		SubProc sp = new SubProc("/usr/local/bin/minikonoha");
		sp.setArgumentList(["-DUSER=dcasedb", "-DPASSWORD=dcasedb", "/home/tsunade/minikonoha/src/package-devel/Deos.DScript/sample/assure.k"]);
		sp.bg();
		return JsonRPC_fmt(j, "result", "\"" + sp.communicate("")[0].replace("\n", "\\n").replace("\"", "\\\"") + "\"");
	}
}
CompileAllDefinedMethod(); //for debug
