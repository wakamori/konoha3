load("../dcasedb_glue.k");
import("konoha.object");

Evidence makeEvidence(DCaseDB repo, String treeName, String name,String description, boolean result) {
	Json json = new Json();
	json.setString("name",name);
	json.setString("description",description);
	json.setBoolean("is_evidence",result);
	json.setString("DBNodeType","Evidence");
	return repo.CreateDBNode(treeName, json) as Evidence;
}

void main() {
	String TreeName = "BasicSyntax";
	DCaseDB repo = new DCaseDB();
	repo.Connect("dcasedb","dcasedb");
	repo.SearchArgument(TreeName);

	Evidence falseEvidence = makeEvidence(repo, TreeName, "Evidence1", "Always Fail test script",false);
	Evidence trueEvidence = makeEvidence(repo, TreeName, "Evidence2", "test script2",true);
	Goal g1 = repo.SearchDBNode(TreeName,"ReturnStatement") as Goal;
	Goal g2 = repo.SearchDBNode(TreeName,"FunctionDeclaration") as Goal;

	repo.InsertLink(TreeName, g1.name , falseEvidence.name);
	repo.InsertLink(TreeName, g2.name , trueEvidence.name);

	System.p(repo.SearchArgument(TreeName).toJson());
}

main();
