#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unordered_set>


//Use system() and bash curl to execute 4 http(s) HEAD requests and save the received headers which are then printed to StdOut. First input an integer [1,4] to define number of threads which will execute the requests in parallel. Set dbg to 1 for debug and run naloga_tester.sh

using namespace std;

bool dbg = 0;
unordered_set<pthread_t> thread_ids; //unique thread ids
//used to later verify if program is in main thread
const pthread_t main_thread_id = pthread_self();

//to check for errors
#define errorexit(errcode, errstr) \
			fprintf(stderr, "%s: &d\n", errstr, errcode); \
			exit(1);

//get substring of 's' between start_ and stop_ markers
string get_str_between_two_str(const string &s, const string &start_marker, const string &stop_marker){
	unsigned first_marker_pos = s.find(start_marker);
	unsigned end_pos_of_first_marker = first_marker_pos + start_marker.length();
	unsigned last_marker_pos = s.find(stop_marker);

	return s.substr(end_pos_of_first_marker, last_marker_pos - end_pos_of_first_marker);
}

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
	if(dbg)
		thread_ids.insert(pthread_self());
	string start_marker = "<h1 class=\"et_pb_module_header\">";
	string stop_marker = "</h1>";
	data.append(start_marker); //in case 'data' is empty
	data = get_str_between_two_str(data, start_marker, stop_marker);

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

int main (int argc, char** argv){
	if(argc != 2)
		return 1;
	int errcode;
	int nthreads = atoi(argv[1]); //max 4
	if (nthreads < 0 || nthreads > 4)
		return 0;
	pthread_t t[nthreads];
	string commands[4] = {"curl -s https://www.result.si/projekti/",
	 							"curl -s https://www.result.si/o-nas/",
								"curl -s https://www.result.si/kariera/",
								"curl -s https://www.result.si/blog/"};
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

	if(dbg){ //print thread ids and curl retrievals
		thread_ids.insert(pthread_self());
		cout << thread_ids.size() << endl;
		return 0;
	}

	//print results
	int worked = 4;
	for(int i=0; i<4; i++){
		if(strlen(headers[i]) == 0)
			worked--;
		else
			cout << headers[i] << endl;
		free(headers[i]);
	}
	printf("\nuspelo: %d\tneuspelo: %d\n", worked, 4-worked);
	return 0;
}
