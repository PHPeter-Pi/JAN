<?php
/**
 * Functions to include.
 * ============================================================================
 * - 関数名の並び： ABC順
 * - 関数の長さ:
 *   - Height: なるべく 35　行以内に収める。
 *   - Width : なるべく 80 文字以内に収める。
 */

function echoErrorAndExit($msg)
{
    $msg = 'ERROR: ' . $msg;
    $msg = escapeshellcmd($msg);

    echoJsonAndExit('', STATUS_FAILURE, $msg);
}

function echoFaviconAndExit()
{
    $bin_image = getFaviconImage();

    header("Content-Type: image/vnd.microsoft.icon");
    header('Content-Length: ' . strlen($bin_image));
    echo $bin_image;

    exit(STATUS_SUCCESS);
}

function getFaviconImage()
{
    $base64_favicon_compressed = BASE64_FAVICON;
    $bin_compress = base64_decode($base64_favicon_compressed);

    return gzdecode($bin_compress);
}

function echoJsonAndExit(
    string $json_string,
    int    $status=STATUS_SUCCESS,
    string $msg_error=''
) {
    $array_result = [];
    $array_result['status']    = ($status === STATUS_SUCCESS) ? 'SUCCESS' : 'FAIL';
    $array_result['msg_error'] = $msg_error;
    $array_result['result']    = json_decode($json_string, JSON_OBJECT_AS_ARRAY);

    $int_options = JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE;
    $str_result  = json_encode($array_result, $int_options);

    if (! isCLI() && ! headers_sent()) {
        header('Access-Control-Allow-Origin: *');
        header('Content-Type: application/json');
        if ($status === STATUS_FAILURE) {
            // @REF: https://developer.mozilla.org/ja/docs/Web/HTTP/Status/422
            header(getServerProtocol() . ' 422 Unprocessable Entity');
        }
    }

    if ($str_result === false) {
        echo '{"result":"FAIL","msg_error":"Failed to parse object to string.","result":""}';
        exit(STATUS_FAILURE);
    }

    echo $str_result;

    exit($status);
}

function filterObjectNecessary($obj_json)
{
    return $obj_json->Products;
}

function getAPIKeyRapidAPI()
{
    static $api_key;

    if (isset($api_key)) {
        return $api_key;
    }

    $api_key = trim(getenv('API_KEY_RAPIDAPI', true) ?: getenv('API_KEY_RAPIDAPI'));

    if (empty($api_key)) {
        $msg_error = 'API_KEY_RAPIDAPI not set. ' .
                     'The API key is not set in env variable as API_KEY_RAPIDAPI.';
        echoErrorAndExit($msg_error);
    }

    return $api_key;
}

function getHostRapidAPI()
{
    static $name_host_api;

    if (isset($name_host_api)) {
        return $name_host_api;
    }

    $name_host_api = getenv('NAME_HOST_RAPIDAPI', true) ?: getenv('NAME_HOST_RAPIDAPI');
    $name_host_api = rtrim(trim($name_host_api), '/');

    if (empty($name_host_api)) {
        $msg_error = 'NAME_HOST_RAPIDAPI not set. ' .
                     'The API host name is not set in env variable as NAME_HOST_RAPIDAPI.';
        echoErrorAndExit($msg_error);
    }

    return $name_host_api;
}

function getHttpHeaderRapidAPI()
{
    $token_api = getAPIKeyRapidAPI();
    $host_api  = getHostRapidAPI();
    return [
        "x-rapidapi-host: ${host_api}",
        "x-rapidapi-key: ${token_api}"
    ];
}

function getInfoJANCode($jan_code): string
{
    $cache = getInfoJANCodeCache($jan_code);
    if ($cache !== false) {
        return $cache;
    }

    $str_json = searchProduct($jan_code);

    // Cache the results
    if (! putInfoJANCodeCache($jan_code, $str_json)) {
        echoErrorAndExit('Fail to write cache file. Check permission.');
    }

    return $str_json;
}

function getInfoJANCodeCache($jan_code): string
{
    $path_file_cache_json = getPathFileCache($jan_code);

    if (file_exists($path_file_cache_json)) {
        return file_get_contents($path_file_cache_json);
    }

    return false;
}

function getJANCode()
{
    $input = (isCLI()) ? getJANCodeFromCLI() : getJANCodeFromURL();
    $input = isRequestSample($input) ? getJANCodeSample() : $input;

    if (! isJANCodeValid($input)) {
        echoErrorAndExit('Invalid JAN Code.');
    }

    return $input;
}

function getJANCodeFromCLI()
{
    global $argc, $argv;

    switch (true) {
        case ($argc === 2 && ('-' === trim($argv[1]))):
        case ($argc === 1):
            echo 'JAN: ';
            $input = trim(fgets(STDIN));
            break;
        case ($argc === 2):
            $input = trim($argv[1]);
            break;
        default:
            return false;
    }

    return $input;
}

function getJANCodeFromURL()
{
    if (! isset($_GET['jan'])) {
        return false;
    }

    $jan_code = $_GET['jan'];

    if (! is_string($jan_code)) {
        echoErrorAndExit('JAN Code is not a string or integer. Maybe array given.');
    }

    return $jan_code;
}

function getJANCodeSample()
{
    static $jan_code_sample;

    if (isset($jan_code_sample)) {
        return $jan_code_sample;
    }

    $list_sample = [
        '4514603325812', //アサヒ飲料 ウィルキンソンタンサンＰＥＴ５００Ｎ
        '4901411011523', //キリンビール のどごし〈生〉３５０缶
        '4901004044334', //アサヒビール 極上＜キレ味＞　缶３５０ｍｌ
    ];

    $jan_code_sample = $list_sample[mt_rand(0, count($list_sample) - 1)];

    return $jan_code_sample;
}

function getPathDirData()
{
    return realpath(PATH_DIR_CACHE);
}

function getPathFileCache($jan_code)
{
    return getPathDirData() . DIR_SEP . $jan_code . '.json';
}

function getServerProtocol()
{
    return $_SERVER['SERVER_PROTOCOL'];
}

function getURLRequestRapidAPI($jan_code)
{
    $url_host     = getHostRapidAPI();
    $url_endpoint = "/services/api/Product/Search/20170426";
    $url_query    = "?keyword=${jan_code}";
    $url_request  = "https://${url_host}${url_endpoint}${url_query}";

    return $url_request;
}

function isCLI()
{
    return (http_response_code() === false);
}

function isJANCodeValid($string)
{
    if (empty(trim($string))) {
        echoErrorAndExit('Given JAN code is empty. Specify a JAN code.');
    }
    if (! is_numeric($string)) {
        echoErrorAndExit('Given JAN code is not a number!');
    }
    if (strlen($string) !== strlen(getJANCodeSample())) {
        echoErrorAndExit('Given JAN code is not in EAN-13 format');
    }

    return true;
}

function isRequestFaviconIco()
{
    if (isset($_SERVER['REQUEST_URI']) && ($_SERVER['REQUEST_URI']==='/favicon.ico')) {
        return true;
    }

    return false;
}

function isRequestSample($jan_code)
{
    return (strtolower($jan_code) === 'sample');
}

function putInfoJANCodeCache($jan_code, $str_json)
{
    $path_dir_cache = getPathDirData();

    if (! is_dir($path_dir_cache)) {
        $msg_error = 'No data directory found. ' .
                     'You need to mount/create cache directory at: ' . $path_dir_cache;
        echoErrorAndExit($msg_error);
    }

    $path_file_cache = getPathFileCache($jan_code);

    return file_put_contents($path_file_cache, $str_json, LOCK_EX);
}

function requestCurl($curl_options)
{
    $curl = curl_init();
    curl_setopt_array($curl, $curl_options);

    $response = curl_exec($curl);
    $err = curl_error($curl);

    curl_close($curl);

    if ($err) {
        echoErrorAndExit('cURL Error #:' . $err);
    }

    return $response;
}

function searchProduct($jan_code): string
{
    $url_request = getURLRequestRapidAPI($jan_code);
    $response    = requestCurl([
        CURLOPT_URL => $url_request,
        CURLOPT_RETURNTRANSFER => true,
        CURLOPT_ENCODING => "",
        CURLOPT_MAXREDIRS => 10,
        CURLOPT_TIMEOUT => 30,
        CURLOPT_HTTP_VERSION => CURL_HTTP_VERSION_1_1,
        CURLOPT_CUSTOMREQUEST => "GET",
        CURLOPT_HTTPHEADER => getHttpHeaderRapidAPI(),
    ]);

    $obj_json = json_decode($response);

    if (null === $obj_json) {
        echoErrorAndExit('Malformed JSON :' . print_r($obj_json, true));
    }

    if (empty($obj_json->Products)) {
        echoErrorAndExit('No item found with JAN Code: ' . $jan_code);
    }

    $int_options = JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE;
    $str_json = json_encode(filterObjectNecessary($obj_json), $int_options);

    return $str_json;
}
