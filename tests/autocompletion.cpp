/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
 *   tamino@cdauth.de                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <string>
#include <iostream>
#include <cstdlib>

#include "vjassdoc.hpp"
#include "objects.hpp"

using namespace vjassdoc;

int main()
{
	std::string line;
	std::cout << "Auto Completion test created by Tamino Dauth" << std::endl;
	std::cout << "This test shows how the auto completion works." << std::endl;
	std::cout << "Enter your code line, please." << std::endl;
	std::cin >> line;
	std::size_t index = 0;
	std::cout << "Enter your line index number, please." << std::endl;
	std::cin >> index;
	// Usually you should call Vjassdoc::configure before using it.
	Vjassdoc::parser()->add(new Struct("Hans", 0, 0, 0, 0, 0, false, std::string(), std::string()));
	std::list<class Object*> results = Vjassdoc::parser()->autoCompletion(line, index);
	std::cout << "Results:" << std::endl;

	for (std::list<class Object*>::iterator iterator = results.begin(); iterator != results.end(); ++iterator)
		std::cout << (*iterator)->identifier() << std::endl;

	Vjassdoc::clear(); // Deletes the parser.

	return EXIT_SUCCESS;
}
