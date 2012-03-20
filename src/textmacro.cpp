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

#include <sstream>

#include <boost/cast.hpp>
#include <boost/foreach.hpp>

#include "objects.hpp"
#include "internationalisation.hpp"

namespace vjassdoc
{

TextMacro::TextMacro(class Parser *parser, const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, bool isOnce, const std::string &parameters) : Object(parser, identifier, sourceFile, line, docComment), m_isOnce(isOnce), m_parameters(parameters)
{
}

TextMacro::TextMacro(class Parser *parser) : Object(parser), m_isOnce(false)
{
}

void TextMacro::init()
{
}

void TextMacro::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Once\">"		<< _("Once") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Parameters\">"		<< _("Parameters") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Instances\">"		<< _("Instances") << "</a></li>\n"
	;
}

void TextMacro::page(std::ofstream &file) const
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
	<< "\t\t<h2><a name=\"Parameters\">" << _("Parameters") << "</a></h2>\n"
	<< "\t\t" << this->parameters() << '\n'
	<< "\t\t<h2><a name=\"Instances\">" << _("Instances") << "</a></h2>\n"
	;
	Parser::SpecificObjectList instanceList = parser()->getSpecificList<TextMacroInstance::UsesTextMacro>(Parser::TextMacroInstances, this);

	if (!instanceList.empty())
	{
		file << "\t\t<ul>\n";

		BOOST_FOREACH(Parser::SpecificObjectList::const_reference ref, instanceList)
		{
			TextMacroInstance *instance = boost::polymorphic_downcast<TextMacroInstance*>(ref.second);

			file << "\t\t\t<li>";
			file << Object::objectPageLink(instance->sourceFile()) << " - " << Object::objectPageLink(instance);
			file << "</li>\n";
		}

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";
}

#ifdef SQLITE
const char* TextMacro::sqlTableName() const
{
	return "TextMacros";
}

std::size_t TextMacro::sqlSize() const
{
	return vjassdoc::Object::sqlSize() + 2;
}

Object::SqlColumn TextMacro::sqlNames() const
{
	SqlColumn result = vjassdoc::Object::sqlNames();
	result.push_back("IsOnce");
	result.push_back("Parameters");

	return result;
}

Object::SqlColumn TextMacro::sqlTypes() const
{
	SqlColumn result = vjassdoc::Object::sqlTypes();
	result.push_back("BOOLEAN");
	result.push_back("VARCHAR(255)");

	return result;
}

Object::SqlColumn TextMacro::sqlValues() const
{
	SqlColumn result = vjassdoc::Object::sqlValues();
	result.push_back(boost::lexical_cast<std::string>(this->isOnce()));
	result.push_back(Object::sqlFilteredString(this->parameters()));

	return result;
}
#endif

}
