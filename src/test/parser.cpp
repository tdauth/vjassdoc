/***************************************************************************
 *   Copyright (C) 2018 by Tamino Dauth                                    *
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

#define BOOST_TEST_MODULE ParserTest
#include <boost/test/unit_test.hpp>

#include "vjassdoc.hpp"
#include "parser.hpp"

using namespace vjassdoc;

BOOST_AUTO_TEST_CASE(ParseFunction)
{
	bool parseObjectsOfList[Parser::MaxLists] =
	{
		true, //comments
		true, //keywords
		true, //keys
		true, //text macros
		true, //text macro instances
		true, //types
		true, //locals
		true, //globals
		true, //members
		true, //parameters
		true, //function interfaces
		true, //functions
		true, //methods
		true, //calls
		true, //implementations
		true, //hooks
		true, //interfaces
		true, //structs
		true, //modules
		true, //scopes
		true, //libraries
		true, //source files
		true //doc comments
	};
	std::list<std::string> importDirs;
	std::list<std::string> filePaths;
	filePaths.push_back("function.j");
	std::list<std::string> databases;
	Vjassdoc program(false, false, true, true, true, false, false, false, true, "", "", false, false, false, parseObjectsOfList, "Test", "", importDirs, filePaths, databases);
	Parser parser(&program);
	parser.parse(filePaths);
}
