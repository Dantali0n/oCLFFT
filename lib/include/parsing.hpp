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

#ifndef OCLFFT_PARSING_H
#define OCLFFT_PARSING_H

#include "oclfft.hpp"

class CSVRow {
public:
	std::string const& operator[](std::size_t index) const;

	std::size_t size() const;

	void readNextRow(std::istream& str);
private:
	std::vector<std::string> m_data;
};

// Cannot be member function, could potentially be 'friend' but I did not
// bother.
std::istream& operator>>(std::istream& str, CSVRow& data);

#endif //OCLFFT_PARSING_H
