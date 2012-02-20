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

#include <sys/time.h>
#include <ctime>
#include <iostream>
#include <cstdio>

#include "vjassdoc.hpp"
#include "internationalisation.hpp"
#include "objects.hpp"

namespace vjassdoc
{

const char *Vjassdoc::version = vjassdoc_VERSION;
#ifdef SQLITE
const bool Vjassdoc::supportsDatabaseCreation = true;
#else
const bool Vjassdoc::supportsDatabaseCreation = false;
#endif
class Parser *Vjassdoc::m_parser = 0;
class Compiler *Vjassdoc::m_compiler = 0;
unsigned int Vjassdoc::m_lines = 0;
unsigned int Vjassdoc::m_files = 0;
double Vjassdoc::m_duration = 0.0;
double Vjassdoc::m_cpuDuration = 0.0;
bool Vjassdoc::m_optionJass = false;
bool Vjassdoc::m_optionDebug = true;
bool Vjassdoc::m_optionPrivate = false;
bool Vjassdoc::m_optionFunctions = false;
bool Vjassdoc::m_optionTextmacros = false;
bool Vjassdoc::m_optionHtml = false;
bool Vjassdoc::m_optionPages = false;
bool Vjassdoc::m_optionSpecialpages = false;
bool Vjassdoc::m_optionSyntax = false;
std::string Vjassdoc::m_optionCompile = "";
std::string Vjassdoc::m_optionDatabase = "";
bool Vjassdoc::m_optionVerbose = false;
bool Vjassdoc::m_optionTime = false;
bool Vjassdoc::m_optionAlphabetical = false;
bool Vjassdoc::m_optionParseObjectsOfList[Parser::MaxLists];
std::string Vjassdoc::m_optionTitle = "";
std::string Vjassdoc::m_optionDir = "";
std::list<std::string> Vjassdoc::m_optionImport = std::list<std::string>();
std::list<std::string> Vjassdoc::m_optionFiles = std::list<std::string>();
std::list<std::string> Vjassdoc::m_optionDatabases = std::list<std::string>();

void Vjassdoc::configure(bool optionJass, bool optionDebug, bool optionPrivate, bool optionTextmacros, bool optionFunctions, bool optionHtml, bool optionPages, bool optionSpecialpages, bool optionSyntax, const std::string &optionCompile, const std::string &optionDatabase, bool optionVerbose, bool optionTime, bool optionAlphabetical, bool optionParseObjectsOfList[Parser::MaxLists], const std::string &optionTitle, const std::string &optionDir, std::list<std::string> optionImport, std::list<std::string> optionFiles, std::list<std::string> optionDatabases)
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
	Vjassdoc::m_duration = 0.0;
	Vjassdoc::m_cpuDuration = 0.0;

	timeval timeValue;
	long beginMicroseconds, endMicroseconds;
	clock_t beginCpu, endCpu;

	if (Vjassdoc::optionTime())
	{
		gettimeofday(&timeValue, 0);
		beginMicroseconds = long(1e6) * timeValue.tv_sec + timeValue.tv_usec;
		beginCpu = clock();
	}

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
	{
		gettimeofday(&timeValue, 0);
		endMicroseconds = long(1e6) * timeValue.tv_sec + timeValue.tv_usec;
		endCpu = clock();
		Vjassdoc::m_duration = (double)(endMicroseconds - beginMicroseconds) / double(1e6);
		Vjassdoc::m_cpuDuration = (double)(endCpu - beginCpu) / double(CLOCKS_PER_SEC);
		printf(_("Duration:\n%f seconds\nCPU duration:\n%f seconds\n"), Vjassdoc::m_duration, Vjassdoc::m_cpuDuration);
	}

	printf(_("Finished (%d lines, %d files).\n"), Vjassdoc::m_lines, Vjassdoc::m_files);
}

void Vjassdoc::clear()
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
