<?php
/**
 * JAN コードから Rakuten RapidAPI を使って商品情報を返すだけの API です.
 * ============================================================================
 * Web サーバー経由の場合は、URL リクエストのクエリに `/?jan=<JANコード>` と指定します。
 *   - API の使用例:
 *     - http://<コンテナ名>:8080/?jan=4514603325812
 *   - Web サーバーの起動は entrypoint.sh をご覧ください。
 * コンテナ内から CLI 経由で実行の場合は、標準入力もしくは第１引数に JAN コードを指定します。
 *   - 引数入力: 13桁の JAN コードを指定します。
 *   - 標準入力: 引数が空、もしくは '-' の場合は標準入力から受け取ります。
 *   - コマンドの使用例:
 *     - $ search_jan 4514603325812
 *     - $ echo '4514603325812' | search_jan -
 */
const DIR_SEP = DIRECTORY_SEPARATOR;
const PATH_DIR_CACHE = '/data';
const STATUS_SUCCESS = 0; // Exit status on success
const STATUS_FAILURE = 1; // Exit status on failure
// Base64 encoded text of gzencode compressed favicon.ico image. (Blank image)
const BASE64_FAVICON = 'H4sIAAAAAAAAA2NgYARCAQEGIKnAkMHCwCDGwMCgAc' .
                       'RAIaAIRBwX+P///ygexaN4xGIGijAASeibMX4EAAA=';

require_once(__DIR__ . DIR_SEP . 'functions.php.inc');

if (isRequestFaviconIco()) {
    echoFaviconAndExit();
}

$jan_code    = getJANCode();
$json_string = getInfoJANCode($jan_code);
$msg_error   = '';
echoJsonAndExit($json_string, STATUS_SUCCESS, $msg_error);

die;
