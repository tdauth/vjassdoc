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

#include <iostream>
#include <cstdio>

#include <boost/scoped_ptr.hpp>
#include <boost/timer.hpp>
#include <boost/format.hpp>

#include "vjassdoc.hpp"
#include "internationalisation.hpp"
#include "objects.hpp"

namespace vjassdoc
{

const char *Vjassdoc::version = "0.4";
#ifdef SQLITE
const bool Vjassdoc::supportsDatabaseCreation = true;
#else
const bool Vjassdoc::supportsDatabaseCreation = false;
#endif

Vjassdoc::Vjassdoc(bool optionJass, bool optionDebug, bool optionPrivate, bool optionTextmacros, bool optionFunctions, bool optionHtml, bool optionPages, bool optionSpecialpages, bool optionSyntax, const std::string &optionCompile, const std::string &optionDatabase, bool optionVerbose, bool optionTime, bool optionAlphabetical, bool optionParseObjectsOfList[Parser::MaxLists], const std::string &optionTitle, const std::string &optionDir, std::list<std::string> optionImport, std::list<std::string> optionFiles, std::list<std::string> optionDatabases) : m_parser(0), m_compiler(0)
{
	Vjassdoc::m_optionJass = optionJass;
	Vjassdoc::m_optionDebug = optionDebug;
	Vjassdoc::m_optionPrivate = optionPrivate;
	Vjassdoc::m_optionTextmacros = optionTextmacros;
	Vjassdoc::m_optionFunctions = optionFunctions;
	Vjassdoc::m_optionHtml = optionHtml;
	Vjassdoc::m_optionPages = optionPages;
	Vjassdoc::m_optionSpecialpages = optionSpecialpages;
	Vjassdoc::m_optionSyntax = optionSyntax;
	Vjassdoc::m_optionCompile = optionCompile;
	Vjassdoc::m_optionDatabase = optionDatabase;
	Vjassdoc::m_optionVerbose = optionVerbose;
	Vjassdoc::m_optionTime = optionTime;
	Vjassdoc::m_optionAlphabetical = optionAlphabetical;

	for (int i = 0; i < Parser::MaxLists; ++i)
		Vjassdoc::m_optionParseObjectsOfList[i] = optionParseObjectsOfList[i];

	Vjassdoc::m_optionTitle = optionTitle;
	Vjassdoc::m_optionDir = optionDir;
	Vjassdoc::m_optionImport = optionImport;
	Vjassdoc::m_optionFiles = optionFiles;
	Vjassdoc::m_optionDatabases = optionDatabases;
}

Vjassdoc::Vjassdoc()
{
	Vjassdoc::m_optionJass = false;
	Vjassdoc::m_optionDebug = false;
	Vjassdoc::m_optionPrivate = false;
	Vjassdoc::m_optionTextmacros = false;
	Vjassdoc::m_optionFunctions = false;
	Vjassdoc::m_optionHtml = false;
	Vjassdoc::m_optionPages = false;
	Vjassdoc::m_optionSpecialpages = false;
	Vjassdoc::m_optionSyntax = false;
	Vjassdoc::m_optionCompile = "";
	Vjassdoc::m_optionDatabase = "";
	Vjassdoc::m_optionVerbose = false;
	Vjassdoc::m_optionTime = false;
	Vjassdoc::m_optionAlphabetical = false;

	for (int i = 0; i < Parser::MaxLists; ++i)
		Vjassdoc::m_optionParseObjectsOfList[i] = false;

	Vjassdoc::m_optionTitle = "";
	Vjassdoc::m_optionDir = "";
	Vjassdoc::m_optionImport = std::list<std::string>();
	Vjassdoc::m_optionFiles = std::list<std::string>();
	Vjassdoc::m_optionDatabases = std::list<std::string>();
}

#ifdef SQLITE
void Vjassdoc::initClasses()
{
	Object::initClass();
	Comment::initClass();
	Keyword::initClass();
	Key::initClass();
	TextMacro::initClass();
	TextMacroInstance::initClass();
	Type::initClass();
	Local::initClass();
	Global::initClass();
	Member::initClass();
	Parameter::initClass();
	FunctionInterface::initClass();
	Function::initClass();
	Method::initClass();
	Call::initClass();
	Implementation::initClass();
	Hook::initClass();
	Interface::initClass();
	Struct::initClass();
	Module::initClass();
	Scope::initClass();
	Library::initClass();
	SourceFile::initClass();
	DocComment::initClass();
}
#endif

void Vjassdoc::run()
{
	Vjassdoc::m_lines = 0;
	Vjassdoc::m_files = 0;

	boost::scoped_ptr<boost::timer> timer;

	if (Vjassdoc::optionTime())
		timer.reset(new boost::timer());

#ifdef SQLITE
	if (!Vjassdoc::optionDatabases().empty())
	{
		if (Vjassdoc::optionVerbose())
			std::cout << _("You've selected one or several databases.") << std::endl;

		std::list<std::string> databases = Vjassdoc::optionDatabases();

		for (std::list<std::string>::iterator iterator = databases.begin(); iterator != databases.end(); ++iterator)
			Vjassdoc::parser()->addDatabase((*iterator).c_str());
	}
#endif

	Vjassdoc::parser()->parse(Vjassdoc::optionFiles());

	if (Vjassdoc::optionAlphabetical())
		Vjassdoc::parser()->sortAlphabetically();

	if (Vjassdoc::optionHtml())
		Vjassdoc::parser()->createHtmlFiles();

	if (Vjassdoc::optionSyntax())
		Vjassdoc::parser()->showSyntaxErrors();

#ifdef SQLITE
	//create SQL database for search functions
	if (!Vjassdoc::optionDatabase().empty())
		Vjassdoc::parser()->createDatabase(Vjassdoc::optionDatabase());
#endif

	if (!Vjassdoc::optionCompile().empty())
		Vjassdoc::compiler()->compile();

	if (Vjassdoc::optionTime())
		std::cout << boost::format(_("Duration: %1% ms")) % timer->elapsed() << std::endl;

	std::cout << boost::format(_("Finished (%1% lines, %2% files).")) % Vjassdoc::m_lines % Vjassdoc::m_files << std::endl;
}

Vjassdoc::~Vjassdoc()
{
	if (Vjassdoc::m_parser != 0)
	{
		delete Vjassdoc::m_parser;
		Vjassdoc::m_parser = 0;
	}

	if (Vjassdoc::m_compiler != 0)
	{
		delete Vjassdoc::m_compiler;
		Vjassdoc::m_compiler = 0;
	}
}

}
