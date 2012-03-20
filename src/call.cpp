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

#include <sstream>

#include "objects.hpp"
#include "internationalisation.hpp"

namespace vjassdoc
{

#ifdef SQLITE
const int Call::maxArguments = 10;
const char *Call::sqlTableName = "Calls";
unsigned int Call::sqlColumns;
std::string Call::sqlColumnStatement;

void Call::initClass()
{
	Call::sqlColumns = Object::sqlColumns + 2 + 2 * Call::maxArguments + 2;
	Call::sqlColumnStatement = Object::sqlColumnStatement +
	",FunctionIdentifier VARCHAR(255)"
	",Function INT";
	std::ostringstream sstream;

	for (int i = 0; i < Call::maxArguments; ++i)
		sstream << ",ArgumentIdentifier" << i << " VARCHAR(255)";

	for (int i = 0; i < Call::maxArguments; ++i)
		sstream << ",Argument" << i << " INT";

	Call::sqlColumnStatement += sstream.str();
	Call::sqlColumnStatement +=
	",IsExecuted BOOLEAN"
	",IsEvaluated BOOLEAN"
	;
}
#endif

Call::Call(class Parser *parser, const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, const std::string &functionIdentifier, std::list<std::string> *argumentIdentifiers, bool isExecuted, bool isEvaluated) : Object(parser, identifier, sourceFile, line, docComment), m_functionIdentifier(functionIdentifier), m_function(0), m_argumentIdentifiers(argumentIdentifiers), m_arguments(0), m_isExecuted(isExecuted), m_isEvaluated(isEvaluated)
{
}

#ifdef SQLITE
Call::Call(std::vector<const unsigned char*> &columnVector) : Object(columnVector)
{
	this->prepareVector();
}
#endif

Call::~Call()
{
	if (this->m_argumentIdentifiers != 0)
		delete this->m_argumentIdentifiers;

	if (this->m_arguments != 0)
		delete this->m_arguments;
}

void Call::init()
{
	this->m_function = boost::polymorphic_downcast<class Function*>(this->parser()->searchObjectInList(this->m_functionIdentifier, Parser::Functions, this));

	if (this->m_function == 0)
		this->m_function = boost::polymorphic_downcast<class Function*>(this->parser()->searchObjectInList(this->m_functionIdentifier, Parser::Methods, this));

	if (this->m_function != 0)
		this->m_functionIdentifier.clear();

	this->m_arguments = new std::list<class Object*>;

	bool foundAll = true;

	for (std::list<std::string>::const_iterator iterator = this->m_argumentIdentifiers->begin(); iterator != this->m_argumentIdentifiers->end(); ++iterator)
	{
		class Object *object = this->parser()->searchObjectInList(*iterator, Parser::Locals, this);

		/// @todo Call arguments can be many different Object child classes.

		if (object == 0)
			foundAll = false;
	}

	if (foundAll)
	{
		delete this->m_argumentIdentifiers;
		this->m_argumentIdentifiers = 0;
	}

}

void Call::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Function\">"		<< _("Function") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Arguments\">"		<< _("Arguments") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Execution\">"		<< _("Execution") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Evaluation\">"		<< _("Evaluation") << "</a></li>\n"
	;
}

void Call::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << "\n"
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Function\">" << _("Function") << "</a></h2>\n"
	<< "\t\t" << Object::objectLink(this->function()) << '\n'
	;
}

#ifdef SQLITE
std::string Call::sqlStatement() const
{
	std::ostringstream sstream;
	sstream
	<< "FunctionIdentifier=\"" << Object::sqlFilteredString(this->m_functionIdentifier) << "\", "
	<< "Function=" << Object::objectId(this->m_function) << ", "
	;
	int i = 0;

	for (std::list<std::string>::const_iterator iterator = this->m_argumentIdentifiers->begin(); iterator != this->m_argumentIdentifiers->end() && i < Call::maxArguments; ++iterator, ++i)
		sstream << ", ArgumentIdentifier" << i << ' ' << *iterator;

	for ( ; i < Call::maxArguments; ++i)
		sstream << ", ArgumentIdentifier" << i << " NULL";

	i = 0;

	for (std::list<class Object*>::const_iterator iterator = this->m_arguments->begin(); iterator != this->m_arguments->end() && i < Call::maxArguments; ++iterator, ++i)
		sstream << ",Argument" << i << ' ' << (*iterator)->id();

	sstream
	<< ",IsExecuted " << this->m_isExecuted
	<< ",IsEvaluated " << this->m_isEvaluated
	;

	return sstream.str();
}
#endif

}
