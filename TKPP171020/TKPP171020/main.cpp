#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#pragma warning(disable : 4996)

using namespace std::literals::string_literals;

static unsigned int csvSplit(std::string str, std::vector<std::string>& devided)
{
	devided.clear();

	std::stringstream ss(str);
	std::string buf;

	while (!std::getline(ss, buf, ',').eof())
		devided.push_back(buf);

	return devided.size();
}

static std::string toExpString(double val)
{
	char buf[64];
	std::sprintf(buf, "%E", val);
	return std::string(buf);
}

int main(int argc, char* argv[])
{
	unsigned int start[] = { 115000, 240000 };
	unsigned int period = 100000;
	unsigned int delay = 85;
	unsigned int skip_lines = 2; //In case using YOKOGAWA WDF2CSV convertor.
	bool bg_process = true;

	switch (argc) {
	case 5:
	case 4:
		std::cerr << "Too few arguments!" << std::endl;
	case 1:
	default:
		std::cerr << "TK Pre Processor 171020" << std::endl;
		std::cerr << "syntax: " << std::endl;
		std::cerr << "\targument1: csv source file" << std::endl;
		std::cerr << "\targument2: bg process flag" << std::endl;
		std::cerr << "\targument3: start1" << std::endl;
		std::cerr << "\targument4: start2" << std::endl;
		std::cerr << "\targument5: period" << std::endl;
		std::cerr << "\targument6: skip lines" << std::endl;
		std::cerr << "\targument7: delay" << std::endl;
		return 0;
	case 8:
		delay = std::stoul(static_cast<std::string>(argv[7]));
	case 7:
		skip_lines = std::stoul(static_cast<std::string>(argv[6]));
	case 6:
		period = std::stoul(static_cast<std::string>(argv[5]));
		start[1] = std::stoul(static_cast<std::string>(argv[4]));
		start[0] = std::stoul(static_cast<std::string>(argv[3]));
	case 3:
		bg_process = static_cast<bool>(std::stoul(static_cast<std::string>(argv[2])));
	case 2:
		;
	}
	std::ifstream ifs(static_cast<std::string>(argv[1]));
	std::string line_buf;

	std::vector<std::string> rdata(period);

	//skip lines until the beggining of the first sweep
	for (size_t i = start[0] + delay + skip_lines; i && std::getline(ifs, line_buf); --i)
		;

	//store beggining of the first sweep position
	std::streampos first_sweep_pos = ifs.tellg();

	//skip lines until the beggining of the second sweep
	for (size_t i = start[1] - start[0]; i && std::getline(ifs, line_buf); --i)
		;

	//keep data during the second sweep
	for (size_t i = 0; i < period && std::getline(ifs, rdata[i]); ++i)
		;

	//back to the beggining of the first sweep position
	ifs.seekg(first_sweep_pos);

	switch (bg_process) {
	case true:
		//first sweep
		for (size_t i = 0; i < period && std::getline(ifs, line_buf); ++i) {
			std::string this_line_str = line_buf + rdata[i];
			std::vector<std::string> this_line_split;
			unsigned int splits;

			splits = csvSplit(this_line_str, this_line_split);

			for (size_t i = 0; i < splits / 2; i++)
				this_line_str += (toExpString(std::stod(this_line_split[i])
					- std::stod(this_line_split[splits / 2 + i])) + ","s);

			std::cout << this_line_str << std::endl;
		}

		break;

	case false:
		//first sweep
		for (size_t i = 0; i < period && std::getline(ifs, line_buf); ++i)
			std::cout << line_buf << rdata[i] << std::endl;

		break;
	}

	return 0;
}




