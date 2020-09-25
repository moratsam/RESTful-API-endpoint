#prepare the base arch
FROM archlinux
RUN pacman --noconfirm -Syy
RUN pacman --noconfirm -Su
RUN pacman --noconfirm -S gcc git 

#grab the repo from github
RUN git clone https://github.com/moratsam/RESTful-API-endpoint.git 

#set input var [0-4]
ENV NAME VAR1
WORKDIR /RESTful-API-endpoint/
RUN g++ -o naloga.out -lpthread naloga.cpp #compile
CMD ["/bin/bash", "/RESTful-API-endpoint/run_naloga.sh"] #run

