#!/usr/local/bin/gawk -f 

BEGIN           {FS=":"}
uid==$1         {print $2}
