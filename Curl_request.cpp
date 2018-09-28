#include "Curl_request.h"
#include "Application_arguments.h"
#include "curl/include/curl.h"
#include <iostream>
#include <mutex>

#pragma comment(lib, "curl/lib/libcurl-d_imp.lib")

Curl_resources::Curl_resources() {
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

Curl_resources::~Curl_resources() {
	curl_global_cleanup();
}

Request_chunk::Request_chunk(Application_arguments const & _args, int _idx) : args(_args), chunk_idx(_idx) {
	curl_handle = curl_easy_init();
	chunk.reserve(args.chunk_size);
}

static std::mutex chunk_complete_cerr_mutex;

static void thread_safe_chunk_complete(int chunk_idx) {
	std::lock_guard< std::mutex >	guard(chunk_complete_cerr_mutex);
	std::cerr << std::to_string(chunk_idx) << ", ";
}

Request_chunk::~Request_chunk() {
	curl_easy_cleanup(curl_handle);
	thread_safe_chunk_complete(chunk_idx);
}

size_t Request_chunk::write_to_chunk(void * buffer, size_t size, size_t nmemb, Request_chunk * request_chunk_p) {
	request_chunk_p->chunk.insert(request_chunk_p->chunk.size(), reinterpret_cast<char*>(buffer), size * nmemb);
	return size * nmemb;
}

std::string Request_chunk::operator()() {
	if (curl_handle) {
		curl_easy_setopt(curl_handle, CURLOPT_URL, args.url.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_RANGE, args.range_str(chunk_idx).c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_to_chunk);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, this);
		CURLcode res = curl_easy_perform(curl_handle);
		if (res != CURLcode::CURLE_OK) {
			throw std::exception(("\n\nUnable to request chunk " + std::to_string(chunk_idx) + "\n\n").c_str());
		}

		return std::move(chunk);
	}

	throw std::exception(("\n\nUnable to open curl handle for chunk " + std::to_string(chunk_idx) + "\n\n").c_str());
}
