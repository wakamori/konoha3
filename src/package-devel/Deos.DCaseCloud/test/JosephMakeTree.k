Import("Deos.DCaseCloud");
Import("JavaStyle.Object");
Import("Type.Json");
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

Evidence makeEvidence(API repo, int argumentId, String description, boolean flag) {
	Json json = new Json();
	json.setString("description",description);
	json.setBoolean("isEvidence",flag);
	json.setString("type","Evidence");
	return repo.CreateDBNode(argumentId, json) as Evidence;
}

Context makeContext(API repo, int argumentId, String description) {
	Json json = new Json();
	json.setString("description",description);
	json.setString("type","Context");
	return repo.CreateDBNode(argumentId, json) as Context;
}

void GenerateTree1(API repo) {
	DCaseArgument argument = repo.CreateArgument("診断スクリプトが正常に動作する");
	int id = argument.argument_id;

	DBNode root = repo.GetArgument(id).GetRoot();

	Json json = new Json();
	json.setString("IP","192.168.0.1");
	json.setString("PORT","1234");
	json.setString("DEST","192.168.0.2");
	repo.AddContext(id, root.node_id, json);

	Strategy s1 = makeStrategy(repo, id, "レイヤーレベルで議論する");
	repo.InsertLink(id, root, s1);

	Goal g2_1 = makeGoal(repo, id, "ネットワークインターフェース層は正常である");
	repo.InsertLink(id, s1 , g2_1);

	Goal g2_2 = makeGoal(repo, id, "インターネット層は正常である");
	repo.InsertLink(id, s1 , g2_2);

	Goal g2_3 = makeGoal(repo, id, "トランスポート層は正常である");
	repo.InsertLink(id, s1 , g2_3);

	Goal g2_4 = makeGoal(repo, id, "アプリケーション層は正常である(名前解決可能である)");
	repo.InsertLink(id, s1 , g2_4);
	Evidence e2_4 = makeEvidence(repo, id, "Nslookup.ds", true);
	repo.InsertLink(id, g2_4, e2_4);

	Strategy s2 = makeStrategy(repo, id, "PCや周辺機器の状態により議論する");
	repo.InsertLink(id, g2_1 , s2);

//	Goal g3_1 = makeGoal(repo, id, "イーサネットケーブルが途切れていない");
//	repo.InsertLink(id, s2, g3_1);
//	Evidence e3_1 = makeEvidence(repo, id, "人による確認");
//	repo.InsertLink(id, g3_1, e3_1);

	Goal g3_1 = makeGoal(repo, id, "イーサネットカードが認識されている");
	repo.InsertLink(id, s2, g3_1);
	Evidence e3_1 = makeEvidence(repo, id, "CheckNIC.ds", true);
	repo.InsertLink(id, g3_1, e3_1);

	Goal g3_2 = makeGoal(repo, id, "イーサネットインターフェースが有効になっている");
	repo.InsertLink(id, s2, g3_2);
	Evidence e3_2 = makeEvidence(repo, id, "Connection.ds", true);
	repo.InsertLink(id, g3_2, e3_2);

	Strategy s3 = makeStrategy(repo, id, "インターネット層の持つ役割を基に議論する");
	repo.InsertLink(id, g2_2 , s3);

	Goal g4_1 = makeGoal(repo, id, "IPアドレスが割り当てられている");
	repo.InsertLink(id, s3, g4_1);
	Evidence e4_1 = makeEvidence(repo, id, "CheckIPAddress.ds", true);
	repo.InsertLink(id, g4_1, e4_1);

	Goal g4_2 = makeGoal(repo, id, "ルーティング可能である");
	repo.InsertLink(id, s3, g4_2);

	Goal g4_3 = makeGoal(repo, id, "firewall設定によりIPレベルでパケットが破棄されない");
	repo.InsertLink(id, s3, g4_2);

	Strategy s4 = makeStrategy(repo, id, "ルーティングテーブルを基に議論する");
	repo.InsertLink(id, g4_2 , s4);

	Goal g5_1 = makeGoal(repo, id, "直接ルーティング可能である");
	repo.InsertLink(id, s4, g5_1);
	Evidence e5_1 = makeEvidence(repo, id, "RoutingDirectly.ds", true);
	repo.InsertLink(id, g5_1, e5_1);

	Goal g5_2 = makeGoal(repo, id, "ゲートウェイを通してルーティング可能である");
	repo.InsertLink(id, s4, g5_2);
	Evidence e5_2 = makeEvidence(repo, id, "RoutingDefault.ds", true);
	repo.InsertLink(id, g5_2, e5_2);

//	Goal g5_3 = makeGoal(repo, id, "ルータが壊れていない");
//	repo.InsertLink(id, s4, g5_3);
//	Evidence e5_3 = makeEvidence(repo, id, "人による確認");
//	repo.InsertLink(id, g5_3, e5_3);

	Strategy s5 = makeStrategy(repo, id, "INPUT, FORWARD, OUTPUT別に確認する");
	repo.InsertLink(id, g4_3, s5);

	Goal g6_1 = makeGoal(repo, id, "INPUTチェーンではパケットを受け入れている");
	repo.InsertLink(id, s5, g6_1);
	Evidence e6_1 = makeEvidence(repo, id, "FirewallIPInput.ds", true);
	repo.InsertLink(id, g6_1, e6_1);

	Goal g6_2 = makeGoal(repo, id, "FORWARDチェーンではパケットを受け入れている");
	repo.InsertLink(id, s5, g6_2);
	Evidence e6_2 = makeEvidence(repo, id, "FirewallIPForward.ds", true);
	repo.InsertLink(id, g6_2, e6_2);

	Goal g6_3 = makeGoal(repo, id, "OUTPUTチェーンではパケットを受け入れている");
	repo.InsertLink(id, s5, g6_3);
	Evidence e6_3 = makeEvidence(repo, id, "FirewallIPOutput.ds", true);
	repo.InsertLink(id, g6_3, e6_3);

	Strategy s6 = makeStrategy(repo, id, "firewall設定を考慮する");
	repo.InsertLink(id, g2_3, s6);

	Goal g7_1 = makeGoal(repo, id, "受信するTCPプロトコルのパケットを破棄しない");
	repo.InsertLink(id, s6, g7_1);
	Evidence e7_1 = makeEvidence(repo, id, "FirewallTCPInput.ds", true);
	repo.InsertLink(id, g7_1, e7_1);

	Goal g7_2 = makeGoal(repo, id, "中継するTCPプロトコルのパケットを破棄しない");
	repo.InsertLink(id, s6, g7_2);
	Evidence e7_2 = makeEvidence(repo, id, "FirewallTCPForward.ds", true);
	repo.InsertLink(id, g7_2, e7_2);

	Goal g7_3 = makeGoal(repo, id, "送信するTCPプロトコルのパケットを破棄しない");
	repo.InsertLink(id, s6, g7_3);
	Evidence e7_3 = makeEvidence(repo, id, "FirewallTCPOutput.ds", true);
	repo.InsertLink(id, g7_3, e7_3);

	System.p(repo.GetArgument(id).toJson());
	System.p(repo.GetArgumentIds());
}

void main() {
	API repo = new API("dcasedb","dcasedb");
	GenerateTree1(repo);
}

main();
