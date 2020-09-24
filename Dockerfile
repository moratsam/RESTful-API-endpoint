#prepare the base arch
FROM archlinux
RUN pacman --noconfirm -Syy
RUN pacman --noconfirm -Su
RUN pacman --noconfirm -S gcc git 

#grab the repo from github
RUN git clone https://github.com/moratsam/RESTful-API-endpoint.git 
RUN mv RESTful-API-endpoint naloga


ENV NAME VAR1 #set input var [0-4]
WORKDIR /naloga/
RUN g++ -o naloga.out -lpthread naloga.cpp #compile
CMD ["/bin/bash", "/naloga/run_naloga.sh"] #run

