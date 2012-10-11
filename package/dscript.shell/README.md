dscript.shell仕様案
===================

文法
----

変数宣言
--------

	a=1		# => var a = 1;
	a="hi"	# => var a = "hi";
	a=1.23	# => var a = 1.23;
	$a		# => a;

特殊変数
--------

	$n		# SCRIPT_ARGV.get(n-1)
	$#		# SCRIPT_ARGV.getSize()
	$@		# SCRIPT_ARGV.slice(1)
	($?		# 最後に実行したコマンドの終了ステータス)
	($!		# 最後に実行したバックグラウンドコマンドのPID)
	$$		# System.getpid()
	($-		# 現在のオプションフラグ)

算術演算子(C + bash)
--------------------

	expr num1 + num2	# num1 +  num2
	expr num1 - num2	# num1 -  num2
	expr num1 * num2	# num1 *  num2
	expr num1 / num2	# num1 /  num2
	expr num1 % num2	# num1 %  num2
	expr num1 & num2	# num1 && num2
	expr num1 | num2	# num1 || num2
	expr num1 = num2	# num1 == num2
	expr num1 > num2	# num1 >  num2
	expr num1 < num2	# num1 <  num2
	expr num1 >= num2	# num1 >= num2
	expr num1 <= num2	# num1 <= num2
	expr num1 != num2	# num1 != num2

コメント
--------

	# comment

条件分岐(C)
-----------

	if ( cond ) {
	}
	else {
	}

条件式
------

	# ファイル形式
	-b file # fileがブロックデバイスなら真
	-c file # fileがキャラクタデバイスなら真
	-d file # fileがディレクトリなら真
	-f file # fileが通常ファイルなら真
	-L file # fileがシンボリックリンクなら真
	-p file # fileが名前付きパイプなら真
	-S file # fileがソケットなら真
	# ファイルパーミッション
	-g file # fileにSGIDがセットされていれば真
	-k file # fileにスティッキービットがセットされていれば真
	-r file # fileが読み取り可能なら真
	-u file # fileにSUIDがセットされていれば真
	-w file # fileが書き込み可能なら真
	-x file # fileが実行可能なら真
	# その他ファイルチェック
	-e file # fileが存在すれば真
	-s file # fileのファイルサイズが0より大きければ真
	# 文字列
	-n str # strの長さが0より大きければ真
	-z str # strの長さが0であれば真
	str1 = str2 # 2つの文字列が等しければ真
	str1 != str2 # 2つの文字列が等しくなければ真
	# 数値
	num1 -eq num2 # 2つの数値が等しければ真
	num1 -ge num2 # 数値1が数値2以上であれば真
	num1 -gt num2 # 数値1が数値2より大きいのであれば真
	num1 -le num2 # 数値1が数値2以下であれば真
	num1 -lt num2 # 数値1が数値2未満であれば真
	num1 -ne num2 # 2つの数値が等しくなければ真
	# 論理結合
	!条件 条件が偽であれば真
	条件1 -a 条件2 # 条件1と条件2の両方が真であれば真
	条件1 -o 条件2 # 条件1と条件2のどちらかが真であれば真

保留
====

文字列演算(bash)
----------------

	文字列 : 正規表現				# 文字列と正規表現が一致し、正規表現に"\(" と "\)"が使われていればマッチした文字列を返す。そうでないなら マッチした文字列の長さを返す。また、一致しなかった場合"\(" と "\)"が使われていればNULLを返し、そうでないなら 0を返す。
	index 文字列1 文字列2			# 文字列1から文字列2のいずれかの文字が最初に見つかった位置を返す。見つからなければ0を返す。
	length 文字列					# 文字列の長さを返す。
	match 文字列 正規表現			# "文字列 : 正規表現"と同じ。
	quote 文字列					# 文字列に演算子やキーワードを含んでも通常の文字列として扱う。
	substr 文字列 文字位置 文字列長	# 文字列の部分文字列を返す。部分文字列は文字位置から始まり、文字列長の長さである。文字位置や文字列長が正の数値でない場合は NULLを返す。
