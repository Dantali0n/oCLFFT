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

#ifndef DFTSEQ_H
#define DFTSEQ_H

#include "oclfft.hpp"

class dftseq : public oCLFFT {
	public:
		using oCLFFT::oCLFFT;
		void synchronize() override;
		void window() override;
		void compute() override;
		void magnitude() override;
	protected:
		// use constexpr as these must be fully computed at compile-time
		static constexpr double TWO_PI = 2 * M_PI;
		static constexpr double FOUR_PI = 4 * M_PI;
		static constexpr double SIX_PI = 6 * M_PI;

		template <class T> T sq(T x) {
			return x * x;
		}
};

#endif // DFTSEQ_H