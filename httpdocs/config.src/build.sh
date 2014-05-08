#!/bin/bash
scss -t compressed main.scss main.css
java -jar /home/dan/.local/share/gcc/compiler.jar --compilation_level SIMPLE_OPTIMIZATIONS main.js > main-min.js
touch index.html
echo '<!DOCTYPE html>' > index.html
echo '<head>' >> index.html
echo '  <title>MineWatch</title>' >> index.html
echo '  <meta name="viewport" content="width=device-width, user-scalable=no">' >> index.html
echo '  <style type="text/css">' >> index.html
cat main.css >> index.html
echo '  </style>' >> index.html
echo '  <script type="text/javascript">' >> index.html
cat main-min.js >> index.html
echo '  </script>' >> index.html
echo '</head>' >> index.html
echo '<body>' >> index.html
cat body.html >> index.html
echo '</body>' >> index.html
echo '</html>' >> index.html
cp -v index.html ../config/
#scp index.html root@wayfaerer.danielhead.com:/var/www/danielhead.com/httpdocs/minewatch/config/
#rsync -a -e ssh config colony.danielhead.com:/var/www/minewatch/ --progress
