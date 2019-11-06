# JAN Code API for Rakuten Rapid API

[JAN コード](https://ja.wikipedia.org/wiki/%E3%83%90%E3%83%BC%E3%82%B3%E3%83%BC%E3%83%89#%E7%B5%B1%E4%B8%80%E5%95%86%E5%93%81%E3%82%B3%E3%83%BC%E3%83%89)の商品名や商品情報を返すだけの Web API コンテナです。

JAN コードの商品情報取得には、[楽天 RapidAPI](https://api.rakuten.co.jp/ja/) を利用しています。そのため、事前に `Rakuten RapidAPI` の `Rakuten Marketplace Product Search` のアクセストークンが必要になります。

## `Rakuten RapidAPI` のアクセストークン取得

1. `Rakuten RapidAPI` に[アカウント登録](https://api.rakuten.co.jp/ja/)する。（要 GitHub or 楽天アカウント）
2. API マーケットプレイスより [`Rakuten Marketplace Product Search` API を検索](https://api.rakuten.net/search/Rakuten%20Marketplace%20Product%20Search)して登録。
3. [「Rakuten Marketplace Product Search 」のドキュメント](https://api.rakuten.net/rakuten_webservice/api/rakuten-marketplace-product-search)を開き、以下の２つを控える。
    1. `X-RapidAPI-Host`
    2. `X-RapidAPI-Key`

## コンテナ（Web API）の利用方法

<details><summary>前提知識</summary><div><br>

### 前提知識

- `git` と言われて意味がわかる and/or 使ったことがある。
- Docker および Docker コンテナを利用したことがある。
- Dockerfile が読める and/or 作れる。
- Docker ネットワークおよびポートの外部公開（`expose` や `--port 8888:8080`）の意味がわかる。
- `docker-compose` を利用したことがある。
- 「Dockerfile のベース・イメージがアーキテクチャと合っていない」と言われてわかる。
  - 「ARM アーキテクチャで x86_64 用のベースイメージを使っている」など

</div></details>

### 使用制限と仕様

- **このコンテナは一般公開向けではありません**。同じ Docker ネットワーク内からのアクセスを前提としています。
- 基本仕様
  - **待ち受けポート：** 8080 ポート（Web API はコンテナの 8080 ポートで待機します）
  - **エンドポイント：** `http://<このコンテナ名>:8080/`（他のコンテナからはコンテナ名をホスト名として`GET` で HTTP リクエストします）
  - **リクエストメソッド：** `GET` `jan=<JANコード>`（JANコードは 13 桁に限定されています）
    - `cURL` の場合のリクエスト例：　`curl http://my_container:8080/?jan=4514603325812`
  - **レスポンス：** 商品情報を含んだ JSON データです。
  - **環境変数：** コンテナ起動時に、環境変数として以下の値をコンテナの `ENV` 値として渡す必要があります。
    - `NAME_HOST_RAPIDAPI=<上記で控えた X-RapidAPI-Host の値>`
    - `API_KEY_RAPIDAPI=<上記で控えた X-RapidAPI-Key の値>`
    - 参考文献：[[コンテナへ環境変数を渡す方法](https://qiita.com/KEINOS/items/518610bc2fdf5999acf2) @ Qiita]
  - **キャッシュ：** 一度検索された情報は、コンテナの `/data` ディレクトリにキャッシュされます。
    - `Rakuten RapidAPI` のリクエスト制限や負荷を減らすために `/data` ディレクトリの永続化（ローカルをマウントするなど）を推奨します。

### 簡易動作確認

コンテナの Web API にローカル（Docker を動かしているホストマシン）からアクセスして動作確認します。（開発時以外は非推奨の利用方法）

```shellsession
$ # イメージの作成。あらかじめリポジトリをローカルに clone しておき、その git リポジトリ内で以下を実行
$ docker build -t my_jan_image:local .
...

$ # コンテナの起動
$ #   --detach -> バックグランドで待機
$ #   --env NAME_HOST_RAPIDAPI に X-RapidAPI-Host の値
$ #   --env API_KEY_RAPIDAPI に X-RapidAPI-Key の値
$ #   --name コンテナ名
$ #   --port コンテナの 8080 ポートをローカルの 8888 で待機
$ docker run \
  --detach \
  --env NAME_HOST_RAPIDAPI=xxxxxx API_KEY_RAPIDAPI=yyyyyy \
  --name my_jan_container \
  --port 8888:8080 \
  my_jan_image

$ # コンテナの Web API へのリクエスト
$ curl http://localhost:8888/?jan=4514603325812
```
