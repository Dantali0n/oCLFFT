/*
	oCLFFT
	Copyright (C) 2021 Corne Lukken

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

#include "oclfft.hpp"
#include "parsing.hpp"

namespace po = boost::program_options;

namespace oclfft {
	std::istream &operator>>(std::istream &in, Output &output) {
		std::string token;
		in >> token;

		boost::to_upper(token);

		if (token == "TIME") {
			output = OUT_TIME;
		} else if (token == "NONE") {
			output = OUT_NONE;
		} else if (token == "REAL") {
			output = OUT_REAL;
		} else if (token == "IMAGINARY") {
			output = OUT_IMAGINARY;
		} else {
			throw po::invalid_option_value("");
		}

		return in;
	}
}

oCLFFT::oCLFFT(std::vector<std::complex<double>> *data) {
	this->data = data;
}

void complex_to_magnitude(fftw_complex *data, size_t n) {
	for (size_t i = 0; i < n; i++) {
		double data_r = data[i][0];
		double data_i = data[i][1];
		data[i][0] = sqrt((data_r*data_r) + (data_i*data_i));
	}
}

void bit_reverse(std::vector<std::complex<double>> *data) {
	size_t samples = data->size();

	uint32_t j = 0;
	for (uint32_t i = 0; i < (samples - 1); i++) {
		if (i < j) {
			std::complex<double> temp = (*data)[i];
			(*data)[i] = (*data)[j];
			(*data)[j] = temp;
		}

		// Reverse bits /
		uint32_t k = (samples >> 1);
		while (k <= j) {
			j -= k;
			k >>= 1;
		}
		j += k;
	}
}

void window_nuttall(std::vector<std::complex<double>> *data) {
	const size_t n = data->size();
	const double TWO_PI = 2*M_PI;
	const double FOUR_PI = 4*M_PI;
	const double SIX_PI = 6*M_PI;
	const double samplesMinusOne = (double(n) - 1.0);

	for (size_t i = 0; i < (n >> 1); i++) {
		double ratio = ((double)i / samplesMinusOne);
		double weighingFactor = 0.355768 -
			(0.487396 * (cos(TWO_PI * ratio))) +
			(0.144232 * (cos(FOUR_PI * ratio))) -
			(0.012604 * (cos(SIX_PI * ratio)));
		(*data)[i].real((*data)[i].real() * weighingFactor);
		(*data)[n - (i + 1)].real((*data)[n - (i + 1)].real() * weighingFactor);
	}
}

void parse_args(int argc, char* argv[], struct oclfft::options *options) {
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Produce help message")
		("samples,s", po::value<size_t>(&options->samples)->default_value(oclfft::DEFAULT_SAMPLES), "Set number of samples")
		("file,f", po::value<std::string>(), "Input data csv")
		("output,o",
			po::value<oclfft::Output>(&options->output)->default_value(oclfft::OUT_TIME),
			R"(Generate output: "time", "none", "real" or "imaginary")"
		);
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << std::endl;
	}

	if(vm.count("file")) {
		options->file = std::make_shared<std::string>(vm["file"].as<std::string>());
	}
	else {
		options->file = std::make_shared<std::string>("");
	}

}

void parse_file(std::string *file, std::vector<std::complex<double>> *data,
	size_t limit)
{
	if(file->empty()) return;

	try {
		size_t current = 0;
		std::ifstream ifile(*file);
		CSVRow row;

		// First row provides mode select
		ifile >> row;
		if (row.size() == 3) {
			data->push_back(std::complex<double>(std::stod(row[1]), std::stod(row[2])));
			current++;
			if(current == limit) return;
			while (ifile >> row) {
				data->push_back(std::complex<double>(std::stod(row[1]), std::stod(row[2])));
				current++;
				if(current == limit) return;
			}
		}
		else if(row.size() == 2){
			data->push_back(std::complex<double>(std::stod(row[1]), 0));
			current++;
			if(current == limit) return;
			while (ifile >> row) {
				data->push_back(std::complex<double>(std::stod(row[1]), 0));
				current++;
				if(current == limit) return;
			}
		}
		else {
			std::cerr << "Error while parsing input file" << std::endl;
			exit(1);
		}
	}
	catch(...) {
		std::cerr << "Error while parsing input file" << std::endl;
		exit(1);
	}

	// Reduce to nearest power of two
	size_t samples = data->size();
	size_t log_2 = round(log2(samples));
	size_t samples_new = pow(2, log_2-1);
	while(samples_new != samples) {
		data->pop_back();
		samples--;
	}
}

void generate_output(std::vector<std::complex<double>> *result,
	std::vector<std::complex<double>> *original, oclfft::Output type)
{
	size_t num_samples = result->size();

	fftw_complex *fw_in, *fw_out;
	fftw_plan p;
	fw_in = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * num_samples);
	fw_out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * num_samples);

	auto begin = std::chrono::high_resolution_clock::now();
	window_nuttall(original);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "FFTW window: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

	for(size_t i = 0; i < num_samples; i++) {
		fw_in[i][0] = (*original)[i].real();
		fw_in[i][1] = (*original)[i].imag();
	}

	begin = std::chrono::high_resolution_clock::now();
	p = fftw_plan_dft_1d(
		num_samples, fw_in, fw_out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(p);
	end = std::chrono::high_resolution_clock::now();
	std::cout << "FFTW fft: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

	begin = std::chrono::high_resolution_clock::now();
	complex_to_magnitude(fw_out, num_samples);
	end = std::chrono::high_resolution_clock::now();
	std::cout << "FFTW magnitude: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

	// The OUT_NONE match is redundant
	if(type != oclfft::OUT_TIME && type != oclfft::OUT_NONE) {
		bool imaginary = false;
		if (type == oclfft::OUT_IMAGINARY) imaginary = true;
		generate_output(fw_out, result, num_samples, imaginary);
	}

	fftw_destroy_plan(p);
	fftw_free(fw_in);
	fftw_free(fw_out);
}

void generate_output(
	fftw_complex *data_ref, std::vector<std::complex<double>> *data_tar,
	size_t n, bool imaginary
) {
	std::cout.precision(oclfft::db_lim::digits10);
	std::cout << "ref" << std::endl;
	for (size_t i = 0; i < n >> 1; i++) {
		if (imaginary)
			std::cout << std::scientific << data_ref[i][1] << std::endl;
		else
			std::cout << std::scientific << data_ref[i][0] << std::endl;
	}

	std::cout << "tar" << std::endl;
	for (size_t i = 0; i < n >> 1; i++) {
		if (imaginary)
			std::cout << std::scientific << (*data_tar)[i].imag() << std::endl;
		else
			std::cout << std::scientific << (*data_tar)[i].real() << std::endl;
	}

	std::cout << "eof" << std::endl;
}

void generate_output(
	double *data_ref_r, double *data_ref_i, double *data_tar_r,
	double *data_tar_i, size_t n, bool imaginary
) {
	double *data_ref = data_ref_r;
	if(imaginary) data_ref = data_ref_i;

	double *data_tar = data_tar_r;
	if(imaginary) data_tar = data_tar_i;

	std::cout.precision(oclfft::db_lim::digits10);
	std::cout << "ref" << std::endl;
	for (size_t i = 0; i < n >> 1; i++) {
		std::cout << std::scientific << data_ref[i] << std::endl;
	}

	std::cout << "tar" << std::endl;
	for (size_t i = 0; i < n >> 1; i++) {
		std::cout << std::scientific << data_tar[i] << std::endl;
	}

	std::cout << "eof" << std::endl;
}

void generate_output(
	double *data_ref_r, double *data_ref_i,
	std::vector<std::complex<double>> *data_tar, size_t n, bool imaginary
) {
	double *data_ref = data_ref_r;
	if(imaginary) data_ref = data_ref_i;

	std::cout.precision(oclfft::db_lim::digits10);
	std::cout << "ref" << std::endl;
	for (size_t i = 0; i < n >> 1; i++) {
		std::cout << std::scientific << data_ref[i] << std::endl;
	}

	std::cout << "tar" << std::endl;
	for (size_t i = 0; i < n >> 1; i++) {
		if (imaginary)
			std::cout << std::scientific << (*data_tar)[i].imag() << std::endl;
		else
			std::cout << std::scientific << (*data_tar)[i].real() << std::endl;
	}

	std::cout << "eof" << std::endl;
}