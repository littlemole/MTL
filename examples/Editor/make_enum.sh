#!/bin/bash

cat << HERE

enum class ID : int {
HERE

awk  '{ if($2!="" && $3!="" && $0 !~"//" && $2 !~/_APS_/ ){print $2" = "$3"," }}' resource.h


cat << THERE

};

THERE

