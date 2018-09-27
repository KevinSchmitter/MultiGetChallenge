#include "curl/include/multi.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#pragma comment(lib, "curl/lib/libcurl-d_imp.lib")

struct Application_arguments {
	Application_arguments(int argc, char **argv) {
		std::vector<std::string> args(argv + 1, argv + argc);
		if (args.empty()) { throw std::exception("Usage: MultiGetchallenge.exe [-o filename] url"); }

		auto	output_arg_it = std::find(args.begin(), args.end(), "-o");
		if (output_arg_it != args.end()) {
			output_filename = *(output_arg_it + 1);
		}

		url = args.back();
	}

	std::string	range_str(int idx) const {
		return std::to_string(idx*chunk_size) + "-" + std::to_string((idx + 1)*chunk_size - 1);
	}

	std::string url;
	std::string output_filename = "MultiGetFile.txt";
	size_t	chunk_size = 1024 * 1024;
	size_t	nr_chunks = 4;
};

class File_output
{
public:
	File_output(std::string const&	output_filename) {
		file_stream.open(output_filename, std::ofstream::binary);
		if (!file_stream.good()) { throw std::exception("Unable to open file"); }
	}

	~File_output() {
		file_stream.close();
	}

	static
		size_t write_data(void *buffer, size_t size, size_t nmemb, void *_file_output) {
		return reinterpret_cast<File_output*>(_file_output)->write(buffer, size, nmemb);
	}

	size_t	write(void *buffer, size_t size, size_t nmemb) {
		if (file_stream.good()) {
			file_stream.write(reinterpret_cast<const char*>(buffer), size*nmemb);
			return size * nmemb;
		}

		return 0;
	}

private:
	std::ofstream	file_stream;
};

struct Curl_resources_obj {
	Curl_resources_obj() {
		curl_global_init(CURL_GLOBAL_DEFAULT);
	}
	~Curl_resources_obj() {
		curl_global_cleanup();
	}
};

class Sequential_curl_request {
public:
	Sequential_curl_request(Application_arguments const& args, int idx, File_output&	output) {
		curl_handle = curl_easy_init();
		if (curl_handle) {
			curl_easy_setopt(curl_handle, CURLOPT_URL, "http://cecfbddf4927545.bwtest-aws.pravala.com/384MB.jar");
			curl_easy_setopt(curl_handle, CURLOPT_RANGE, args.range_str(idx).c_str());
			curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, File_output::write_data);
			curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &output);
			CURLcode res = curl_easy_perform(curl_handle);
		}
	}

	~Sequential_curl_request() {
		curl_easy_cleanup(curl_handle);
		std::cerr << '.';
	}

private:
	CURL *curl_handle;
};

int main(int argc, char **argv) {
	try {
		Application_arguments	args(argc, argv);
		File_output	file_output(args.output_filename);
		Curl_resources_obj	manage_curl_resources;

		for (size_t i = 0; i < args.nr_chunks; ++i) {
			Sequential_curl_request(args, i, file_output);
		}

		std::cerr << std::endl;
		std::cerr << "Output saved to " << args.output_filename;
	}
	catch (std::exception const& e) {
		std::cerr << e.what();
	}

	getchar();
	return 0;
}