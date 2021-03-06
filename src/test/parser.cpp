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
#include "objects.hpp"

using namespace vjassdoc;

struct MyFixture
{
	MyFixture() : program(false, false, true, true, true, false, false, false, true, "", "", false, false, false, parseObjectsOfList, "Test", "", importDirs, filePaths, databases), parser(&program)
	{
	}

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
	std::list<std::string> databases;
	std::list<std::string> filePaths;
	Vjassdoc program;
	Parser parser;
};

BOOST_FIXTURE_TEST_CASE(ParseFunction, MyFixture)
{
	std::list<std::string> filePaths;
	filePaths.push_back("function.j");
	parser.parse(filePaths);
	BOOST_REQUIRE_EQUAL(1, parser.getList(Parser::Functions).size());
	Function *f = dynamic_cast<Function*>(parser.searchObjectInList("pow", Parser::Functions));
	BOOST_REQUIRE(f != nullptr);
	BOOST_CHECK(!f->isPublic());
	BOOST_CHECK(!f->isConstant());
	BOOST_CHECK(!f->isNative());
	BOOST_CHECK(f->library() == nullptr);
	BOOST_CHECK(f->scope() == nullptr);
	BOOST_CHECK(!f->isPrivate());
	BOOST_CHECK_EQUAL(2, f->parameters().size());
	const FunctionInterface::Parameters &parameters = f->parameters();
	BOOST_CHECK_EQUAL("a", parameters.front()->identifier());
	BOOST_CHECK(parameters.front()->type() != nullptr);
	BOOST_CHECK_EQUAL(parser.integerType(), parameters.front()->type());
	BOOST_CHECK_EQUAL("b", parameters.back()->identifier());
	BOOST_CHECK(parameters.back()->type() != nullptr);
	BOOST_CHECK_EQUAL(parser.integerType(), parameters.back()->type());
	BOOST_CHECK(f->returnType() != nullptr);
	BOOST_CHECK_EQUAL(parser.integerType(), f->returnType());
}

BOOST_FIXTURE_TEST_CASE(ParseNativeFunction, MyFixture)
{
	std::list<std::string> filePaths;
	filePaths.push_back("native.j");
	parser.parse(filePaths);
	BOOST_REQUIRE_EQUAL(1, parser.getList(Parser::Functions).size());
	Function *f = dynamic_cast<Function*>(parser.searchObjectInList("ConvertRace", Parser::Functions));
	BOOST_REQUIRE(f != nullptr);
	BOOST_CHECK(!f->isPublic());
	BOOST_CHECK(f->isConstant());
	BOOST_CHECK(f->isNative());
	BOOST_CHECK(f->library() == nullptr);
	BOOST_CHECK(f->scope() == nullptr);
	BOOST_CHECK(!f->isPrivate());
	BOOST_CHECK_EQUAL(1, f->parameters().size());
	const FunctionInterface::Parameters &parameters = f->parameters();
	BOOST_CHECK_EQUAL("i", parameters.front()->identifier());
	BOOST_CHECK(parameters.front()->type() != nullptr);
	BOOST_CHECK_EQUAL(parser.integerType(), parameters.front()->type());
	BOOST_CHECK(f->returnType() == nullptr);
	BOOST_CHECK_EQUAL("race", f->returnTypeExpression());
}

BOOST_FIXTURE_TEST_CASE(ParseType, MyFixture)
{
	std::list<std::string> filePaths;
	filePaths.push_back("type.j");
	parser.parse(filePaths);
	/*
	 * There are already default JASS types.
	 */
	BOOST_REQUIRE_EQUAL(7, parser.getList(Parser::Types).size());
	Type *t = dynamic_cast<Type*>(parser.searchObjectInList("agent", Parser::Types));
	BOOST_REQUIRE(t != nullptr);
	BOOST_CHECK_EQUAL("agent", t->identifier());
	BOOST_CHECK(t->size() == nullptr);
	BOOST_CHECK(t->type() != nullptr);
	BOOST_CHECK_EQUAL(parser.handleType(), t->type());
}
