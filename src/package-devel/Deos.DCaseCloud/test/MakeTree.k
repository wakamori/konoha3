Load("../dcasecloud_glue.k");
Import("JavaStyle.Object");
//Import("konoha.object");

Strategy makeStrategy(API repo, int argumentId, String description) {
	Json json = new Json();
	json.setString("description",description);
	json.setString("type","Strategy");
	return repo.CreateDBNode(argumentId,json) as Strategy;
}

Goal makeGoal(API repo, int argumentId, String description) {
	Json json = new Json();
	json.setString("description",description);
	json.setString("type","Goal");
	return repo.CreateDBNode(argumentId, json) as Goal;
}

Evidence makeEvidence(API repo, int argumentId, String description) {
	Json json = new Json();
	json.setString("description",description);
	json.setString("type","Evidence");
	return repo.CreateDBNode(argumentId, json) as Evidence;
}

Context makeContext(API repo, int argumentId, String description) {
	Json json = new Json();
	json.setString("description",description);
	json.setString("type","Context");
	return repo.CreateDBNode(argumentId, json) as Context;
}

void main() {
	API repo = new API("dcasedb","dcasedb");

	DCaseArgument argument = repo.CreateArgument("Basic Syntax");
	int id = argument.argument_id;

	DBNode root = repo.GetArgument(id).GetRoot();

	Strategy s1 = makeStrategy(repo, id, "Separated by Syntax");
	repo.InsertLink(id, root, s1);

	Goal g2_1 = makeGoal(repo, id, "Function Decralation");
	repo.InsertLink(id, s1 , g2_1);

	Goal g2_2 = makeGoal(repo, id, "Return Statement");
	repo.InsertLink(id, s1 , g2_2);

	System.p(repo.GetArgument(id).toJson());
	System.p(repo.GetArgumentIds());
}

main();
