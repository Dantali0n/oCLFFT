/*

	FFT libray
	Copyright (C) 2010 Didier Longueville
	Copyright (C) 2014 Enrique Condes

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "ard-seq.hpp"

arduinoFFT::arduinoFFT(
	double *vReal, double *vImag, uint32_t samples, double samplingFrequency)
{
	this->_vReal = vReal;
	this->_vImag = vImag;
	this->_samples = samples;
	this->_samplingFrequency = samplingFrequency;
	this->_power = Exponent(samples);
}

arduinoFFT::~arduinoFFT(void)
{
	// Destructor
}

void arduinoFFT::Compute(uint8_t dir)
{
	// reorder //
	uint32_t j = 0;
	for (uint32_t i = 0; i < (this->_samples - 1); i++) {
		if (i < j) {
			Swap(&this->_vReal[i], &this->_vReal[j]);
			Swap(&this->_vImag[i], &this->_vImag[j]);
			if(dir==FFT_REVERSE)
				Swap(&this->_vImag[i], &this->_vImag[j]);
		}

		// Reverse bits /
		uint32_t k = (this->_samples >> 1);
		while (k <= j) {
			j -= k;
			k >>= 1;
		}
		j += k;
	}
	// Compute the FFT  /
//#ifdef __AVR__
//	uint8_t index = 0;
//#endif
	double c1 = -1.0;
	double c2 = 0.0;
	uint32_t l2 = 1;
	for (uint8_t l = 0; (l < this->_power); l++) {
		uint32_t l1 = l2;
		l2 <<= 1;
		//printf("l2: %u\n", l2);
		double u1 = 1.0;
		double u2 = 0.0;
		for (j = 0; j < l1; j++) {
			for (uint32_t i = j; i < this->_samples; i += l2) {
				uint32_t i1 = i + l1;
				double t1 = u1 * this->_vReal[i1] - u2 * this->_vImag[i1];
				double t2 = u1 * this->_vImag[i1] + u2 * this->_vReal[i1];
				this->_vReal[i1] = this->_vReal[i] - t1;
				this->_vImag[i1] = this->_vImag[i] - t2;
				this->_vReal[i] += t1;
				this->_vImag[i] += t2;
			}
			double z = ((u1 * c1) - (u2 * c2));
			u2 = ((u1 * c2) + (u2 * c1));
			u1 = z;
		}
//#ifdef __AVR__
//		c2 = pgm_read_float_near(&(_c2[index]));
//		c1 = pgm_read_float_near(&(_c1[index]));
//		index++;
//#else
		c2 = sqrt((1.0 - c1) / 2.0);
		c1 = sqrt((1.0 + c1) / 2.0);
//#endif
		if (dir == FFT_FORWARD) {
			c2 = -c2;
		}
	}
	// Scaling for reverse transform /
	if (dir != FFT_FORWARD) {
		for (uint32_t i = 0; i < this->_samples; i++) {
			this->_vReal[i] /= this->_samples;
			this->_vImag[i] /= this->_samples;
		}
	}
}

void arduinoFFT::ComplexToMagnitude()
{ // vM is half the size of vReal and vImag
	for (uint32_t i = 0; i < this->_samples; i++) {
		this->_vReal[i] = sqrt(sq(this->_vReal[i]) + sq(this->_vImag[i]));
	}
}

void arduinoFFT::DCRemoval()
{
	// calculate the mean of vData
	double mean = 0;
	for (uint32_t i = 1; i < ((this->_samples >> 1) + 1); i++)
	{
		mean += this->_vReal[i];
	}
	mean /= this->_samples;
	// Subtract the mean from vData
	for (uint32_t i = 1; i < ((this->_samples >> 1) + 1); i++)
	{
		this->_vReal[i] -= mean;
	}
}

void arduinoFFT::Windowing(uint8_t windowType, uint8_t dir)
{// Weighing factors are computed once before multiple use of FFT
// The weighing function is symetric; half the weighs are recorded
	double samplesMinusOne = (double(this->_samples) - 1.0);
	for (uint32_t i = 0; i < (this->_samples >> 1); i++) {
		double indexMinusOne = double(i);
		double ratio = (indexMinusOne / samplesMinusOne);
		double weighingFactor = 1.0;
		// Compute and record weighting factor
		switch (windowType) {
			case FFT_WIN_TYP_RECTANGLE: // rectangle (box car)
				weighingFactor = 1.0;
				break;
			case FFT_WIN_TYP_HAMMING: // hamming
				weighingFactor = 0.54 - (0.46 * cos(TWO_PI * ratio));
				break;
			case FFT_WIN_TYP_HANN: // hann
				weighingFactor = 0.54 * (1.0 - cos(TWO_PI * ratio));
				break;
			case FFT_WIN_TYP_TRIANGLE: // triangle (Bartlett)
				weighingFactor = 1.0 - ((2.0 * abs(indexMinusOne - (samplesMinusOne / 2.0))) / samplesMinusOne);
				break;
			case FFT_WIN_TYP_NUTTALL: // nuttall
				weighingFactor = 0.355768 - (0.487396 * (cos(TWO_PI * ratio))) + (0.144232 * (cos(FOUR_PI * ratio))) - (0.012604 * (cos(SIX_PI * ratio)));
				break;
			case FFT_WIN_TYP_BLACKMAN: // blackman
				weighingFactor = 0.42323 - (0.49755 * (cos(TWO_PI * ratio))) + (0.07922 * (cos(FOUR_PI * ratio)));
				break;
			case FFT_WIN_TYP_BLACKMAN_NUTTALL: // blackman nuttall
				weighingFactor = 0.3635819 - (0.4891775 * (cos(TWO_PI * ratio))) + (0.1365995 * (cos(FOUR_PI * ratio))) - (0.0106411 * (cos(SIX_PI * ratio)));
				break;
			case FFT_WIN_TYP_BLACKMAN_HARRIS: // blackman harris
				weighingFactor = 0.35875 - (0.48829 * (cos(TWO_PI * ratio))) + (0.14128 * (cos(FOUR_PI * ratio))) - (0.01168 * (cos(SIX_PI * ratio)));
				break;
			case FFT_WIN_TYP_FLT_TOP: // flat top
				weighingFactor = 0.2810639 - (0.5208972 * cos(TWO_PI * ratio)) + (0.1980399 * cos(FOUR_PI * ratio));
				break;
			case FFT_WIN_TYP_WELCH: // welch
				weighingFactor = 1.0 - sq((indexMinusOne - samplesMinusOne / 2.0) / (samplesMinusOne / 2.0));
				break;
		}
		if (dir == FFT_FORWARD) {
			this->_vReal[i] *= weighingFactor;
			this->_vReal[this->_samples - (i + 1)] *= weighingFactor;
		}
		else {
			this->_vReal[i] /= weighingFactor;
			this->_vReal[this->_samples - (i + 1)] /= weighingFactor;
		}
	}
}

double arduinoFFT::MajorPeak()
{
	double maxY = 0;
	uint32_t IndexOfMaxY = 0;
	//If sampling_frequency = 2 * max_frequency in signal,
	//value would be stored at position samples/2
	for (uint32_t i = 1; i < ((this->_samples >> 1) + 1); i++) {
		if ((this->_vReal[i-1] < this->_vReal[i]) && (this->_vReal[i] > this->_vReal[i+1])) {
			if (this->_vReal[i] > maxY) {
				maxY = this->_vReal[i];
				IndexOfMaxY = i;
			}
		}
	}
	double delta = 0.5 * ((this->_vReal[IndexOfMaxY-1] - this->_vReal[IndexOfMaxY+1]) / (this->_vReal[IndexOfMaxY-1] - (2.0 * this->_vReal[IndexOfMaxY]) + this->_vReal[IndexOfMaxY+1]));
	double interpolatedX = ((IndexOfMaxY + delta)  * this->_samplingFrequency) / (this->_samples-1);
	if(IndexOfMaxY==(this->_samples >> 1)) //To improve calculation on edge values
		interpolatedX = ((IndexOfMaxY + delta)  * this->_samplingFrequency) / (this->_samples);
	// returned value: interpolated frequency peak apex
	return(interpolatedX);
}

void arduinoFFT::MajorPeak(double *f, double *v)
{
	double maxY = 0;
	uint32_t IndexOfMaxY = 0;
	//If sampling_frequency = 2 * max_frequency in signal,
	//value would be stored at position samples/2
	for (uint32_t i = 1; i < ((this->_samples >> 1) + 1); i++) {
		if ((this->_vReal[i - 1] < this->_vReal[i]) && (this->_vReal[i] > this->_vReal[i + 1])) {
			if (this->_vReal[i] > maxY) {
				maxY = this->_vReal[i];
				IndexOfMaxY = i;
			}
		}
	}
	double delta = 0.5 * ((this->_vReal[IndexOfMaxY - 1] - this->_vReal[IndexOfMaxY + 1]) / (this->_vReal[IndexOfMaxY - 1] - (2.0 * this->_vReal[IndexOfMaxY]) + this->_vReal[IndexOfMaxY + 1]));
	double interpolatedX = ((IndexOfMaxY + delta)  * this->_samplingFrequency) / (this->_samples - 1);
	if (IndexOfMaxY == (this->_samples >> 1)) //To improve calculation on edge values
		interpolatedX = ((IndexOfMaxY + delta)  * this->_samplingFrequency) / (this->_samples);
	// returned value: interpolated frequency peak apex
	*f = interpolatedX;
	*v = abs(this->_vReal[IndexOfMaxY - 1] - (2.0 * this->_vReal[IndexOfMaxY]) + this->_vReal[IndexOfMaxY + 1]);
}

uint8_t arduinoFFT::Exponent(uint32_t value)
{
	// Calculates the base 2 logarithm of a value
	uint8_t result = 0;
	while (((value >> result) & 1) != 1) result++;
	return(result);
}

void arduinoFFT::Swap(double *x, double *y)
{
	double temp = *x;
	*x = *y;
	*y = temp;
}

template <class T>
T arduinoFFT::sq(T x) {
	return x*x;
}