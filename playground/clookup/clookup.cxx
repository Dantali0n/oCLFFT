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

#include <math.h>
#include <iostream>
#include <limits>

typedef std::numeric_limits<double> db_lim;

int main(int argc, char* argv[]) {
	size_t l = 1;
	double c1 = -1.0;
	double c2 = 0.0;

	std::cout.precision(db_lim::digits10);

	for(size_t i = 0; i < 22; i++) {
		double u1 = 1.0;
		double u2 = 0.0;
		l <<= 1;

		std::cout << "__constant double u1" << i << "[] = {";
		std::cout << u1 << ",";
		for(size_t x = 0; x < l; x++) {
			double z = ((u1 * c1) - (u2 * c2));
			std::cout << z;
			if(x+1 != l) std::cout << ",";
			if(x % 5 == 0) std::cout << "\n";
			u2 = ((u1 * c2) + (u2 * c1));
			u1 = z;
		}
		std::cout << "};" << std::endl;

		u1 = 1.0;
		u2 = 0.0;
		std::cout << "__constant double u2" << i << "[] = {";
		std::cout << u2 << ",";
		for(size_t x = 0; x < l; x++) {
			double z = ((u1 * c1) - (u2 * c2));
			u2 = ((u1 * c2) + (u2 * c1));
			std::cout << u2;
			if(x+1 != l) std::cout << ",";
			if(x % 5 == 0) std::cout << "\n";
			u1 = z;
		}
		std::cout << "};" << std::endl;

		c2 = -sqrt((1.0 - c1) / 2.0);
		c1 = sqrt((1.0 + c1) / 2.0);
	}
}