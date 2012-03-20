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

#include <boost/cast.hpp>
#include <boost/foreach.hpp>

#include "objects.hpp"
#include "internationalisation.hpp"
#include "vjassdoc.hpp"

namespace vjassdoc
{

#ifdef SQLITE
const char *Interface::sqlTableName = "Interfaces";
unsigned int Interface::sqlColumns;
std::string Interface::sqlColumnStatement;

void Interface::initClass()
{
	Interface::sqlColumns = Object::sqlColumns + 3;
	Interface::sqlColumnStatement = Object::sqlColumnStatement +
	",Library INT,"
	"Scope INT,"
	"IsPrivate BOOLEAN";
}
#endif

Interface::Interface(class Parser *parser, const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Library *library, class Scope *scope, bool isPrivate) : Object(parser, identifier, sourceFile, line, docComment), m_library(library), m_scope(scope), m_isPrivate(isPrivate)
{
}

#ifdef SQLITE
Interface::Interface(std::vector<const unsigned char*> &columnVector) : Object(columnVector), m_library(0), m_scope(0)
{
}
#endif

void Interface::init()
{
}

void Interface::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Library\">"		<< _("Library") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Scope\">"		<< _("Scope") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Private\">"		<< _("Private") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Members\">"		<< _("Members") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Implementations\">"	<< _("Implementations") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Methods\">"		<< _("Methods") << "</a></li>\n"
	;
}

void Interface::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << '\n'
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Library\">" << _("Library") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->library()) << '\n'
	<< "\t\t<h2><a name=\"Scope\">" << _("Scope") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->scope()) << '\n'
	<< "\t\t<h2><a name=\"Private\">" << _("Private") << "</a></h2>\n"
	<< "\t\t" << Object::showBooleanProperty(this->isPrivate()) << '\n'
	<< "\t\t<h2><a name=\"Members\">" << _("Members") << "</a></h2>\n"
	;

	this->getMemberList(file, this->parser()->getSpecificList<IsInContainer>(Parser::Members, this));

	file
	<< "\t\t<h2><a name=\"Implementations\">" << _("Implementations") << "</a></h2>\n"
	;

	this->getImplementationList(file);

	file
	<< "\t\t<h2><a name=\"Methods\">" << _("Methods") << "</a></h2>\n"
	;

	this->getMethodList(file, this->parser()->getSpecificList<IsInContainer>(Parser::Methods, this));
}

#ifdef SQLITE
std::string Interface::sqlStatement() const
{
	std::ostringstream sstream;
	sstream
	<< Object::sqlStatement() << ", "
	<< "Library=" << Object::objectId(this->library()) << ", "
	<< "Scope=" << Object::objectId(this->scope()) << ", "
	<< "IsPrivate=" << this->isPrivate();

	return sstream.str();
}
#endif

class Library* Interface::library() const
{
	return this->m_library;
}

class Scope* Interface::scope() const
{
	return this->m_scope;
}

void Interface::getMemberList(std::ofstream &file, const Parser::SpecificObjectList &memberList) const
{
	if (!memberList.empty())
	{
		file << "\t\t<ul>\n";

		BOOST_FOREACH(Parser::SpecificObjectList::const_reference ref, memberList)
		{
			const Member *member = boost::polymorphic_downcast<Member*>(ref.second);

			file << "\t\t\t<li>";

			if (member->isPublic())
				file << "public ";

			if (member->isPrivate())
				file << "private ";

			if (member->isStatic())
				file << "static ";

			if (member->isConstant())
				file << "constant ";

			if (member->isDelegate())
				file << "delegate ";

			file << Object::objectPageLink(member->type(), member->typeExpression()) << ' ' << Object::objectPageLink(member);

			if (member->size() != 0)
				file << '[' << Object::objectPageLink(member->size()) << ']';
			else if (member->sizeLiteral() != -1)
				file << '[' << member->sizeLiteral() << ']';
			else if (member->value() != 0) //arrays can not have sizes
				file << " = " << Object::objectPageLink(member->value());
			else if (member->valueLiteral() != "-")
				file << " = " << member->valueLiteral();

			file << "</li>\n";
		}

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";
}

void Interface::getImplementationList(std::ofstream &file) const
{
	Parser::SpecificObjectList implementationList = parser()->getSpecificList<IsInContainer>(Parser::Implementations, this);

	if (!implementationList.empty())
	{
		file << "\t\t<ul>\n";

		BOOST_FOREACH(Parser::SpecificObjectList::const_reference ref, implementationList)
		{
			const Implementation *implementation = boost::polymorphic_downcast<Implementation*>(ref.second);

			file << "\t\t\t<li>";

			if (implementation->isOptional())
				file << "optional ";

			file
			<< Object::objectPageLink(implementation)
			<< "</li>\n"
			;
		}

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";
}

void Interface::getMethodList(std::ofstream &file, const Parser::SpecificObjectList &methodList) const
{
	if (!methodList.empty())
	{
		file << "\t\t<ul>\n";

		BOOST_FOREACH(Parser::SpecificObjectList::const_reference ref, methodList)
		{
			const Method *method = boost::polymorphic_downcast<Method*>(ref.second);

			file << "\t\t\t<li>";

			if (method->isPublic())
				file << "public ";

			if (method->isPrivate())
				file << "private ";

			if (method->isStatic())
				file << "static ";

			if (method->isConstant())
				file << "constant ";

			if (method->isStub())
				file << "stub ";

			file << method->pageLink();

			file << "</li>\n";
		}

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";
}

}
