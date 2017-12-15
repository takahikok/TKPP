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
	const unsigned int MAX_TRACE_NUMBER = 17;
	unsigned int ma_sample = 5;
	std::string deliminator = ",";

	switch (argc) {
	case 1:
	default:
		std::cerr << "TK Moving Average" << std::endl;
		std::cerr << "syntax: " << std::endl;
		std::cerr << "\targument1: comma separated source data file" << std::endl;
		std::cerr << "\targument2: moving average sampling number" << std::endl;
		std::cerr << "\targument3: \",\":comma separated source(default), \"w\":white-space separated source)" << std::endl;
		return 0;
	case 4:
		if (static_cast<std::string>(argv[3]) == "w")
			deliminator = " \t";
		else
			std::cerr << "��3������ǂݍ��߂܂���ł���" << std::endl;
	case 3:
		try {
			ma_sample = std::stoi(argv[2]);
		}
		catch (std::invalid_argument) {
			std::cerr << "��2������ǂݍ��߂܂���ł���" << std::endl;
		}
	case 2:
		;
	}

	std::ifstream ifs(static_cast<std::string>(argv[1]));
	std::string line_buf;

	/**
	* �f�[�^�̐擪�œǂݍ��݂��X�L�b�v����s���ł��B�����I�ɑ������܂��B
	*/
	unsigned int skip_line = 0;

	/**
	* ���ߐ��s�̃f�[�^�}�g���N�X�ł��B
	*/
	std::vector<std::vector<double> > source_data_matrix(ma_sample,
		std::vector<double>(MAX_TRACE_NUMBER));


	//�t�@�C����1�s���ǂݍ��݂܂��B�ǂݍ��񂾍s�͎��̃��[�v�Ŕj������܂��B
	//�z��̊e�v�f�ɕ������ꂽ��Ŏ����ϊ����ꂽ�}�g���N�X�͒��߂�ma_sample�s�������ێ�����܂��B
	for (size_t i = 0; std::getline(ifs, line_buf); i++) {

		//�s���Ŏw�肳�ꂽ�Z�p���[�^�ɂ���؂���s���܂��B
		std::vector<std::string> tok;
		csvSplit(line_buf, tok);

		//�Z�p���[�g���ꂽ�e�v�f�ɑ΂��Ď����ϊ����s���܂��B
		for (size_t x = 0; x < tok.size(); x++) {

			//�����ւ̕ϊ������݂܂��B
			try {
				source_data_matrix[i % ma_sample][x] = std::stod(tok[x]);
			}

			//�����񂪊܂܂��ȂǁA�����ւ̕ϊ��Ɏ��s�����ꍇ�͂��̍s��ǂݔ�΂��܂��B
			//���邢�́A�t�@�C���擪�̃R�����g�s�͎����I�ɓǂݔ�΂���܂��B
			//�A���A���̂悤�ȍs�̓t�@�C���擪�ɏW�����Ă���K�v������܂��B
			catch (std::invalid_argument) {
				skip_line++;
				break;
			}
		}

		//�}�g���N�X���S�ėL�ӂȃf�[�^�Ŗ��܂�܂œǂݍ��݂��J��Ԃ��܂�
		if (i < ma_sample + skip_line - 1) {
			//�t�@�C���擪�̃R�����g�s�͂��̂܂܏o�͂���܂��B
			std::cout << line_buf << std::endl;
			continue;
		}


		//�������炪�ړ����όv�Z�ł��B

		std::string out_line;
		//std::vector<double> output_data_line(tok.size(), 0);

		//�ړ����όv�Z�ł��B
		for (size_t x = 0; x < tok.size(); x++) {
			double sum = 0.0;
			for (size_t y = 0; y < ma_sample; y++)
				sum += source_data_matrix[(i - y) % ma_sample][x];
			out_line += (toExpString(sum / ma_sample) + ",");
		}

		//���ʂ��o�͂��܂��B
		std::cout << out_line << std::endl;
	}

	return 0;
}




