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

#include <boost/cast.hpp>

#include "objects.hpp"
#include "file.hpp"
#include "internationalisation.hpp"

namespace vjassdoc
{

Member::Member(class Parser *parser, const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Library *library, class Scope *scope, bool isPrivate, bool isPublic, bool isConstant, const std::string &typeExpression, const std::string &valueExpression, const std::string &sizeExpression, class Object *container, bool isStatic, bool isDelegate) : Global(parser, identifier, sourceFile, line, docComment, library, scope, isPrivate, isPublic, isConstant, typeExpression, valueExpression, sizeExpression), m_container(container), m_isStatic(isStatic), m_isDelegate(isDelegate)
{
}

Member::Member(Parser *parser) : Global(parser), m_container(0)
{
}

void Member::init()
{
	if (this->typeExpression() == File::expressionText[File::ThistypeExpression]) //is not as fast as direct set
		this->m_typeExpression = this->container()->identifier();
	else if (this->typeExpression() == File::expressionText[File::SuperExpression])
	{
		/// @todo ERROR
		this->m_typeExpression = boost::polymorphic_downcast<class Struct*>(this->container())->extensionExpression(); /// use expression since structs are initialized after members.
	}

	Global::init();
}

void Member::pageNavigation(std::ofstream &file) const
{
	Global::pageNavigation(file);
	file
	<< "\t\t\t<li><a href=\"#Container\">"	<< _("Container") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Static\">"	<< _("Static") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Delegate\">"	<< _("Delegate") << "</a></li>\n"
	;
}

void Member::page(std::ofstream &file) const
{
	Global::page(file);
	file
	<< "\t\t<h2><a name=\"Container\">" << _("Container") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->container()) << "\n"
	<< "\t\t<h2><a name=\"Static\">" << _("Static") << "</a></h2>\n"
	<< "\t\t" << Object::showBooleanProperty(this->isStatic()) << "\n"
	<< "\t\t<h2><a name=\"Delegate\">" << _("Delegate") << "</a></h2>\n"
	<< "\t\t" << Object::showBooleanProperty(this->isDelegate()) << "\n"
	;
}

#ifdef SQLITE
const char* Member::sqlTableName() const
{
	return "Members";
}

std::size_t Member::sqlSize() const
{
	return Global::sqlSize() + 3;
}

Object::SqlColumn Member::sqlNames() const
{
	SqlColumn result = Global::sqlNames();
	result.push_back("Container");
	result.push_back("IsStatic");
	result.push_back("IsDelegate");

	return result;
}

Object::SqlColumn Member::sqlTypes() const
{
	SqlColumn result = Global::sqlTypes();
	result.push_back("INT");
	result.push_back("BOOLEAN");
	result.push_back("BOOLEAN");

	return result;
}

Object::SqlColumn Member::sqlValues() const
{
	SqlColumn result = Global::sqlValues();
	result.push_back(objectIdString(this->container()));
	result.push_back(boost::lexical_cast<std::string>(this->isStatic()));
	result.push_back(boost::lexical_cast<std::string>(this->isDelegate()));

	return result;
}
#endif

class Object* Member::container() const
{
	return this->m_container;
}

}
