#include "tkfft.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <string>

#pragma warning(disable : 4996)

using namespace std::literals::string_literals;

static unsigned int csvSplit(std::string& const str, std::vector<std::string>& devided)
{
	devided.clear();

	std::stringstream ss(str);
	std::string buf;

	while (!std::getline(ss, buf, ',').eof())
		devided.push_back(buf);

	return devided.size();
}

static std::string toExpString(double const val)
{
	char buf[64];
	std::sprintf(buf, "%E", val);
	return std::string(buf);
}
static std::string toExpString(std::complex<double>& const val)
{
	char buf[128];
	std::sprintf(buf, "%E%c%E", std::real(val), (std::imag(val) < 0 ? '-' : '+'), std::imag(val));
	return std::string(buf);
}
static inline double toPS(std::complex<double>& const val)
{
	double tmp = std::abs(val);
	return tmp * tmp;
}

static bool isPowerOfTwo(unsigned int n)
{
	unsigned int remainder;

	while (n /= 2)
		remainder = n % 2;

	return static_cast<bool>(remainder);
}

static void madokaSin(std::vector<std::vector<double> >& data)
{
	const double pi = 3.141592653589793;
	for (size_t i = 0; i < data.size(); ++i) {
		const unsigned int size = data[i].size();
		const double pi_size = pi / size;
		for (size_t j = 0; j < size; ++j) {
			data[i][j] *= std::sin(j * pi_size);
		}
	}
}

void test()
{
	double pi = 3.141592653589793;
	std::vector<double> f(1 << 16);
	for (size_t i = 0; i < f.size(); i++)
		f[i] = (std::sin(0.01 * i * pi) + 2 * std::sin(0.03 * i * pi)) * std::sin(i * pi / f.size());
	std::vector<std::complex<double> > result(f.size());
	result = TKFFT::FFT(f);
	for (size_t i = 0; i < f.size(); i++)
		std::cout << (std::abs(result[i]) * std::abs(result[i])) << " " << f[i] << std::endl;
}

int main(int argc, char* argv[])
{
	unsigned int start = 25000;
	unsigned int period = 32768;
	//	unsigned int start = 0;
	//	unsigned int period = 262144;
	unsigned int delay = 0;
	unsigned int skip_lines = 2; //In case using YOKOGAWA WDF2CSV convertor.
	unsigned int madoka_chosen = 1;

	switch (argc) {
	case 4:
		std::cerr << "Too few arguments!" << std::endl;
	case 1:
	default:
		std::cerr << "TK Pre Processor 171020" << std::endl;
		std::cerr << "syntax: " << std::endl;
		std::cerr << "\targument1: csv source file" << std::endl;
		std::cerr << "\targument2: window function" << std::endl;
		std::cerr << "\t\t0: no window function" << std::endl;
		std::cerr << "\t\t1: sin" << std::endl;
		std::cerr << "\targument3: start" << std::endl;
		std::cerr << "\targument4: period (must 2**n)" << std::endl;
		std::cerr << "\targument5: skip lines" << std::endl;
		std::cerr << "\targument6: delay" << std::endl;
		return 0;
	case 7:
		delay = std::stoul(static_cast<std::string>(argv[6]));
	case 6:
		skip_lines = std::stoul(static_cast<std::string>(argv[5]));
	case 5:
		period = std::stoul(static_cast<std::string>(argv[4]));
		start = std::stoul(static_cast<std::string>(argv[3]));
		if (isPowerOfTwo(period)) {
			std::cerr << "Period must be a power of 2!" << std::endl;
			return 0;
		}
	case 3:
		madoka_chosen = std::stoul(static_cast<std::string>(argv[2]));
	case 2:
		;
	}
	std::ifstream ifs(static_cast<std::string>(argv[1]));
	std::string line_buf;


	//skip lines 
	for (size_t i = start + delay + skip_lines; i && std::getline(ifs, line_buf); --i)
		;

	std::vector<std::string> tmpc;
	unsigned int splits = csvSplit(line_buf, tmpc);
	std::vector<std::vector<double> > data_raw(splits, std::vector<double>(period));
	std::vector<std::vector<std::complex<double> > > data_fft(splits, std::vector<std::complex<double> >(period));

	//store data
	for (size_t i = 0; i < period && std::getline(ifs, line_buf); ++i) {
		std::vector<std::string> this_line_split;
		unsigned int splits;

		splits = csvSplit(line_buf, this_line_split);

		for (size_t j = 0; j < splits; ++j)
			data_raw[j][i] = std::stod(this_line_split[j]);
	}

	madokaSin(data_raw);

	for (size_t i = 0; i < data_raw.size(); i++)
	data_fft[i] = (TKFFT::FFT(data_raw[i]));

	for (size_t i = 0; i < period / 2; ++i) {
		std::string this_line_str;

		for (size_t j = 0; j < splits; ++j)
			this_line_str += (toExpString(toPS(data_fft[j][i])) + ","s);

		std::cout << this_line_str << std::endl;

	}

	return 0;
}
