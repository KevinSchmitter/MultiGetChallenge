#pragma once
#include <string>
#include <fstream>

class File_output
{
public:
	File_output(std::string const&	output_filename) {
		file_stream.open(output_filename, std::ofstream::binary);
		if (!file_stream.good()) { throw std::exception("Unable to open file\n\n"); }
	}

	~File_output() {
		file_stream.close();
	}

	void	write_chunk(std::string const& str) {
		if (file_stream.good()) {
			file_stream.write(&str[0], str.size());
		}
	}

private:
	std::ofstream	file_stream;
};
