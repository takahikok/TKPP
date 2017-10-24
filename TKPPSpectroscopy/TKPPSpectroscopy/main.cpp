#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std::literals::string_literals;

static std::vector<std::string> csvSplit(std::string const str, const char delimiter = ',')
{
	std::vector<std::string> devided;

	std::stringstream ss(str);
	std::string buf;

	while (std::getline(ss, buf, delimiter))
		devided.push_back(buf);

	return devided;
}

int main(int argc, char* argv[])
{
	const char default_delimiter = '\t';
	unsigned int channel_number = 2048;
	bool is_inheritance_comment = true;

	switch (argc) {
	case 2:
		std::cerr << "Too few arguments!" << std::endl;
	case 1:
	default:
		std::cerr << "TK Pre Processor 171020" << std::endl;
		std::cerr << "syntax: " << std::endl;
		std::cerr << "\targument1: source file" << std::endl;
		std::cerr << "\targument2: bg file" << std::endl;
		std::cerr << "\targument3: is inheritance comment" << std::endl;
		std::cerr << "\targument4: channel number" << std::endl;
		return 0;
	case 5:
		channel_number = std::stoul(static_cast<std::string>(argv[4]));
	case 4:
		is_inheritance_comment = static_cast<bool>(std::stoul(static_cast<std::string>(argv[3])));
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

	std::string line_buf;
	std::vector<std::vector<int> > data(ifs.size(), std::vector<int>(channel_number));

	//store data
	for (size_t i = 0; i < ifs.size(); ++i)
		for (size_t j = 0; j < channel_number && std::getline(ifs[i], line_buf); ++j)
			data[i][j] = std::move(std::stoi(csvSplit(line_buf, default_delimiter)[1]));

	//output remainder
	for (size_t i = 0; i < channel_number; ++i)
		std::cout << (i + 1) << default_delimiter << (data[0][i] - data[1][i]) << std::endl;

	//output comment
	if (is_inheritance_comment)
		for (std::ifstream& the_ifs : ifs)
			while (std::getline(the_ifs, line_buf))
				std::cout << line_buf << std::endl;

	return 0;
}
