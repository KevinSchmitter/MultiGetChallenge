#include "Application_arguments.h"
#include <vector>
#include <sstream>

Application_arguments::Application_arguments(int argc, char ** argv) {
	std::vector<std::string> args(argv + 1, argv + argc);
	if (args.empty()) { throw Argument_exception(); }

	if (std::find(args.begin(), args.end(), "-parallel") != args.end()) {
		parallel = true;
	}

	auto	output_arg_it = std::find(args.begin(), args.end(), "-o");
	if (output_arg_it == args.end() - 1) { throw Argument_exception(); }
	if (output_arg_it != args.end()) {
		output_filename = *(output_arg_it + 1);
	}

	output_arg_it = std::find(args.begin(), args.end(), "-n");
	if (output_arg_it == args.end() - 1) { throw Argument_exception(); }
	if (output_arg_it != args.end()) {
		from_string(*(output_arg_it + 1), nr_chunks);
	}

	output_arg_it = std::find(args.begin(), args.end(), "-s");
	if (output_arg_it == args.end() - 1) { throw Argument_exception(); }
	if (output_arg_it != args.end()) {
		from_string(*(output_arg_it + 1), chunk_size);
	}

	url = args.back();
}

std::string Application_arguments::range_str(int idx) const {
	return std::to_string(idx*chunk_size) + "-" + std::to_string((idx + 1)*chunk_size - 1);
}

template<typename _T>
void Application_arguments::from_string(std::string str, _T & t) const {
	(std::stringstream() << str) >> t;
}