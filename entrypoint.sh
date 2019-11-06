#!/bin/sh
# =============================================================================
#  Container Entrypoint
#
# PHP のビルトイン Web サーバーを起動します。
# デフォルトの待ち受けポートは 8080 です。また、`/data` ディレクトリがマウントされていない
# 場合は処理を中止します。その場合、コンテナは起動しません。
# =============================================================================
PORT_DEFAULT=${PORT_DEFAULT:-8080}
PATH_DIR_CACHE='/data'

if [ ! -d $PATH_DIR_CACHE ];then
  echo 'ERROR: "/data" directory not found.'
  echo '  Mount a volume to /data on the container. Ex: Use "--volume ./data:/data" docker options.'
  exit 1
fi

php -S $(cat /etc/hostname):$PORT_DEFAULT /app/index.php
