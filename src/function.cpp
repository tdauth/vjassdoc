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

#include "objects.hpp"
#include "internationalisation.hpp"

namespace vjassdoc
{

#ifdef SQLITE
const char *Function::sqlTableName = "Functions";
unsigned int Function::sqlColumns;
std::string Function::sqlColumnStatement;

void Function::initClass()
{
	Function::sqlColumns = FunctionInterface::sqlColumns + 3;
	Function::sqlColumnStatement = FunctionInterface::sqlColumnStatement +
	",IsPublic BOOL,"
	"IsConstant BOOL,"
	"IsNative BOOL";
}
#endif

Function::Function(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Library *library, class Scope *scope, bool isPrivate, std::list<class Parameter*> parameters, const std::string &returnTypeExpression, bool isPublic, bool isConstant, bool isNative) : FunctionInterface(identifier, sourceFile, line, docComment, library, scope, isPrivate, parameters, returnTypeExpression), m_isPublic(isPublic), m_isConstant(isConstant), m_isNative(isNative)
{
}

#ifdef SQLITE
Function::Function(std::vector<const unsigned char*> &columnVector) : FunctionInterface(columnVector)
{
	this->prepareVector();
}
#endif

void Function::init()
{
	FunctionInterface::init();
}

void Function::pageNavigation(std::ofstream &file) const
{
	FunctionInterface::pageNavigation(file);
	file
	<< "\t\t\t<li><a href=\"#Public\">"	<< _("Public") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Native\">"	<< _("Native") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Constant\">"	<< _("Constant") << "</a></li>\n"
	;
}

void Function::page(std::ofstream &file) const
{
	FunctionInterface::page(file);
	file
	<< "\t\t<h2><a name=\"Public\">" << _("Public") << "</a></h2>\n"
	<< "\t\t" << Object::showBooleanProperty(this->isPublic()) << "\n"
	<< "\t\t<h2><a name=\"Native\">" << _("Native") << "</a></h2>\n"
	<< "\t\t" << Object::showBooleanProperty(this->isNative()) << "\n"
	<< "\t\t<h2><a name=\"Constant\">" << _("Constant") << "</a></h2>\n"
	<< "\t\t" << Object::showBooleanProperty(this->isConstant()) << "\n"
	;
}

#ifdef SQLITE
std::string Function::sqlStatement() const
{
	std::ostringstream sstream;
	sstream
	<< FunctionInterface::sqlStatement() << ", "
	<< "IsPublic=" << this->isPublic() << ", "
	<< "IsConstant=" << this->isConstant() << ", "
	<< "IsNative=" << this->isNative();

	return sstream.str();
}
#endif

}
