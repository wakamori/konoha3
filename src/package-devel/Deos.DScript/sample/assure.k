Import("Deos.DScript");
Import("JavaScript.Array");

/*
 * reboot.ds
 */
assure "サービスが正常に運用できる" {
	assure "再起動が正常に動作する" {
		System.p("sudo shutdown -r now");
	}
	assure "Webサーバが正常に動作している" {
		System.p("telnet " + @{IP} + " " + @{PORT});
	}
	argue "再起動が正常に動作する" {
		strategy pre-post {
			assure "事前にWebサーバが正常に動作している" {
				System.p("=== pre  start ===");
				@[Webサーバが正常に動作している];
				System.p("=== pre  end   ===");
			}
			assure "事後にWebサーバが正常に動作している" {
				System.p("=== post start ===");
				@[Webサーバが正常に動作している];
				System.p("=== post end   ===");
			}
		}
	}
}

void test() {
	String execcmd = "サービスが正常に運用できる";
	if(SCRIPT_ARGV.getlength() == 2) {
		execcmd = SCRIPT_ARGV[1];
	}
	eval("@[" + execcmd + "];");
}

test();
