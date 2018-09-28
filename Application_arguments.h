#pragma once
#include <string>

class Application_arguments {

public:

	Application_arguments(int argc, char **argv);

	std::string	range_str(int idx) const;

	std::string url;
	std::string output_filename = "MultiGetFile.txt";
	size_t	chunk_size = 1024 * 1024;
	size_t	nr_chunks = 4;
	bool	parallel = false;

	struct Argument_exception {
		const char* what() const {
			return "Usage: MultiGetchallenge.exe [-o filename] [-parallel] [-n nr_chunks] [-s chunk_size_bytes] url\n\n";
		}
	};

private:

	template< typename _T >
	void from_string(std::string str, _T& t) const;
};