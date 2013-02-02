load("../dcasecloud_glue.k");


void main() {

	API repo = new API("dcasedb","dcasedb");
	Json Ids = repo.GetArgumentIds();
	DCaseArgument argument = repo.GetArgument(Ids[1].getString("argument_id").toint());
	System.p(argument.toJson());

	System.p(repo.GetContext(argument.GetDBNode(7)));
}

main();
