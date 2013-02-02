Import("Deos.DCaseCloud");
Import("Type.Json");


void main() {

	API repo = new API("dcasedb","dcasedb");
	Json Ids = repo.GetArgumentIds();
	DCaseArgument argument = repo.GetArgument(Ids.get(1).getString("argument_id").toint());
	System.p(argument.toJson());
}

main();
