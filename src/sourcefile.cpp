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

#ifdef SQLITE
const char *SourceFile::sqlTableName = "SourceFiles";
unsigned int SourceFile::sqlColumns;
std::string SourceFile::sqlColumnStatement;

void SourceFile::initClass()
{
	SourceFile::sqlColumns = 2;
	SourceFile::sqlColumnStatement =
	"Identifier VARCHAR(255),"
	"Path VARCHAR(255)";
}
#endif

SourceFile::SourceFile(class Parser *parser, const std::string &identifier, const std::string &path) : Object(parser, identifier, 0, 0, 0), m_path(path)
{
}

#ifdef SQLITE
SourceFile::SourceFile(std::vector<const unsigned char*> &columnVector) : Object(columnVector)
{
}
#endif

void SourceFile::init()
{
}

void SourceFile::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Code\">"	<< _("Code") << "</a></li>\n"
	;
}

void SourceFile::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Code\">" << _("Code") << "</a></h2>\n"
	<< "\t\t<pre>\n"
	<< "\t\t<code>\n"
	;

	std::ifstream sourceFile(this->path().c_str());
	int i = 1;

	while (sourceFile.good())
	{
		std::string line;
		std::getline(sourceFile, line);
		file << i << "\t\t\t<a name=\"" << i << "\" class=\"sourcefilecode\">" << line << "</a><br>\n";
		++i;
	}

	sourceFile.close();
	file
	<< "\t\t</code>\n"
	<< "\t\t</pre>\n"
	;

}

#ifdef SQLITE
std::string SourceFile::sqlStatement() const
{
	std::ostringstream sstream;
	sstream
	<< "Identifier=\"" << Object::sqlFilteredString(this->identifier()) << "\", "
	<< "Path=\"" << this->path() << '\"';

	return sstream.str();
}
#endif

}
