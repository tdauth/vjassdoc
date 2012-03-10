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

#ifndef VJASSDOC_VJASSDOC_HPP
#define VJASSDOC_VJASSDOC_HPP

#include <string>
#include <list>

#include "parser.hpp"
#include "compiler.hpp"
#include "vjassdocConfig.h"

namespace vjassdoc
{

class Parser;

class Vjassdoc
{
	public:
		static const char *version;
		static const bool supportsDatabaseCreation;

		Vjassdoc(bool optionJass, bool optionDebug, bool optionPrivate, bool optionTextmacros, bool optionFunctions, bool optionHtml, bool optionPages, bool optionSpecialpages, bool optionSyntax, const std::string &optionCompile, const std::string &optionDatabase, bool optionVerbose, bool optionTime, bool optionAlphabetical, bool optionParseObjectsOfList[Parser::MaxLists], const std::string &optionTitle, const std::string &optionDir, std::list<std::string> optionImport, std::list<std::string> optionFiles, std::list<std::string> optionDatabases);
		Vjassdoc();

#ifdef SQLITE
		static void initClasses();
#endif
		void run();
		~Vjassdoc();
		class Parser* parser() const;
		class Compiler* compiler() const;
		unsigned int lines() const;
		unsigned int files() const;
		bool optionJass() const;
		bool optionDebug() const;
		bool optionPrivate() const;
		bool optionTextmacros() const;
		bool optionFunctions() const;
		bool optionHtml() const;
		bool optionPages() const;
		bool optionSpecialpages() const;
		bool optionSyntax() const;
		const std::string& optionCompile() const;
		const std::string& optionDatabase() const;
		bool optionVerbose() const;
		bool optionTime() const;
		bool optionAlphabetical() const;
		bool optionParseObjectsOfList(enum Parser::List list) const;
		const std::string& optionTitle() const;
		const std::string& optionDir() const;
		const std::list<std::string>& optionImport() const;
		const std::list<std::string>& optionFiles() const;
		const std::list<std::string>& optionDatabases() const;

		void addLines(unsigned int addedLines);
		void addFile();
	private:
		//operator=(const Vjassdoc&);
		Vjassdoc(const Vjassdoc&);

		class Parser *m_parser;
		class Compiler *m_compiler;
		unsigned int m_lines;
		unsigned int m_files;
		bool m_optionJass;
		bool m_optionDebug;
		bool m_optionPrivate;
		bool m_optionTextmacros;
		bool m_optionFunctions;
		bool m_optionHtml;
		bool m_optionPages;
		bool m_optionSpecialpages;
		bool m_optionSyntax;
		std::string m_optionCompile;
		std::string m_optionDatabase;
		bool m_optionVerbose;
		bool m_optionTime;
		bool m_optionAlphabetical;
		bool m_optionParseObjectsOfList[Parser::MaxLists];
		std::string m_optionTitle;
		std::string m_optionDir;
		std::list<std::string> m_optionImport;
		std::list<std::string> m_optionFiles;
		std::list<std::string> m_optionDatabases;
};

inline Parser* Vjassdoc::parser() const
{
	if (this->m_parser == 0)
		const_cast<Vjassdoc*>(this)->m_parser = new Parser(const_cast<Vjassdoc*>(this));

	return this->m_parser;
}

inline Compiler* Vjassdoc::compiler() const
{
	if (this->m_compiler == 0)
		const_cast<Vjassdoc*>(this)->m_compiler = new Compiler(const_cast<Vjassdoc*>(this));

	return this->m_compiler;
}

inline unsigned int Vjassdoc::lines() const
{
	return Vjassdoc::m_lines;
}

inline unsigned int Vjassdoc::files() const
{
	return Vjassdoc::m_files;
}

inline bool Vjassdoc::optionJass() const
{
	return Vjassdoc::m_optionJass;
}

inline bool Vjassdoc::optionDebug() const
{
	return Vjassdoc::m_optionDebug;
}

inline bool Vjassdoc::optionPrivate() const
{
	return Vjassdoc::m_optionPrivate;
}

inline bool Vjassdoc::optionTextmacros() const
{
	return Vjassdoc::m_optionTextmacros;
}

inline bool Vjassdoc::optionFunctions() const
{
	return Vjassdoc::m_optionFunctions;
}

inline bool Vjassdoc::optionHtml() const
{
	return Vjassdoc::m_optionHtml;
}

inline bool Vjassdoc::optionPages() const
{
	return Vjassdoc::m_optionPages;
}

inline bool Vjassdoc::optionSpecialpages() const
{
	return Vjassdoc::m_optionSpecialpages;
}

inline bool Vjassdoc::optionSyntax() const
{
	return Vjassdoc::m_optionSyntax;
}

inline const std::string& Vjassdoc::optionCompile() const
{
	return Vjassdoc::m_optionCompile;
}

inline const std::string& Vjassdoc::optionDatabase() const
{
	return Vjassdoc::m_optionDatabase;
}

inline bool Vjassdoc::optionVerbose() const
{
	return Vjassdoc::m_optionVerbose;
}

inline bool Vjassdoc::optionTime() const
{
	return Vjassdoc::m_optionTime;
}

inline bool Vjassdoc::optionAlphabetical() const
{
	return Vjassdoc::m_optionAlphabetical;
}

inline bool Vjassdoc::optionParseObjectsOfList(Parser::List list) const
{
	return Vjassdoc::m_optionParseObjectsOfList[list];
}

inline const std::string& Vjassdoc::optionTitle() const
{
	return Vjassdoc::m_optionTitle;
}

inline const std::string& Vjassdoc::optionDir() const
{
	return Vjassdoc::m_optionDir;
}

inline const std::list<std::string>& Vjassdoc::optionImport() const
{
	return Vjassdoc::m_optionImport;
}

inline const std::list<std::string>& Vjassdoc::optionFiles() const
{
	return Vjassdoc::m_optionFiles;
}

inline const std::list<std::string>& Vjassdoc::optionDatabases() const
{
	return Vjassdoc::m_optionDatabases;
}

inline void Vjassdoc::addLines(unsigned int addedLines)
{
	Vjassdoc::m_lines += addedLines;
}

inline void Vjassdoc::addFile()
{
	++Vjassdoc::m_files;
}

}

#endif
