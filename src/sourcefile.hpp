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

#ifndef VJASSDOC_SOURCEFILE_HPP
#define VJASSDOC_SOURCEFILE_HPP

#include "object.hpp"

namespace vjassdoc
{

class SourceFile : public Object
{
	public:
#ifdef SQLITE
		static const char *sqlTableName;
		static unsigned int sqlColumns;
		static std::string sqlColumnStatement;

		static void initClass();
#endif
		SourceFile(const std::string &identifier, const std::string &path);
#ifdef SQLITE
		SourceFile(std::vector<const unsigned char*> &columnVector);
#endif
		virtual void init();
		virtual void pageNavigation(std::ofstream &file) const;
		virtual void page(std::ofstream &file) const;
#ifdef SQLITE
		virtual std::string sqlStatement() const;
#endif
		std::string path() const;
		std::string lineLink(const unsigned int &line, const std::string &text) const;
		
		/**
		* @param sourceFileName If this value is true, the name of source file will be shown as link. Otherwise the line will be shown.
		*/
		static std::string sourceFileLineLink(const class Object *object, const bool &sourceFileName = true, const std::string &identifier = "-");

	protected:
		//Do not use
		class SourceFile* sourceFile() const;
		unsigned int line() const;
		class DocComment* docComment() const;

		std::string m_path;
};

inline std::string SourceFile::path() const
{
	return this->m_path;
}

inline std::string SourceFile::lineLink(const unsigned int &line, const std::string &text) const
{
	std::ostringstream sstream;
	sstream << "<a href=\"" << this->id() << ".html#" << line << "\">" << text << "</a>";
	return sstream.str();
}

inline std::string SourceFile::sourceFileLineLink(const class Object *object, const bool &sourceFileName, const std::string &identifier)
{
	if (object->sourceFile() == 0)
		return identifier;
	
	if (sourceFileName)
		return object->sourceFile()->lineLink(object->line(), object->sourceFile()->identifier());
	
	std::ostringstream sstream;
	sstream << object->line();
	return object->sourceFile()->lineLink(object->line(), sstream.str());
}

}

#endif
