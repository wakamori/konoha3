Import("Deos.DCaseCloud");
Import("JavaStyle.Object");
Import("Type.Json");

Strategy makeStrategy(API repo, int argumentId, String description) {
	Json json = new Json();
	json.setString("description",description);
	json.setString("type","Strategy");
	return repo.CreateDBNode(json) as Strategy;
}

Goal makeGoal(API repo, int argumentId, String description) {
	Json json = new Json();
	json.setString("description",description);
	json.setString("type","Goal");
	return repo.CreateDBNode(json) as Goal;
}

Evidence makeEvidence(API repo, int argumentId, String description) {
	Json json = new Json();
	json.setString("description",description);
	json.setString("type","Evidence");
	return repo.CreateDBNode(json) as Evidence;
}

Context makeContext(API repo, int argumentId, Json context) {
	Json json = new Json();
	json.set("properties",context);
	json.setString("type","Context");
	return repo.CreateDBNode(json) as Context;
}

void GenerateTree1(API repo) {
	DCaseArgument argument = repo.CreateArgument("reboot must work");
	int id = argument.argument_id;

	DBNode root = repo.GetArgument(id).GetRoot();

	Json json = new Json();
	json.setString("IP","192.168.0.10");
	json.setString("PORT","1234");
	json.setString("DSE1","192.168.0.10:8080");
	repo.AddContext(id, root.node_id, json);

	Evidence e2_1 = makeEvidence(repo, id, "D-Script:sudo shutdown -r now");
	repo.InsertLink(id, root, e2_1);

	System.p(repo.GetArgument(id).toJson());
	System.p(repo.GetArgumentIds());
}

void GenerateTree2(API repo) {
	DCaseArgument argument = repo.CreateArgument("D-Script must work");
	int id = argument.argument_id;

	DBNode root = repo.GetArgument(id).GetRoot();

	Json json = new Json();
	json.setString("D-Script","sudo shutdown -r now");
	repo.AddContext(id, root.node_id, json);

	Strategy s1 = makeStrategy(repo, id, "argument by precondition/postcondition");
	repo.InsertLink(id, root, s1);

	Goal g2_1 = makeGoal(repo, id, "web server must be alive before the fact");
	repo.InsertLink(id, s1, g2_1);

	Json ctx = new Json();
	ctx.setString("DSE2","192.168.0.20:8080");
	Context c2_1 = makeContext(repo, id, ctx);
	repo.InsertLink(id, g2_1, c2_1);

	Goal g2_2 = makeGoal(repo, id, "web server must be alive after the fact");
	repo.InsertLink(id, s1, g2_2);

	Json ctx2 = new Json();
	ctx2.setString("DSE2","192.168.0.20:8080");
	Context c2_2 = makeContext(repo, id, ctx2);
	repo.InsertLink(id, g2_2, c2_2);

	Evidence e3_1 = makeEvidence(repo, id, "D-Script:telnet @{IP} @{PORT}");
	repo.InsertLink(id, g2_1, e3_1);
	repo.InsertLink(id, g2_2, e3_1);

	System.p(repo.GetArgument(id).toJson());
	System.p(repo.GetArgumentIds());
}

void main() {
	API repo = new API("dcasedb","dcasedb");
	GenerateTree1(repo);
	GenerateTree2(repo);
}

main();
