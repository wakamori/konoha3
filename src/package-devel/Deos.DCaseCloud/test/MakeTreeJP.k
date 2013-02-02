load("../dcasedb_glue.k");
import("konoha.object");

Strategy makeStrategy(DCaseDB repo, String treeName, String name,String description) {
	Json json = new Json();
	json.setString("name",name);
	json.setString("description",description);
	json.setString("DBNodeType","Strategy");
	return repo.CreateDBNode(treeName,json) as Strategy;
}

Goal makeGoal(DCaseDB repo, String treeName, String name,String description) {
	Json json = new Json();
	json.setString("name",name);
	json.setString("description",description);
	json.setString("DBNodeType","Goal");
	return repo.CreateDBNode(treeName, json) as Goal;
}

Evidence makeEvidence(DCaseDB repo, String treeName, String name,String description) {
	Json json = new Json();
	json.setString("name",name);
	json.setString("description",description);
	json.setString("DBNodeType","Evidence");
	return repo.CreateDBNode(treeName, json) as Evidence;
}

Context makeContext(DCaseDB repo, String treeName, String name, String description) {
	Json json = new Json();
	json.setString("name",name);
	json.setString("description",description);
	json.setString("DBNodeType","Context");
	return repo.CreateDBNode(treeName, json) as Context;
}

void main() {
	String TreeName = "BasicSyntax";
	DCaseDB repo = new DCaseDB();
	repo.Connect("dcasedb","dcasedb");
	repo.CreateArgument(TreeName,"基本的な構文");

	Goal root = repo.SearchArgument(TreeName).root as Goal;

	Context c = makeContext(repo, TreeName, "BasicSyntaxContext", "コンテキスト");
	repo.InsertLink(TreeName, root.name, c.name);

	Strategy s1 = makeStrategy(repo, TreeName, "Strategy1", "機能ごとに分ける");
	repo.InsertLink(TreeName, root.name, s1.name);

	Goal g2_1 = makeGoal(repo, TreeName, "FunctionDeclaration", "関数定義");
	repo.InsertLink(TreeName, s1.name , g2_1.name);

	Goal g2_2 = makeGoal(repo, TreeName, "ReturnStatement", "Return文");
	repo.InsertLink(TreeName, s1.name , g2_2.name);

	System.p(repo.SearchArgument(TreeName).toJson());
}

main();
