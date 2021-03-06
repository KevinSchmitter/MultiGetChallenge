#include "Application_arguments.h"
#include "Curl_request.h"
#include "File_output.h"

#include <vector>
#include <string>
#include <iostream>
#include <thread>
#include <future>

void	request_chunks(Application_arguments const& args, File_output& file_output);

int main(int argc, char **argv) {
	try {
		Application_arguments	args(argc, argv);
		File_output	file_output(args.output_filename);
		Curl_resources	curl_resources;

		request_chunks(args, file_output);

		std::cerr << std::endl;
		std::cerr << "Output saved to " << args.output_filename << std::endl;
	}
	catch (Application_arguments::Argument_exception const& e) {
		std::cerr << e.what();
	}
	catch (std::exception const& e) {
		std::cerr << e.what();
	}

	return 0;
}

void	request_chunks(Application_arguments const& args, File_output& file_output){
	if (args.parallel) {
		std::vector< std::future< std::string > >	future_chunks;
		for (size_t i = 0; i < args.nr_chunks; ++i) {
			future_chunks.push_back(std::async([&args, i]() { return Request_chunk(args, i)(); }));
		}

		for (auto& future_chunk : future_chunks) {
			file_output.write_chunk(future_chunk.get());
		}
	}
	else {
		for (size_t i = 0; i < args.nr_chunks; ++i) {
			file_output.write_chunk(Request_chunk(args, i)());
		}
	}
}