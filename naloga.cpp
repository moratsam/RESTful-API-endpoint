#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unordered_set>
#include <curl/curl.h>
#include <gtest/gtest.h>

//Use curl to execute 4 http(s) requests and process the Out to get headlines, which are then printed to StdOut.
//Input integer [1-4] to define how many threads will run in parallel (1->main thread, 4->main thread and 3 spawned threads).

using namespace std;

//to check for errors
#define errorexit(errcode, errstr) \
			fprintf(stderr, "%s: &d\n", errstr, errcode); \
			exit(1);

struct curl_out{ //used to store data from curl call
	int errcode;
	string out;
};

//each object is used to execute a single curl command.
//curl 'link', store processed Out to 'data'.
class Curler{
	string link;
	string data;

	public: void set_link(string l){
		link = l;
	}

	public: string get_data(){
		return data;
	}

	//used for curl()
	static size_t WriteCallback(void *contents, size_t size,\
										size_t nmemb, void *userp){
		((string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}

	FRIEND_TEST(Curler, CurlTrivial); 
	FRIEND_TEST(Curler, CurlBadHost); 
	//execute curl command and return its errcode and out
	struct curl_out curl(const string cmd){
		struct curl_out co;

		CURL *curl;
		CURLcode res;
		curl = curl_easy_init();
		if(curl){
			curl_easy_setopt(curl, CURLOPT_URL, cmd.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &co.out);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			co.errcode = res;
		}
		return co;
	}

	FRIEND_TEST(Curler, getStrTrivial);
	FRIEND_TEST(Curler, getStrStrEmpty);
	FRIEND_TEST(Curler, getStrTwoStarts);
	FRIEND_TEST(Curler, getStrTwoStops);
	//get substring of 's' between start_ and stop_ markers
	string get_str_between_two_str(const string &s, const string &start_marker,\
											const string &stop_marker){
		string tmp = s;
		tmp.append(start_marker);
		unsigned first_marker_pos = tmp.find(start_marker);
		unsigned end_pos_of_first_marker = first_marker_pos + start_marker.length();
		unsigned last_marker_pos = tmp.find(stop_marker);
		return tmp.substr(end_pos_of_first_marker, last_marker_pos - end_pos_of_first_marker);
	}

	//execute curl call and post-process its results
	public: void full_curl(){
		struct curl_out co = curl(link); //run curl method
		if(co.errcode != 0) //check curl finished successfully
			printf("warning:\tCurler.curl errcode: %d\n", co.errcode);
		//get what you need from curl retrieval
		string start_marker = "<h1 class=\"et_pb_module_header\">";
		string stop_marker = "</h1>";
		data = get_str_between_two_str(co.out, start_marker, stop_marker);
	}
};

//helper function for execution with pthread_create()
void* fcurl(void* r){
	Curler* request = (Curler *)r;
	request->full_curl();
	return NULL;
}

//------TESTING------
//curl
TEST(Curler, CurlTrivial){
	Curler c;
	EXPECT_EQ(c.curl("https://ipfs.io").errcode, 0);
}
TEST(Curler, CurlBadHost){
	Curler c;
	EXPECT_EQ(c.curl("https://ipfs.id").errcode, 6);
}

//get_str_between_two_str
TEST(Curler, getStrTrivial){
	Curler c;
	EXPECT_EQ(c.get_str_between_two_str("barXcode", "bar", "code"), "X");
}
TEST(Curler, getStrStrEmpty){
	Curler c;
	EXPECT_EQ(c.get_str_between_two_str("", "bar", "code"), "");
}
TEST(Curler, getStrTwoStarts){
	Curler c;
	EXPECT_EQ(c.get_str_between_two_str("barbarXcode", "bar", "code"), "barX");
}
TEST(Curler, getStrTwoStops){
	Curler c;
	EXPECT_EQ(c.get_str_between_two_str("barXcodecode", "bar", "code"), "X");
}


//------MAIN------
int main(int argc, char** argv){
	int errcode; //for pthread errorexit macro
/*
run for testing
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
*/

	if(argc != 2)
		return 1;
	int runs = 4;
	int nthreads = atoi(argv[1])-1; //number of spawned threads
	if(nthreads < 0 || nthreads > runs-1)
		return 1;
	pthread_t t[nthreads];

	string links[4] = {"https://www.result.si/blog/",
								"https://www.result.si/kariera/",
	 							"https://www.result.si/o-nas/",
								"https://www.result.si/projekti/"};

	Curler reqs[runs]; //curl requests
	for(int i=0; i<runs; i++) //set links
		reqs[i].set_link(links[i]);
	
	int curr_run=0;
	while(curr_run < runs){
		for(int i=0; i<nthreads; i++){ //parallel part
			//if clause to assure the last req is always done by main thread
			if(curr_run+1 == runs)
				break;
			if(errcode = pthread_create(&t[i], NULL, fcurl, &reqs[curr_run])){
				errorexit(errcode, "pthread_create");
			}
			curr_run++;
		}
		reqs[curr_run].full_curl(); //seq part
		curr_run++;

		for(int i=0; i<(curr_run-1)%(nthreads+1); i++) //join threads from parallel part
			if(errcode = pthread_join(t[i], NULL)){
				errorexit(errcode, "pthread_join");
			}
		
	}

	//print results
	int worked = runs;
	for(int i=0; i<runs; i++){
		if(reqs[i].get_data().size() == 0)
			worked--;
		else
			cout << reqs[i].get_data() << endl;
	}
	printf("\nuspelo: %d\tneuspelo: %d\n", worked, runs-worked);
	return 0;
}
