load("../dcasecloud_glue.k");


void main() {

	API repo = new API("dcasedb","dcasedb");
	System.p(repo.Search("Syntax"));
	System.p(repo.Search("Syntax","Goal"));
}

main();
