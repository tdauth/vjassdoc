/***************************************************************************
 *   Copyright (C) 2008, 2009 by Tamino Dauth                              *
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

Keyword::Keyword(class Parser *parser, const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Library *library, class Scope *scope, bool isPrivate) : Object(parser, identifier, sourceFile, line, docComment), m_library(library), m_scope(scope), m_isPrivate(isPrivate)
{
}

Keyword::Keyword(Parser *parser) : Object(parser), m_library(0)
{
}

#ifdef SQLITE
const char* Keyword::sqlTableName() const
{
	return "Keywords";
}

std::size_t Keyword::sqlSize() const
{
	return vjassdoc::Object::sqlSize() + 3;
}

Object::SqlColumn Keyword::sqlNames() const
{
	SqlColumn result = Object::sqlNames();
	result.push_back("Library");
	result.push_back("Scope");
	result.push_back("IsPrivate");

	return result;
}

Object::SqlColumn Keyword::sqlTypes() const
{
	SqlColumn result = Object::sqlTypes();
	result.push_back("INT");
	result.push_back("INT");
	result.push_back("BOOLEAN");

	return result;
}

Object::SqlColumn Keyword::sqlValues() const
{
	SqlColumn result = Object::sqlValues();
	result.push_back(objectIdString(this->library()));
	result.push_back(objectIdString(this->scope()));
	result.push_back(boost::lexical_cast<std::string>(this->isPrivate()));

	return result;
}
#endif

void Keyword::init()
{
}

void Keyword::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Library\">"		<< _("Library") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Scope\">"		<< _("Scope") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Private\">"		<< _("Private") << "</a></li>\n"
	;
}

void Keyword::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << '\n'
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Library\">" << _("Library") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->library()) << "\n"
	<< "\t\t<h2><a name=\"Scope\">" << _("Scope") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->scope()) << "\n"
	<< "\t\t<h2><a name=\"Private\">" << _("Private") << "</a></h2>\n"
	<< "\t\t" << Object::showBooleanProperty(this->isPrivate()) << "\n"
	;
}

class Library* Keyword::library() const
{
	return this->m_library;
}

class Scope* Keyword::scope() const
{
	return this->m_scope;
}

}
