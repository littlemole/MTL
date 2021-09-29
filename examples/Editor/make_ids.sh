#!/bin/bash

cat << HERE

#include <string>
#include <sstream>
#include <map>

std::map<int,std::string> int2id;
std::map<std::string,int> id2int;

static int unused = []()
{
HERE

awk  '{ if($2!="" && $3!="" && $0 !~"//" && $2 !~/_APS_/ ){print "int2id["$3"]=""\""$2"\";\r\n id2int[\""$2"\"]="$3";"; }}' Resource.h


cat << THERE

	return 0;
}();


THERE

