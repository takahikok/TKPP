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
			std::cerr << "第3引数を読み込めませんでした" << std::endl;
	case 3:
		try {
			ma_sample = std::stoi(argv[2]);
		}
		catch (std::invalid_argument) {
			std::cerr << "第2引数を読み込めませんでした" << std::endl;
		}
	case 2:
		;
	}

	std::ifstream ifs(static_cast<std::string>(argv[1]));
	std::string line_buf;

	/**
	* データの先頭で読み込みをスキップする行数です。自動的に代入されます。
	*/
	unsigned int skip_line = 0;

	/**
	* 直近数行のデータマトリクスです。
	*/
	std::vector<std::vector<double> > source_data_matrix(ma_sample,
		std::vector<double>(MAX_TRACE_NUMBER));


	//ファイルを1行ずつ読み込みます。読み込んだ行は次のループで破棄されます。
	//配列の各要素に分割された上で実数変換されたマトリクスは直近のma_sample行分だけ保持されます。
	for (size_t i = 0; std::getline(ifs, line_buf); i++) {

		//行内で指定されたセパレータによる区切りを行います。
		std::vector<std::string> tok;
		csvSplit(line_buf, tok);

		//セパレートされた各要素に対して実数変換を行います。
		for (size_t x = 0; x < tok.size(); x++) {

			//実数への変換を試みます。
			try {
				source_data_matrix[i % ma_sample][x] = std::stod(tok[x]);
			}

			//文字列が含まれるなど、実数への変換に失敗した場合はその行を読み飛ばします。
			//あるいは、ファイル先頭のコメント行は自動的に読み飛ばされます。
			//但し、そのような行はファイル先頭に集中している必要があります。
			catch (std::invalid_argument) {
				skip_line++;
				break;
			}
		}

		//マトリクスが全て有意なデータで埋まるまで読み込みを繰り返します
		if (i < ma_sample + skip_line - 1) {
			//ファイル先頭のコメント行はそのまま出力されます。
			std::cout << line_buf << std::endl;
			continue;
		}


		//ここからが移動平均計算です。

		std::string out_line;
		//std::vector<double> output_data_line(tok.size(), 0);

		//移動平均計算です。
		for (size_t x = 0; x < tok.size(); x++) {
			double sum = 0.0;
			for (size_t y = 0; y < ma_sample; y++)
				sum += source_data_matrix[(i - y) % ma_sample][x];
			out_line += (toExpString(sum / ma_sample) + ",");
		}

		//結果を出力します。
		std::cout << out_line << std::endl;
	}

	return 0;
}




