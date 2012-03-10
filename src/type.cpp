/***************************************************************************
 *   Copyright (C) 2008 by Tamino Dauth                                    *
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

#include <cctype>
#include <sstream>

#include <boost/cast.hpp>

#include "objects.hpp"
#include "internationalisation.hpp"

namespace vjassdoc
{

Type::Type(class Parser *parser, const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, const std::string &typeExpression, const std::string &sizeExpression) : typeExpression(typeExpression), sizeExpression(sizeExpression), m_type(0), m_size(0), Object(parser, identifier, sourceFile, line, docComment)
{
}

Type::Type(Parser *parser) : m_type(0), m_size(0), Object(parser)
{
}

void Type::init()
{
	if (!this->typeExpression.empty())
	{
		this->m_type = boost::polymorphic_cast<Type*>(this->parser()->searchObjectInList(this->typeExpression, Parser::Types));

		if (this->m_type != 0)
			this->typeExpression.clear();
	}
	else
		this->typeExpression = '-';

	if (this->sizeExpression.empty())
	{
		this->sizeExpression = '-';
		return;
	}

	if (isdigit(this->sizeExpression[0])) //expression can be an integer like 43
		return;

	this->m_size = this->parser()->searchObjectInList(this->sizeExpression, Parser::Globals, this);

	if (this->m_size == 0)
		this->m_size = this->parser()->searchObjectInList(this->sizeExpression, Parser::Members, this);

	if (this->m_size == 0)
		this->m_size = this->parser()->searchObjectInList(this->sizeExpression, Parser::Functions, this);

	if (this->m_size == 0)
		this->m_size = this->parser()->searchObjectInList(this->sizeExpression, Parser::Methods, this);

	if (this->m_size != 0)
		this->sizeExpression.clear();
}

void Type::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Inherited Type\">"	<< _("Inherited Type") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Size\">"		<< _("Size") << "</a></li>\n"
	;
}

void Type::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << '\n'
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Inherited Type\">" << _("Inherited Type") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->type(), this->typeExpression) << '\n'
	<< "\t\t<h2><a name=\"Size\">" << _("Size") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->size(), this->sizeExpression) << '\n'
	;
}

#ifdef SQLITE
const char* Type::sqlTableName() const
{
	return "Types";
}

std::size_t Type::sqlSize() const
{
	return Object::sqlSize() + 2;
}

Object::SqlColumn Type::sqlNames() const
{
	SqlColumn result = Object::sqlNames();
	result.push_back("Type");
	result.push_back("Size");

	return result;
}

Object::SqlColumn Type::sqlTypes() const
{
	SqlColumn result = Object::sqlTypes();
	result.push_back("INT");
	result.push_back("INT");

	return result;
}

Object::SqlColumn Type::sqlValues() const
{
	SqlColumn result = Object::sqlValues();
	result.push_back(objectIdString(this->type()));
	result.push_back(objectIdString(this->size()));

	return result;
}
#endif

}
