#include <iostream>
#include <stdio.h>
#include <stdlib.h>

//Use system() and bash curl to execute 4 http(s) HEAD requests and save the received headers which are then printed to StdOut. First input an integer [1,4] to define number of threads which will execute the requests in parallel.

using namespace std;

//used to later verify if program is in main thread
const pthread_t main_thread_id = pthread_self();

//to check for errors
#define errorexit(errcode, errstr) \
			fprintf(stderr, "%s: &d\n", errstr, errcode); \
			exit(1);

//execute command 'c', divert its output from StdOut to 'data'. If this is being run by spawned thread, return via 'pthread_exit()'.
void *curl(void *c) {
	string cmd = *((string *)c); //command to execute
	string data;
	FILE *stream;
	const int max_buffer = 256;
	char buffer[max_buffer];
	cmd.append(" 2>&1");

	stream = popen(cmd.c_str(), "r"); //execute command
	if (stream) {
		while (!feof(stream)) //divert command output from StdOut to 'data'
			if (fgets(buffer, max_buffer, stream) != NULL)
				data.append(buffer);
		pclose(stream);
	}

	char *ret_data = (char*)malloc(data.size()+1); //make thread allocate memory for the data
	int i;
	for(i=0; i<data.size(); i++)
		ret_data[i] = data[i];
	ret_data[i] = '\0';
	if (pthread_equal(main_thread_id, pthread_self()))
		return (void *)ret_data;
	else
		pthread_exit((void*)ret_data); //exit the thread
}

int main (){
	int errcode;
	int nthreads; //max 4
	cin >> nthreads;
	if (nthreads < 0 || nthreads > 4)
		return 0;
	pthread_t t[nthreads];
	string commands[4] = {"curl -sI https://www.result.si/projekti/",
	 							"curl -sI https://www.result.si/o-nas/",
								"curl -sI https://www.result.si/kariera/",
								"curl -sI https://www.result.si/blog/"};
	char *headers[4]; //used to store return values from curl calls.

	for(int i=0; i<nthreads; i++) //create 'nthreads' parallel threads to run curl().
		if (errcode = pthread_create(&t[i], NULL, curl, &commands[i])){
			errorexit(errcode, "pthread_create");
		}
	for(int i=nthreads; i<4; i++) //run the rest sequentially.
		headers[i] = (char *)curl(&commands[i]);
	
	for(int i=0; i<nthreads; i++) //join the threads
		if (errcode = pthread_join(t[i], (void**)&headers[i])){
			errorexit(errcode, "pthread_join");
		}


	//print the headers to StdOut
	for(int i=0; i<4; i++){
		cout << headers[i] << endl;
		free(headers[i]);
	}
	return 0;
}
