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

#include "objects.hpp"
#include "internationalisation.hpp"

namespace vjassdoc
{

TextMacroInstance::TextMacroInstance(class Parser *parser, const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, bool isOptional, const std::string &arguments) : TextMacro(parser, identifier, sourceFile, line, docComment, false, arguments), m_textMacro(0), m_isOptional(isOptional) /// isOnce of @class TextMacro is unnecessary
{
}

TextMacroInstance::TextMacroInstance(Parser *parser) : TextMacro(parser), m_textMacro(0)
{
	/// @todo m_isOptional
}

void TextMacroInstance::init()
{
	TextMacro::init();

	// mustn't be empty
	this->m_textMacro = boost::polymorphic_cast<class TextMacro*>(this->parser()->searchObjectInList(this->identifier(), Parser::TextMacros, this));
}

void TextMacroInstance::pageNavigation(std::ofstream &file) const
{
	//do not use TextMacro::pageNavigation() because there are listed all instances and isOnce.
	file
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Arguments\">"		<< _("Arguments") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Text Macro\">"		<< _("Text Macro") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Optional\">"		<< _("Optional") << "</a></li>\n"
	;
}

void TextMacroInstance::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << '\n'
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Arguments\">" << _("Arguments") << "</a></h2>\n"
	<< "\t\t" << this->parameters() << '\n'
	<< "\t\t<h2><a name=\"Text Macro\">" << _("Text Macro") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->textMacro(), this->identifier()) << '\n'
	<< "\t\t<h2><a name=\"Optional\">" << _("Optional") << "</a></h2>\n"
	<< "\t\t" << Object::showBooleanProperty(this->isOptional()) << '\n'
	;
}

#ifdef SQLITE
const char* TextMacroInstance::sqlTableName() const
{
	return "TextMacroInstances";
}

std::size_t TextMacroInstance::sqlSize() const
{
	return TextMacro::sqlSize() + 2;
}

Object::SqlColumn TextMacroInstance::sqlNames() const
{
	SqlColumn result = TextMacro::sqlNames();
	result.push_back("TextMacro");
	result.push_back("IsOptional");

	return result;
}

Object::SqlColumn TextMacroInstance::sqlTypes() const
{
	SqlColumn result = TextMacro::sqlTypes();
	result.push_back("INT");
	result.push_back("BOOLEAN");

	return result;
}

Object::SqlColumn TextMacroInstance::sqlValues() const
{
	SqlColumn result = TextMacro::sqlValues();
	result.push_back(Object::objectIdString(this->textMacro()));
	result.push_back(boost::lexical_cast<std::string>(this->isOptional()));

	return result;
}
#endif

}
