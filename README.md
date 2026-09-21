このリポジトリは https://github.com/r57zone/OpenVR-driver-for-DIY v1.1の一部を変更して作成された、Windows用OpenXRデバイスドライバです。
元リポジトリでは操作キーがハードコードされていましたが、それを.txtで変更可能にしたものになります。

outputディレクトリ内のものをそのまま`C:\Program Files (x86)\Steam\steamapps\common\SteamVR\drivers`にコピーすれば動作するはずですが、保証はありません。悪しからず。

openVR-driver-for-DIYのREADMEを参照してCMakeで展開した後、その中の/OpenVR/samples/driver-sample/の中のものをこちらのcustomized-src/以下のもので置き換えて頂き、プロジェクトに入っていないcppとhを含めてビルドしてもらえれば使えるはずです。適当で申し訳ありませんが、ご了承ください。

キーはresources/settings/keyCode(device/controller).txtから変更できますが、どうなっているかは多分コードを読まないとわかりません。また、動作中に変更を加えても反映されません。

2026 9.22 2x255
