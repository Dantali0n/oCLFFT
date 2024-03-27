/*
	oCLFFT
	Copyright (C) 2024 Corne Lukken

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

#include "defaults.hpp"
#include "options.hpp"
#include "parsing.hpp"

namespace po = boost::program_options;

void parse_args(int argc, char* argv[], options *option) {
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Produce help message")
		("samples,s", po::value<size_t>(&option->samples)->default_value(DEFAULT_SAMPLES), "Set number of samples")
        ("precision,p", po::value<size_t>(&option->precision)->default_value(DEFAULT_PRECISION), "Set precision in number of digits")
        ("scale,f", po::value<float>(&option->scale)->default_value(DEFAULT_SCALE), "Set random noise scale");
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << std::endl;
	}
}