# SSBPLib
ssbpLib v1.3.5をゲーム向けに改造しました。

SpriteStudio5.8でのssbpファイルに対応できたので公開してみました。
作ってるゲームでの必要性に応じて改良してこうかなーって思ってます。

主な改造点
* ロード・描画の処理をライブラリ外で定義できるようにしました
* インスタンスアニメーションの扱いを利用者側で定義できるようにしました
* リソース周りの扱いをゲーム向けに修正しました
* エフェクト機能を単独再生できるようにしました

ひとまず動作させたいときは、 DxLibSample/Readme.txt を参照してください。
動作には、テクスチャとssbpファイルが必要になります。
ssbpファイルは、Ss5Converterを使ってsspjファイルから生成してください。

-----
本家様 https://github.com/SpriteStudio/ssbpLib

Ss5Converterは、SpriteStudio 5 SDK v1.7.1 のバージョンで動作確認しています。
https://github.com/SpriteStudio/SpriteStudio5-SDK


