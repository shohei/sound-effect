                              このフォルダの内容について
　　　                                                     平成24年10月26日
                                                        　　　   小坂直敏

　このフォルダには、本文で説明しているプログラムのソースコードを中心に関連する
ファイルを載せている。また、32bitマシンでコンパイルしたobjファイルが残っている。
新たに実行する方は、これらを一旦削除してもよく、基本的には内部のコードからobjファイル
が生成できるようになっている。

●配布物のヴァージョンについて
　　Revision.txtに配布物の履歴があるため、かならず参照のこと。

●文書ファイルついて
　Program_id.xls 　内容物の説明
　Revision.txt   これまでの文書、およびプログラムの更新履歴

●サウンドについて
　プログラムで用いた出力も併せて残してある。
　本プログラム以外で作成された音、サンプル音は以下のとおり。
　   fot.bak.441.wav 女声　短文章
     fot.bak.44.wav　同上
     summer2006_5.wav　finaleで作成したピアノサンプル音
　　フォルダ　summer2006  summer2006_5.wav 関連のヴァリエーション

●本プログラムはWindows XP, Windows Vista, Windows 7で動作確認済み。
　　　　　　　　また、32bitマシンだけでなく、64bitマシンでも動作確認済み。

●コンパイラからの警告について
　bcc32では、変数を定義しておきながら、プログラムで使用しない場合はW8057のコードで、
　また、変数に代入しておきながら、その行以降でその変数を使用しない場合W8005 のコードで
　警告が出る。
　警告 W8057の例：
 　int abc;
   以後の本文ではabc を一切使用しない場合。

　警告 W8004 の例:
  　int def;
    def= 1.5*4;
    この行以降で　右辺にdefが来る式がない場合。
 
本文や本サイトで紹介しているプログラムには、これらの警告が出るものがある。
W8057はプログラムの開発途上で必要であった変数が最終的に不要となり、建築物の養生や
足場と同様に、完成時には撤去(削除)すべきであるがこれを削除し忘れたものである。

W8004も同様に、開発途上で必要とあった計算が、完成時にはその計算結果を利用しない場合など
がある。

これらは、本来完全に消し去るべきである、との考えが自然ではあるが、ここでは、以下の
考えにより、多少の警告を残したままとする。

特に６章のPv, Sinuなどの応用プログラムは、中間パラメータも多少は変更時には必要との判断と
以下の本コンパイラの特有の事情により、本来警告すべきでないものも警告とすることになる。

警告なしの例
    nwdw=2*L*nfft+1;
    hamwdw1(hwdw,nwdw);

警告 W8004 の例:
    hamwdw1(hwdw,(nwdw=2*L*nfft+1));

プログラムの動作はまったく同一にもかかわらず、これらは警告が出る。

これらの警告が煩わしい場合は、以下のコンパイルスイッチを使用することを勧める。
例プログラムtest.cの警告を表示させない場合。

    bcc32 -w-8057 test.c
    bcc32 -w-8004 test.c

●プログラムについて
　本文とともにお読みください。
---------------
3.4 簡単なプログラム【リスト3.1】
実行ファイル作成 および実行
 bcc32 hello.c
 hello
---------------
3.5 音ファイルの入出力　wavreadtest.c【リスト3.2, 3.3, 3.4, 3.5】
実行ファイル作成
 bcc32 -c wavread.c
 bcc32 -c wavwrite.c
 bcc32 wavreadtest.c wavread.obj wavwrite.obj
実行
 wavreadtest inputsound.wav outputsound.wav

---------------
3.7 正弦波音ファイルの作成　sine.c【リスト3.6】
実行ファイル作成（コンパイル・リンク）および実行例
 bcc32 sine.c wavwrite.obj
 sine 88200 10000 440 44100 3.6.sine.wav

==========================================================================
Windows APIを用いたプログラム
参考資料
1. マルチメディアプログラミング
http://www13.plala.or.jp/kymats/study/multimedia.html

2. Atelier Blue内　WAVEファイルのデータをリアルタイムに書き換えて再生する
http://www.atelier-blue.com/memo/memo2006-4-22.htm

---------------
3.8.1 ウィンドウの表示　winapi_test.c【リスト3.7】
実行ファイル作成　および実行
 bcc32 -W winapi_test.c
 winapi_test

コンパイル時に以下の警告が出る。
Borland C++ 5.5.1 for Win32 Copyright (c) 1993, 2000 Borland
winapi_test.c:
警告 W8057 winapi_test.c 49: パラメータ 'hPrevInstance' は一度も使用されない(関数 WinMain )
警告 W8057 winapi_test.c 49: パラメータ 'lpCmdLine' は一度も使用されない(関数 WinMain )
警告 W8057 winapi_test.c 49: パラメータ 'nCmdShow' は一度も使用されない(関数 WinMain )
Turbo Incremental Link 5.00 Copyright (c) 1997, 2000 Borland

これらの警告は変数を定義しているのに使用していないことに対する警告である。
プログラム使用上の実際に不都合はない。この例では、include fileのwindows.h
 の中でさらにincludeしているwinbase.h の中で、1262-1267行目

WinMain(
    IN HINSTANCE hInstance,
    IN HINSTANCE hPrevInstance,
    IN LPSTR lpCmdLine,
    IN int nShowCmd
    );
と定義しており、この定義に沿ってリスト3.7のようにプログラム上で必要のない
変数を記述している場合に生ずる警告である。一方、これらの変数を除去すると、
この定義に反する意味で、逆にコンパイル時にエラーとなる。

本書では、Windows APIの簡単な紹介にとどめるため、include fileを利用してい
る。一方、これらの変数をダミーで使用するような無駄な処理は行わないため、
コンパイル時の警告は不可避なものとして登場する。以降のWindows API使用時の
プログラムも同様な現象が出る。
これを上記を抑えるために、-w-8057を加える。

bcc32 -W -w-8057 winapi_test.c

---------------
3.8.2 ボタン・ＯＫボタンの表示と簡単な描画　winapi_test1.c【リスト3.8】
実行ファイル作成　および実行
 bcc32 -W winapi_test1.c
 winapi_test1

警告を抑える場合の実行ファイル作成
　bcc32 -W -w-8057 winapi_test1.c

---------------
3.9.1 ディスクファイルからの再生　wav_multi_file.c【リスト3.9】 
実行ファイル作成　および実行
 bcc32 -W wav_multi_file.c wavread.obj
 wav_multi_file

エラーが起こる場合は、リスト3.9の15行目を見られたい。
本文中にあるように、作業フォルダがc:¥srcでないとエラーが起こるため、注意を
要する。
なお、ソースコードは、上記参考資料　2)を参考にし以下の点を考慮して製作した。
1. ステレオ信号の加工再生をモノラル信号の再生とした。
2. 音ファイル関係を特別なopenWaveFile関数を用いずにwavreadを用いた。
3. 簡易化するため、349行のプログラムから194行とした。

注：警告W8060も本体の動作には影響しない。
【警告を非表示にする実行ファイル作成】
 bcc32 -W -w-8060 -w-8057 wav_multi_file.c wavread.obj

---------------
3.9.2 関数波形のリアルタイム再生　wav_multi.sine.c【リスト3.10】
実行ファイル作成　および実行
 bcc32 -W wav_multi.sine.c
 wav_multi.sine

操作：ウィンドウ内で左クリックで再生、右クリックで停止。ウィンドウを消去して
も音は停止する。
本プログラムの特徴
9行目
double f=357.43 ; // 信号の周波数〔Hz〕
とあるように、任意周波数の正弦波が途切れずに、永久に継続して再生する。

---------------
3.9.3 鍵盤制御によるハーモにクス音の制御　wav_multi.harm.c【本文中リストなし】
実行ファイル作成　および実行
 bcc32 -W wav_multi.harm.c
 wav_multi.harm

【警告を除去した修正版】　wav_multi.harm.v1.c
実行ファイル作成　および実行
 bcc32 -W -w-8057 wav_multi.harm.v1.c
 wav_multi.harm.v1

操作：ウィンドウ内の鍵盤内で左クリックで再生、右クリックで停止。ウィンドウを
消去しても音は停止する。なお、停止と再生を交互に行うことは考慮されておらず、
通常は一回停止すると音の再生はなされない。敢えて行うためには、鍵盤上で右クリ
ックで停止したあと、一旦ウィンドウ内の鍵盤外へ出て、左クリックをし、一定時間
経たのち、鍵盤内に戻ると初期状態に戻り再生可能となる。

---------------
3.9.4 波形テーブル合成 wavtable0.c【本文中リストなし】
実行ファイル作成　および実行
 bcc32 -W wavtable0.c
 wavtable0

操作：ウィンドウ内の鍵盤内で左クリックで再生、右クリックで停止。トラックバー
の位置をマウスのドラグにより移動することにより、正弦波の周波数を変更できる。
周波数は表示される。特徴：関数sine_func内で、周波数が異なるときスムーズに移行
できるよう振幅を補間しながら接続を行う（クロスフェード）

【警告を非表示にする実行ファイル作成】
bcc32 -W -w-8057 wavtable0.c

==========================================================================
4.1　関数波形の合成 func_gen.c【リスト4.1】
実行ファイル作成　および実行
 bcc32 func_gen.c wavwrite.obj
 func_gen func# n_data amp f fs filename

実行例
 白雑音 2秒　振幅:10000 (440はダミー) サンプリング周波数:44100Hz
　func_gen 1 88200 10000 440 44100 outsound.wav

 三角波　2秒 振幅:10000 信号周波数:440Hz サンプリング周波数:44100Hz
　func_gen 2 88200 10000 440 44100 outsound.wav

---------------
4.2 変調合成　func_gen1.c 【リスト4.2(抜粋)】
抜粋は上記プログラムの42-51行目、および 62-83行目
実行ファイル作成　および実行
 bcc32 func_gen1.c wavwrite.obj
 func_gen1 func# n_data amp f(fc) fm I fs filename

注：ソースコード中cons1,cons2なる変数は不要でコンパイル時も警告が出るが、
実行上のエラーにはならない。本文との対応上、これらを除去修正しないプログラム
を残す。
修正版
 func_gen1.v1.c

実行例:
正弦波のように、これほど多くの引数がいらない波形合成も、ダミーの値を必ず入れる。
以下の正弦波の場合で1500と3.0、またRMの2 はダミー数値である。

正弦波　2秒　振幅:10000 信号周波数: 440Hz サンプリング周波数:44100Hz 
         出力ファイル:outsound.wav
func_gen1 0 88200 10000 440 1500 3.0 44100 outsound.wav

RM 2秒　振幅:10000 搬送周波数fc: 440Hz 変調周波数fm: 2Hz サンプリング周波数:
         44100Hz　出力ファイル:outsound.wav
func_gen1 2 88200 10000 440 2 3.0 44100 outsound.wav

FM 2秒  振幅:10000 搬送周波数fc: 440Hz 変調周波数fm: 80Hz   変調指数I: 5.0
         2Hz サンプリング周波数:44100Hz     出力ファイル:outsound.wav
func_gen1 4 88200 10000 440 80 5.0 44100 outsound.wav

---------------
4.3 波形テーブル合成
　3.9.4 参照

---------------
4.3.2 掃引関数による波形テーブル合成
実行ファイル作成　および実行
 bcc32 -W wavtable_sweep.c
 wavtable_sweep

操作は　3.9.4 波形テーブル合成の操作と同様。

【警告を除去した修正版】 wavtable_sweep.v1.c
実行ファイル作成　および実行
 bcc32 -W -w-8057 wavtable_sweep.v1.c

---------------
4.4 Karplus-Strong方式による撥弦音の合成 Karplus.c 【リスト4.5】
実行ファイル作成　および実行
 bcc32 Karplus.c wavwrite.obj
 Karplus n_data amp f fs filename (本文p93 10行目はfが記載漏れ,
　また同様にリスト4.5内もこれと連動してfが記載漏れとなっている。
　ここでのKarplus.cは修正版を載せる。)

実行例
  同方式によるピッチ880Hz, fs:44100Hzの音を2秒分outsound.wavに合成する。
　Karplus 88200 10000 880 44100 outsound.wav

---------------
4.5 Karplus=Strong方式による打楽器音の合成 Karplus_drum.c 【リスト4.6 (抜粋)】
実行ファイル作成　および実行
 bcc32 Karplus_drum.c wavwrite.obj
 Karplus_drum wnoise n_data amp b f fs filename

実行例:
スネアの響き　(b=0.5 更新周期　100Hz) 減衰時間は長い　2秒
 Karplus_drum 88200 10000 0.5 100 44100 outsound.wav

スネアの響き　(b=0.5 更新周期　800Hz) 減衰時間は短い　2秒
 Karplus_drum 88200 10000 0.5 100 44100 outsound.wav

メタリックな響き　(b=0.8 更新周期　100Hz) 減衰時間は長い　2秒
 Karplus_drum 88200 10000 0.8 100 44100 outsound.wav

---------------
4.6.3 母音'ieaou'の合成　Formant_Vocoder.c 【リスト4.7】
実行ファイル作成　および実行
 bcc32 Formant_Vocoder.c wavwrite.obj
 Formant_Vocoder

出力：sig.wav
注：コンパイル時にf0,iに関して代入した値の未使用の警告があるが、動作への影響はない。
本文との整合性のため、ソースコードは変更せず。

なお、下記の変更を加えれば、警告はでない。
59行目　⇒　//  f0 = (F[0][0]+F[0][1])/2.0;　// （この行を削除）
64行目　⇒  for(/* i=*/ ipos=nrepeat=id=0;; /* i++ */) {  // iに関する部分を削除
70行目　⇒    nrepeat = fs/(/* f0=*/ (F[0][0]+F[0][1])/2.0 // f0に関する部分を削除

修正版　Formant_Vocoder.v1.c
---------------
4.7　歌声の合成　(CHANT方式による歌声の合成)
4.7.6 実プログラムと実際の計算　chantS.c【リスト4.8】
　"                    　FOF関数 　FOF.c【リスト4.9】
実行ファイル作成　および実行
 bcc32 -c FOF.c
 bcc32 chantS.c FOF.obj wavwrite.obj
 chantS outfile.wav

注：コンパイル時に警告で値が未使用となるNtauは、最初に定めた2.4秒長のサン
プルが本文図4.20のようにインパルス応答をtauづつずらすときに生ずるインパルス
応答の数で整数。変更時やモニター時に有用な変数ゆえ現在未使用であるが残している。

警告を抑えるコンパイル
 bcc32 -w-8004 chantS.c FOF.obj wavwrite.obj
---------------
4.8 打撃音の合成
4.8.4 打撃音の合成　本体プログラム PhM_Armk1.c 【リスト4.10】
　"   関数：3×3の逆行列算出  inv3.c 【リスト4.11】
　"   関数：楽器idから音色パラメータ算出  Timbre_Paraset.c 【リスト4.12】
【バグ】
PhM_Armk1.c の91行目
誤  for (j=0; j<=Nmaterial; j++) {
正  for (j=0; j<Nmaterial; j++) {

Nmaterialの直前の = を削除
*サーバ上のファイルは本文の誤りを修正済み
【軽微な修正】（実施済み)
73行 Amp = 0.1;　→　コメントアウト　// Amp = 0.1;

実行ファイル作成
 bcc32 -c inv3.c
 bcc32 -c Timbre_Paraset.c
 bcc32 PhM_Armk1.c inv3.obj Timbre_Paraset.obj wavwrite.obj
 
実行例
 PhM_Armk1 xylo_ 0

これは、xylophoneの音色(id=0)で、出力として
xylo_Glass.wav, xylo_Steel.wav, xylo_Wood.wav
の3種類の音ファイルが合成される。

==========================================================================
5.1　撥弦による簡易シーケンサ　Karplus.seq1.c 【リスト5.1】
実行ファイル作成　および実行
 bcc32 Karplus.seq1.c wavwrite.obj
 Karplus.seq1

実行例
  Karplus.seq1 1 10000 80 44100 Melody_I.wav

---------------
5.3 理想的ローパスフィルタ他HPF,BPF,BEF　filt_LPFapp.c 【リスト5.2】
5.3.1 ローパスフィルタ設計関数　lpfdsn関数　【リスト5.3】
5.3.2 ハイパスフィルタ設計関数　hpfdsn関数  【リスト5.4】
5.3.3 バンドパスフィルタ設計関数 bpfdsn関数 【リスト5.5】
5.3.4 バンドエリミネートフィルタ設計関数 befdsn関数 【リスト5.6】
実行ファイル作成　および実行
 bcc32 filt_LPFapp.c wavread.obj wavwrite.obj
 filt_LPFapp func# inputfile outputfile

実行例
LPF fc=3000Hz
 filt_LPFapp 0 Melody_I.wav MelodyILPF3k.wav

---------------
5.4 櫛型フィルタとオールパスフィルタ　filt_catalog1.c 
5.4.1 FIR櫛型フィルタ(1)　加算の場合　case 4 と関数 FIRcomb 【リスト5.7】
5.4.2 FIR櫛型フィルタ(2)　減算の場合　case 5 【リスト5.8】
5.4.3 IIR櫛型フィルタ(3)　加算の場合　case 6 と関数 IIRcomb 【リスト5.9】
5.5 オールパスフィルタ　オールパスフィルタ関数　Allpass関数 【リスト5.10】
実行ファイル作成　および実行
 bcc32 filt_catalog1.c wavread.obj wavwrite.obj

実行例　FIR櫛型フィルタ(1)　加算
 filt_catalog1 4 Melody_I.wav Melody_CombAdd4.wav

---------------
5.6.1  1タップ遅延処理　onetap_delayline.c 【リスト5.11】
実行ファイル作成　および実行
 bcc32 onetap_delayline.c wavread.obj wavwrite.obj

実行例
 onetap_delayline fot.bak.441.wav fot.bak.441.del1.8.wav

---------------
5.6.2  マルチタップ遅延処理　Mtap_delayline.c 【リスト5.12】
実行ファイル作成　および実行
 bcc32 Mtap_delayline.c wavread.obj wavwrite.obj

実行例
 Mtap_delayline fot.bak.441.wav fot.bak.441.Mtdel1.8.wav

---------------
5.7.4 残響のシミュレーション　rev_schoroeder1all.c 【リスト5.13】
　1-159行　メインプログラム　【リスト5.13 部分１】
　160-208行　関数 IIRcomb_revOnly 【リスト5.13 部分2】
　209-249行  関数 Allpass_rev 【リスト5.13 部分3】
  250-297行  関数 MTdelay0【リスト5.13 部分4】
実行ファイル作成　および実行
 bcc32 rev_schroeder1all.c wavread.obj wavwrite.obj

実行例
 rev_schroeder1all summer2006_5.wav summer2006_5.rev.wav

システムから以下のエラーが出力される。
 The number of clipped samples: 11
クリッピング歪を起こしたサンプルが11個あったとの報告である。
そこで、入力を6dB低くし、再度実行すると、今度はエラーがなくなる。

rev_schroeder1all summer2006_5.p.wav summer2006_5.p.rev.wav

---------------
5.8 非線形アンプ　nonlinamp.c 【リスト5.14】
実行ファイル作成　および実行
 bcc32 nonlinamp.c wavread.obj wavwrite.obj

*ここで表示される警告は　上記の　コンパイラからの警告についての項で、
W8004の警告例で示した理由による。82行目。したがって、削除せずに残す。

【警告を非表示にする実行ファイル作成】 
 bcc32 -w-8004 nonlinamp.c wavread.obj wavwrite.obj

実行
 nonlinamp func# 入力ファイル名　出力ファイル名
実行例
 nonlinamp 0 Karplus.demo.wav Karplus.demo.nonlin.wav
 magicN? 16


---------------
6.2.3 オーバーラップアド方式の基礎 overlapAdd_Basic.c　【リスト6.1】
　　　ハミング窓　関数　hamwdw1.c 【リスト6.2】
何も処理をしないオーバーラップアド方式

実行ファイル作成　および実行
 bcc32 overlapAdd_Basic.c
 Karplus.seq1  -1 10000 80 44100 wave001
 overlapAdd_Basic

2番目のコマンドはoverlapAdd_Basicは wave001という音ファイルを入力とするため、
これを作成している。このときのコマンドの２番目の引数は負値であり、wave001は
wavファイルでなく、rawファイルとなることに注意。なお拡張子はない。
　出力はdata1である。
なお、冒頭の３窓分は方式の特徴として原音の定数倍にはならない。本データは最初
から振幅値が大きいため、冒頭の差異は顕著である。

---------------
6.3.2-6.3.3 オーバーラップアド方式による細粒合成 overlapAdd_granular.c 【リスト6.3】
実行ファイル作成
 bcc32 -c wavRopen.c
 bcc32 -c wavWopen.c
 bcc32 -c wavWclose.c
 bcc32 overlapAdd_granular.c wavRopen.obj wavWopen.obj wavWclose.obj

実行
 ovelapAdd_granular I_fn nwdw nshiftA ratio(real) O_fn
実行例
 overlapAdd_granular fot.bak.44.wav 1440 360 3.0 outSound1.wav

---------------
6.4 振幅スペクトルからの信号波形推定による波形伸縮  OLA_sigEst.c 【リスト6.4】
実行ファイル作成　および実行
 bcc32 -c d1fft.c
 bcc32 OLA_sigEst.c d1fft.obj wavread.obj wavwrite.obj

実行:
 OLA_sigEst 入力ファイル名　出力ファイル名
実行例:
 OLA_sigEst fot.bak.441.wav fot.bak.441.st2.wav

音声を２倍に伸張する例である。実行には繰り返し計算のため、ＣＰＵ性能にもよるが、
1-2分前後かかる。
本文でも述べているとおり、初期位相の選択は３通りあり、その選択により
処理音質も影響を受ける。本プログラムはその指標を以下のとおり「原音の分析結果」
からのもの、と固定している。
プログラム 43-44行目
 id_ph = 0;     // 初期位相の指標
                 // 0:原音の分析結果より　1:ランダム　2:ゼロ

---------------
6.6.1 フェーズボコーダ　pv.c 【本文中リストなし】
6.6.1  フェーズボコーダによる伸縮　pv_st.c【本文中リストなし】

フォルダ　Pv　内に関連するプログラムが全て格納されている。
説明は同フォルダ内の　ReadmePv.txt を参照

---------------
6.7 正弦波モデル　geneMQ.c 【本文中リストなし】
     関数：トラック情報からの音合成　genesinu.c【リスト6.5】
　説明はフォルダGeneMQ内のReadmeMQ.txtを参照。