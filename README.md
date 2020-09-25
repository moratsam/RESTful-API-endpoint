# RESTful-API-endpoint

#skeleton Swagger UI on
#https://app.swaggerhub.com/apis/bahanje/result/1.0.0

#pull the docker image
$[torsocks] docker pull moratsam/arch_api

#test it
$docker run -it moratsam/arch_api bash
$cd /RESTful-API-endpoint/
#set 'dbg' bool to 1 (line 12)
$vim naloga.cpp
$./tests.sh
#ctrl+d to exit

#run it
$docker run -it -e VAR1=[0-4] moratsam/arch_api

