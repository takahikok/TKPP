#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std::literals::string_literals;

int main(int argc, char* argv[])
{
	std::string delimiter = ",";
	unsigned int channel_number = 2048;
	bool is_eol_delimiter = true;

	switch (argc) {
	case 2:
		std::cerr << "Too few arguments!" << std::endl;
	case 1:
	default:
		std::cerr << "TK Sed" << std::endl;
		std::cerr << "syntax: " << std::endl;
		std::cerr << "\targument1: left file" << std::endl;
		std::cerr << "\targument2: right file" << std::endl;
		std::cerr << "\targument3: add delimiter at end of line" << std::endl;
		std::cerr << "\targument4: \",\":comma separated source(default), \"w\":white-space separated source)" << std::endl;
		return 0;
	case 5:
		if (static_cast<std::string>(argv[4]) == "w")
			delimiter = " ";
	case 4:
		is_eol_delimiter = static_cast<bool>(std::stoul(static_cast<std::string>(argv[3])));
	case 3:
		;
	}
	std::vector<std::ifstream> ifs(2);
	for (size_t i = 0; i < ifs.size(); ++i)
		ifs[i].open(argv[1 + i], std::ios::in);

	for (std::ifstream& the_ifs : ifs)
		if (the_ifs.fail()) {
			std::cerr << "Open failed!" << std::endl;
			return 0;
		}

	std::string line_buf[2];

	while (std::getline(ifs[0], line_buf[0])) {
		std::getline(ifs[1], line_buf[1]);
		std::cout << line_buf[0] << (is_eol_delimiter ? delimiter : "") << line_buf[1] << std::endl;
	}

	return 0;
}
