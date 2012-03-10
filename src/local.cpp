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

#include "objects.hpp"
#include "vjassdoc.hpp"
#include "internationalisation.hpp"

namespace vjassdoc
{

Local::Local(class Parser *parser, const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Function *function, const std::string &typeExpression, const std::string &valueExpression) : Object(parser, identifier, sourceFile, line, docComment), m_function(function), m_type(0), m_typeExpression(typeExpression), m_value(0), m_valueExpression(valueExpression)
{
}

Local::Local(Parser *parser) : Object(parser), m_function(0), m_type(0), m_value(0)
{
}

/// @todo Value expressions can be calculations etc..
void Local::init()
{
	//Must not be empty.
	this->m_type = this->parser()->searchObjectInList(this->typeExpression(), Parser::Types, this);

	if (this->m_type == 0)
		this->m_type = this->parser()->searchObjectInList(this->typeExpression(), Parser::Interfaces, this);

	if (this->m_type == 0)
		this->m_type = this->parser()->searchObjectInList(this->typeExpression(), Parser::Structs, this);

	if (this->m_type != 0)
		this->m_typeExpression.clear();

	this->m_value = this->findValue(this->type(), this->m_valueExpression);
}

void Local::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Function\">"		<< _("Function") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Type\">"		<< _("Type") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Value\">"		<< _("Value") << "</a></li>\n"
	;
}

void Local::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << '\n'
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Function\">" << _("Function") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->function()) << "\n"
	<< "\t\t<h2><a name=\"Type\">" << _("Type") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->type(), this->typeExpression()) << '\n'
	<< "\t\t<h2><a name=\"Value\">" << _("Value") << "</a></h2>\n"
	;

	if (this->valueExpression().empty() || this->valueExpression() == "-")
		file << "\t\t" << Object::objectPageLink(this->value(), this->valueExpression()) << '\n';
	else
	{
		file << "\t\t";

		if (this->value() != 0)
			file << Object::objectPageLink(this->value());

		file << "\t\t" << this->valueExpression() << '\n';
	}
}

#ifdef SQLITE
const char* Local::sqlTableName() const
{
	return "Locals";
}

std::size_t Local::sqlSize() const
{
	return Object::sqlSize() + 5;
}

Object::SqlColumn Local::sqlNames() const
{
	SqlColumn result = Object::sqlNames();
	result.push_back("Function");
	result.push_back("Type");
	result.push_back("TypeExpression");
	result.push_back("Value");
	result.push_back("ValueExpression");

	return result;
}

Object::SqlColumn Local::sqlTypes() const
{
	SqlColumn result = Object::sqlTypes();
	result.push_back("INT");
	result.push_back("INT");
	result.push_back("VARCHAR(50)");
	result.push_back("INT");
	result.push_back("VARCHAR(50)");

	return result;
}

Object::SqlColumn Local::sqlValues() const
{
	SqlColumn result = Object::sqlValues();
	result.push_back(objectIdString(this->function()));
	result.push_back(objectIdString(this->type()));
	result.push_back(sqlFilteredString(this->typeExpression()));
	result.push_back(objectIdString(this->value()));
	result.push_back(sqlFilteredString(this->valueExpression()));

	return result;
}
#endif

}
