#!/bin/bash
# upload the web app to the board

# investigate using a single file to bring in the defaults
ip=$(< ip.txt)

#ip=${1:-"192.168.0.103"}
url="http://$ip/edit"

declare -a filenames=(
  "styles.css"
  "app.js"
  "index.htm"
  "simple.css"
  "simple.js"
  "simple.htm"
  "edit.htm"
)

# "images/atom196.png"
# "favicon.ico"

for filename in "${filenames[@]}"
do
  # add --trace-ascii curl.log for logging

  #gzip -kf data/$filename

  echo $filename
  curl --form "file=@data/$filename;filename=$filename" $url

  #rm -f data/$filename.gz
done
