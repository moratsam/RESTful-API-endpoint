# RESTful-API-endpoint

skeleton Swagger UI on
https://app.swaggerhub.com/apis/bahanje/result/1.0.0

#pull the docker image

$[torsocks] docker pull moratsam/arch_api


#test it

$docker run -it moratsam/arch_api bash (compile flag -lgtest)


#run it
$docker run -it -e VAR1=[1-4] moratsam/arch_api

