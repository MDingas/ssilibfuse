#!/usr/local/bin/gawk -f

BEGIN           {FS=":"}
username==$1    {print $2}
