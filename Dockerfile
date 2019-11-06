FROM keinos/php8-jit

USER root
COPY src/search_jan /usr/local/bin/search_jan
COPY entrypoint.sh /entrypoint.sh

USER www-data
COPY ["src/index.php", "src/functions.php.inc", "/app/"]
ENTRYPOINT [ "/entrypoint.sh" ]
