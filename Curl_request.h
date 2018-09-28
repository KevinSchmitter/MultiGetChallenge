#pragma once
#include <string>

struct Curl_resources {
	Curl_resources();
	~Curl_resources();
};

class Application_arguments;
typedef void CURL;

class Request_chunk {
public:
	Request_chunk(Application_arguments const& _args, int _idx);
	~Request_chunk();

	static	size_t	write_to_chunk(void *buffer, size_t size, size_t nmemb, Request_chunk *request_chunk_p);

	std::string operator()();

private:
	CURL *curl_handle;
	std::string	chunk;

	Application_arguments const& args;
	int chunk_idx;
};