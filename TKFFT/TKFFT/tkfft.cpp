#include "tkfft.h"
#include <vector>
#include <complex>

namespace TKFFT
{
	const double PI = 3.14159265358979;

	static int bit_reverse(int x, int n)
	{
		int rev = 0;

		for (--n; n > 0; n >>= 1, x >>= 1)
			rev = (rev << 1) | (x & 1);

		return rev;
	}

	static std::vector<std::complex<double> > genFFT(const std::vector<std::complex<double> > &x, int sgn)
	{
		int N = x.size();
		std::vector<std::complex<double> > X(N);

		// set init values with bit reversed
		for (int i = 0; i < N; i++) {
			int rev = bit_reverse(i, N);
			X[i] = x[rev];
		}

		// butterfly diagrams
		for (int b = 2; b <= N; b <<= 1) {
			for (int i = 0; i < N; i++) {
				if (i % b >= b / 2)
					continue;

				int k = i + b / 2;
				std::complex<double> Wi = exp(std::complex<double>(0, sgn * 2 * PI*(i%b) / b));
				std::complex<double> Wk = exp(std::complex<double>(0, sgn * 2 * PI*(k%b) / b));

				X[i] += Wi*X[k];
				X[k] = Wk*X[k] + (X[i] - Wi*X[k]);
			}
		}

		return X;
	}

	std::vector<std::complex<double> > FFT(const std::vector<double> &x)
	{
		int N = x.size();

		// convert real numbers to complex numbers
		std::vector<std::complex<double> > x_t(N);
		for (int i = 0; i < N; i++)
			x_t[i] = std::complex<double>(x[i], 0);

		return genFFT(x_t, -1);
	}

	std::vector<double> IFFT(const std::vector<std::complex<double> > &X)
	{
		int N = X.size();

		std::vector<std::complex<double> > x_t = genFFT(X, 1);

		// convert complex numbers to real numbers
		// also normalize the amplituds (devide by N)
		std::vector<double>x(N);
		for (int i = 0; i < N; i++)
			x[i] = x_t[i].real() / N;

		return x;
	}

}
