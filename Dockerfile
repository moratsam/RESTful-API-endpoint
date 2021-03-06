#prepare the base arch
FROM archlinux
RUN pacman --noconfirm -Syy && pacman --noconfirm -Su
RUN pacman --noconfirm -S git vim gcc gtest

#grab the repo from github
RUN git clone https://github.com/moratsam/RESTful-API-endpoint.git 

RUN pacman --noconfirm -S gcc gtest

#set input var [1-4]
ENV NAME VAR1
WORKDIR /RESTful-API-endpoint/
RUN g++ -o naloga.out -lpthread -lcurl -lgtest naloga.cpp #compile
CMD ["/bin/bash", "/RESTful-API-endpoint/run_naloga.sh"] #run

