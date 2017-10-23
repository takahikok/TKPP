#include <vector>
#include <complex>

//typedef double Amp;               // amplitude at each sampling time
//typedef std::complex<double> std::complex<double>;     // constituent std::complex<double>uencies

namespace TKFFT
{
	std::vector<std::complex<double> > FFT(const std::vector<double> &x);
	std::vector<double> IFFT(const std::vector<std::complex<double> > &X);
}
