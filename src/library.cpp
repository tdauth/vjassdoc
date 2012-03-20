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

#include <boost/foreach.hpp>

#include "objects.hpp"
#include "internationalisation.hpp"
#include "vjassdoc.hpp"
#include "parser.hpp"

namespace vjassdoc
{

#ifdef SQLITE
const char *Library::sqlTableName = "Libraries";
unsigned int Library::sqlColumns;
std::string Library::sqlColumnStatement;
#endif

bool Library::HasRequirement::operator()(const Library *thisObject, const Library *library) const
{
	if (thisObject->requirements().get() != 0)
	{
		BOOST_FOREACH(Library::RequirementsContainer::const_reference ref, *thisObject->requirements())
		{
			if (ref.library() == library)
				return true;
		}
	}

	return false;
}

#ifdef SQLITE
void Library::initClass()
{
	Library::sqlColumns = Object::sqlColumns + 3;
	/// @todo Add class Requirement.
	Library::sqlColumnStatement = Object::sqlColumnStatement +
	",IsOnce BOOLEAN,"
	"Initializer INT,"
	"Requirement INT";
}
#endif

Library::Library(class Parser *parser, const std::string &identifier, class SourceFile *sourceFile, unsigned int line, DocComment *docComment, bool isOnce, const std::string &initializerExpression) : Object(parser, identifier, sourceFile, line, docComment), m_isOnce(isOnce), initializerExpression(initializerExpression), m_initializer(0)
{
}

#ifdef SQLITE
Library::Library(std::vector<const unsigned char*> &columnVector) : Object(columnVector), m_initializer(0)
{
}
#endif

Library::~Library()
{
}

void Library::init()
{
	if (!this->initializerExpression.empty())
	{
		this->m_initializer = boost::polymorphic_downcast<Function*>(this->parser()->searchObjectInList(this->initializerExpression, Parser::Functions, this));

		if (this->m_initializer == 0)
			this->m_initializer = boost::polymorphic_downcast<Function*>(this->parser()->searchObjectInList(this->initializerExpression, Parser::Methods, this));

		if (this->m_initializer != 0)
			this->initializerExpression.clear();
	}
	else
		this->initializerExpression = '-';

	if (this->requirements().get() != 0 && !this->requirements()->empty())
	{
		BOOST_FOREACH(RequirementsContainer::reference ref, *this->requirements())
		{
			Library *object = boost::polymorphic_downcast<Library*>(this->parser()->searchObjectInList(ref.expression(), Parser::Libraries, this));

			if (object != 0)
			{
				ref.expression().clear();
				ref.setLibrary(object);
			}
		}
	}
}

void Library::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Description\">"			<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source file\">"			<< _("Source file") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Once\">"				<< _("Once") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Initializer\">"			<< _("Initializer") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Requirement\">"			<< _("Requirement") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Keywords\">"				<< _("Keywords") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Text Macros\">"			<< _("Text Macros") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Text Macro Instances\">"		<< _("Text Macro Instances") << "</a></li>"
	<< "\t\t\t<li><a href=\"#Types\">"				<< _("Types") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Globals\">"				<< _("Globals") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Function Interfaces\">"		<< _("Function Interfaces") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Functions\">"				<< _("Functions") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Interfaces\">"				<< _("Interfaces") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Structs\">"				<< _("Structs") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Scopes\">"				<< _("Scopes") << "</a></li>\n"
	;
}

void Library::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << "\n"
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Once\">" << _("Once") << "</a></h2>\n"
	<< "\t\t" << Object::showBooleanProperty(this->isOnce()) << '\n'
	<< "\t\t<h2><a name=\"Initializer\">" << _("Initializer") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->initializer(), this->initializerExpression) << '\n'
	<< "\t\t<h2><a name=\"Requirement\">" << _("Requirement") << "</a></h2>\n"
	;

	if (this->requirements().get() != 0 && !this->requirements()->empty())
	{
		file << "\t\t<ul>\n";
		BOOST_FOREACH(RequirementsContainer::const_reference ref, *this->requirements())
		{
			file << "\t\t\t<li>";

			if (ref.isOptional())
				file << "optional ";

			file << Object::objectPageLink(ref.library(), ref.expression()) << "</li>\n";
		}

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t<p>-</p>\n";

	file
	<< "\t\t<h2><a name=\"Keywords\">" << _("Keywords") << "</a></h2>\n"
	;

	Parser::SpecificObjectList list = parser()->getSpecificList<IsInLibrary>(Parser::Keywords, this);

	if (!list.empty())
	{
		file << "\t\t<ul>\n";

		BOOST_FOREACH(Parser::SpecificObjectList::const_reference ref, list)
			file << "\t\t\t<li>" << Object::objectPageLink(ref.second) << "</li>\n";

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";

	file
	<< "\t\t<h2><a name=\"Text Macros\">" << _("Text Macros") << "</a></h2>\n"
	;

	list = parser()->getSpecificList<IsInLibrary>(Parser::TextMacros, this);

	if (!list.empty())
	{
		file << "\t\t<ul>\n";

		BOOST_FOREACH(Parser::SpecificObjectList::const_reference ref, list)
			file << "\t\t\t<li>" << Object::objectPageLink(ref.second) << "</li>\n";

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";

	file
	<< "\t\t<h2><a name=\"Text Macro Instances\">" << _("Text Macro Instances") << "</a></h2>\n"
	;

	list = parser()->getSpecificList<IsInLibrary>(Parser::TextMacroInstances, this);

	if (!list.empty())
	{
		file << "\t\t<ul>\n";

		BOOST_FOREACH(Parser::SpecificObjectList::const_reference ref, list)
			file << "\t\t\t<li>" << Object::objectPageLink(ref.second) << "</li>\n";

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";

	file
	<< "\t\t<h2><a name=\"Types\">" << _("Types") << "</a></h2>\n"
	;

	list = parser()->getSpecificList<IsInLibrary>(Parser::Types, this);

	if (!list.empty())
	{
		file << "\t\t<ul>\n";

		BOOST_FOREACH(Parser::SpecificObjectList::const_reference ref, list)
			file << "\t\t\t<li>" << Object::objectPageLink(ref.second) << "</li>\n";

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";

	file
	<< "\t\t<h2><a name=\"Globals\">" << _("Globals") << "</a></h2>\n"
	;

	list = parser()->getSpecificList<IsInLibrary>(Parser::Globals, this);

	if (!list.empty())
	{
		file << "\t\t<ul>\n";

		BOOST_FOREACH(Parser::SpecificObjectList::const_reference ref, list)
			file << "\t\t\t<li>" << Object::objectPageLink(ref.second) << "</li>\n";

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";

	file
	<< "\t\t<h2><a name=\"Function Interfaces\">" << _("Function Interfaces") << "</a></h2>\n"
	;

	list = parser()->getSpecificList<IsInLibrary>(Parser::FunctionInterfaces, this);

	if (!list.empty())
	{
		file << "\t\t<ul>\n";

		BOOST_FOREACH(Parser::SpecificObjectList::const_reference ref, list)
			file << "\t\t\t<li>" << Object::objectPageLink(ref.second) << "</li>\n";

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";

	file
	<< "\t\t<h2><a name=\"Functions\">" << _("Functions") << "</a></h2>\n"
	;

	list = parser()->getSpecificList<IsInLibrary>(Parser::Functions, this);

	if (!list.empty())
	{
		file << "\t\t<ul>\n";

		BOOST_FOREACH(Parser::SpecificObjectList::const_reference ref, list)
			file << "\t\t\t<li>" << Object::objectPageLink(ref.second) << "</li>\n";

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";

	file
	<< "\t\t<h2><a name=\"Interfaces\">" << _("Interfaces") << "</a></h2>\n"
	;

	list = parser()->getSpecificList<IsInLibrary>(Parser::Interfaces, this);

	if (!list.empty())
	{
		file << "\t\t<ul>\n";

		BOOST_FOREACH(Parser::SpecificObjectList::const_reference ref, list)
			file << "\t\t\t<li>" << Object::objectPageLink(ref.second) << "</li>\n";

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";

	file
	<< "\t\t<h2><a name=\"Structs\">" << _("Structs") << "</a></h2>\n"
	;

	list = parser()->getSpecificList<IsInLibrary>(Parser::Structs, this);

	if (!list.empty())
	{
		file << "\t\t<ul>\n";

		BOOST_FOREACH(Parser::SpecificObjectList::const_reference ref, list)
			file << "\t\t\t<li>" << Object::objectPageLink(ref.second) << "</li>\n";

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";

	file
	<< "\t\t<h2><a name=\"Scopes\">" << _("Scopes") << "</a></h2>\n"
	;

	list = parser()->getSpecificList<IsInLibrary>(Parser::Scopes, this);

	if (!list.empty())
	{
		file << "\t\t<ul>\n";

		BOOST_FOREACH(Parser::SpecificObjectList::const_reference ref, list)
			file << "\t\t\t<li>" << Object::objectPageLink(ref.second) << "</li>\n";

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";

	//contained keywords
	//contained text macros
	//contained text macro instances
	//contained types
	//contained globals
	//contained function interfaces
	//contained functions
	//contained interfaces
	//contained structs
	//contained scopes
}

#ifdef SQLITE
std::string Library::sqlStatement() const
{
	std::stringstream sstream;
	sstream
	<< Object::sqlStatement() << ", "
	<< "IsOnce=" << this->isOnce() << ", "
	<< "Initializer=" << Object::objectId(this->initializer()) << ", ";

	if (this->requirement() != 0)
		sstream << "Requirement=" << Object::objectId(this->requirement()->front()); //Array
	else
		sstream << "Requirement=-1";

	return sstream.str();
}
#endif

}
