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
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include <list>

#include <getopt.h>

#include <boost/scoped_ptr.hpp>

#include "internationalisation.hpp"
#include "vjassdoc.hpp"
#include "parser.hpp"

using namespace vjassdoc;

int main(int argc, char *argv[])
{
	// Set the current locale.
	setlocale(LC_ALL, "");
	// Set the text message domain.
	bindtextdomain("vjassdoc", LOCALE_DIR);
	textdomain("vjassdoc");

	static struct option options[] =
	{
		{"version",                 no_argument,             0, 'V'},
		{"help",                    no_argument,             0, 'h'},
		{"jass",                    no_argument,             0, 'j'},
		{"debug",                   no_argument,             0, 'd'},
		{"private",                 no_argument,             0, 'p'},
		{"textmacros",              no_argument,             0, 'm'},
		{"functions",               no_argument,             0, 'f'},
		{"nocomments",              no_argument,             0, 0},
		{"nokeywords",              no_argument,             0, 0},
		{"notextmacros",            no_argument,             0, 0},
		{"notextmacroinstances",    no_argument,             0, 0},
		{"notypes",                 no_argument,             0, 0},
		{"nolocals",                no_argument,             0, 0},
		{"noglobals",               no_argument,             0, 0},
		{"nomembers",               no_argument,             0, 0},
		{"noparameters",            no_argument,             0, 0},
		{"nofunctioninterfaces",    no_argument,             0, 0},
		{"nofunctions",             no_argument,             0, 0},
		{"nomethods",               no_argument,             0, 0},
		{"nocalls",                 no_argument,             0, 0},
		{"noimplementations",       no_argument,             0, 0},
		{"nohooks",                 no_argument,             0, 0},
		{"nointerfaces",            no_argument,             0, 0},
		{"nostructs",               no_argument,             0, 0},
		{"nomodules",               no_argument,             0, 0},
		{"noscopes",                no_argument,             0, 0},
		{"nolibraries",             no_argument,             0, 0},
		{"nosourcefiles",           no_argument,             0, 0},
		{"nodoccomments",           no_argument,             0, 0},
		{"html",                    no_argument,             0, 'l'},
		{"pages",                   no_argument,             0, 'g'},
		{"specialpages",            no_argument,             0, 's'},
		{"syntax",                  no_argument,             0, 'x'},
		{"compile",                 required_argument,       0, 'C'},
#ifdef SQLITE
		{"database",                required_argument,       0, 'L'},
#endif
		{"verbose",                 no_argument,             0, 'v'},
		{"time",                    no_argument,             0, 't'},
		{"alphabetical",            no_argument,             0, 'a'},
		{"title",                   required_argument,       0, 'T'},
		{"importdirs",              required_argument,       0, 'I'},
		{"dir",                     required_argument,       0, 'D'},
#ifdef SQLITE
		{"databases",               required_argument,       0, 'B'},
#endif
		{0, 0, 0, 0}
	};

	bool jass = false;
	bool debug = false;
	bool parsePrivate = false;
	bool textmacros = false;
	bool functions = false;
	bool html = false;
	bool pages = false;
	bool specialPages = false;
	bool syntax = false;
	std::string compileFilePath;
	std::string databaseFilePath;
	bool verbose = false;
	bool time = false;
	bool alphabetical = false;
	static const char* objectListOption[Parser::MaxLists] =
	{
		"nocomments",
		"nokeywords",
		"nokeys",
		"notextmacros",
		"notextmacroinstances",
		"notypes",
		"nolocals",
		"noglobals",
		"nomembers",
		"noparameters",
		"nofunctioninterfaces",
		"nofunctions",
		"nomethods",
		"nocalls",
		"noimplementations",
		"nohooks",
		"nointerfaces",
		"nostructs",
		"nomodules",
		"noscopes",
		"nolibraries",
		"nosourcefiles",
		"nodoccomments"
	};
	bool parseObjectsOfList[Parser::MaxLists] =
	{
		true, //comments
		true, //keywords
		true, //keys
		true, //text macros
		true, //text macro instances
		true, //types
		true, //locals
		true, //globals
		true, //members
		true, //parameters
		true, //function interfaces
		true, //functions
		true, //methods
		true, //calls
		true, //implementations
		true, //hooks
		true, //interfaces
		true, //structs
		true, //modules
		true, //scopes
		true, //libraries
		true, //source files
		true //doc comments
	};
	std::string title;
	std::string dir;
	std::list<std::string> importDirs;
	std::list<std::string> databases;
	std::list<std::string> filePaths;
	int optionShortcut;

	while (true)
	{
		int optionIndex = 0;
		optionShortcut = getopt_long(argc, argv, "VhjdpmflgsxC:L:vtaT:I:D:B:", options, &optionIndex);

		if (optionShortcut == -1)
			break;

		switch (optionShortcut)
		{
			case 'V':
			{
				printf("vjassdoc %s.\n", Vjassdoc::version);
				std::cout << _(
				"Copyright © 2008, 2009 Tamino Dauth\n"
				"License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>\n"
				"This is free software: you are free to change and redistribute it.\n"
				"There is NO WARRANTY, to the extent permitted by law."
				) << std::endl;

				return EXIT_SUCCESS;
			}

			case 'h':
			{
				printf("vjassdoc %s.\n\n", Vjassdoc::version);
				std::cout <<
				_("Usage: vjassdoc [Options] [Code files]\n\n") <<
				_("Options:\n") <<
				_("\t-V --version                Shows the current version of vjassdoc.\n") <<
				_("\t-h --help                   Shows this text.\n") <<
				_("\t-j --jass                   vJass code will be ignored.\n") <<
				_("\t-d --debug                  Lines starting with the vJass keyword \'debug\' won't be ignored.\n") <<
				_("\t-p --private                Private objects will be parsed.\n") <<
				_("\t-m --textmacros             Code between text macro statements will be parsed (Warning: There can be many parsing errors!).\n") <<
				_("\t-f --functions              Code between function/method statements will be parsed.\n") <<
				_("\t--no<object type name>      Objects of type <object type name> won't added to the output files.\n") <<
				_("\t                            The following object type names are available:\n") <<
				_("\t                            comments\n") <<
				_("\t                            keywords\n") <<
				_("\t                            textmacros\n") <<
				_("\t                            textmacroinstances\n") <<
				_("\t                            types\n") <<
				_("\t                            locals\n") <<
				_("\t                            globals\n") <<
				_("\t                            members\n") <<
				_("\t                            parameters\n") <<
				_("\t                            functioninterfaces\n") <<
				_("\t                            functions\n") <<
				_("\t                            methods\n") <<
				_("\t                            calls\n") <<
				_("\t                            implementations\n") <<
				_("\t                            hooks\n") <<
				_("\t                            interfaces\n") <<
				_("\t                            structs\n") <<
				_("\t                            modules\n") <<
				_("\t                            scopes\n") <<
				_("\t                            libraries\n") <<
				_("\t                            sourcefiles\n") <<
				_("\t                            doccomments\n") <<
				_("\t-l --html                   Creates a simple HTML API documentation.\n") <<
				_("\t-g --pages                  Creates an HTML file for each parsed object.\n") <<
				_("\t-s --specialpages           Creates additional HTML files containing more information about all parsed objects.\n") <<
				_("\t-x --syntax                 Checks syntax. Not implemented yet!\n") <<
				_("\t-C --compile <arg>          Uses file <arg> to create a map Jass script.\n") <<
#ifdef SQLITE
				_("\t-L --database <arg>         Uses file <arg> to create an SQLite3 database which contains all parsed objects.\n") <<
#endif
				_("\t-v --verbose                Shows more information about the process.\n") <<
				_("\t-t --time                   Detects the elapsed time and shows it at the end of the process.\n") <<
				_("\t-a --alphabetical           All objects will be aranged in alphabetical order.\n") <<
				_("\t-T --title <arg>            <arg> has to be replaced by the title which is used for the API documentation.\n") <<
				_("\t-I --importdirs <args>      <args> has to be replaced by one or more import directories (Used for the //! import macro in vJass).\n") <<
				_("\t-D --dir <arg>              <arg> has to be replaced by the output directory path.\n") <<
#ifdef SQLITE
				_("\t-B --databases <args>       <args> has to be replaced by the SQLite3 databases which should be added to the output.\n") <<
#endif
				std::endl <<
				_("Several arguments (indicated by \"<args>\") have to be separated by using the : character.\n") <<
				_("\nReport bugs to tamino@cdauth.de or on https://wc3lib.org") <<
				std::endl;

				return EXIT_SUCCESS;
			}

			case 'j':
				jass = true;

				break;

			case 'd':
				debug = true;

				break;

			case 'p':
				parsePrivate = true;

				break;

			case 'm':
				textmacros = true;

				break;

			case 'f':
				functions = true;

				break;

			case 'l':
				html = true;

				break;

			case 'g':
				pages = true;

				break;

			case 's':
				specialPages = true;

				break;

			case 'x':
				syntax = true;

				break;

			case 'C':
				compileFilePath = optarg;

				break;
#ifdef SQLITE
			case 'L':
				databaseFilePath = optarg;

				break;
#endif

			case 'v':
				verbose = true;

				break;

			case 't':
				time = true;

				break;

			case 'a':
				alphabetical = true;

				break;

			case 'T':
				title = optarg;

				break;

			case 'I':
				for (char *path = strtok(optarg, ":"); path != 0;  path = strtok(0, ":"))
				{
					for (std::list<std::string>::const_iterator iterator = importDirs.begin(); iterator != importDirs.end(); ++iterator)
					{
						if (*iterator == path)
						{
							fprintf(stderr, _("Import directory path \"%s\" has already been added to list.\n"), path);

							continue;
						}
					}

					importDirs.push_back(path);
				}

				break;

			case 'D':
				dir = optarg;

				break;

#ifdef SQLITE
			case 'B':
				for (char *path = strtok(optarg, ":"); path != 0;  path = strtok(0, ":"))
				{
					for (std::list<std::string>::const_iterator iterator = databases.begin(); iterator != databases.end(); ++iterator)
					{
						if (*iterator == path)
						{
							fprintf(stderr, _("Database \"%s\" has already been added to list.\n"), path);

							continue;
						}
					}

					databases.push_back(path);
				}

				break;
#endif

			default:
				for (int j = 0; j < Parser::MaxLists; ++j)
				{
					if (strcmp(options[optionIndex].name, objectListOption[j]) == 0)
					{
						if (parseObjectsOfList[j])
							parseObjectsOfList[j] = false;
						else
							fprintf(stderr, _("Objects of list %d already won't be parsed.\n"), j);

						break;
					}
				}

				break;
		}
	}


	if (optind < argc)
	{
		for ( ; optind < argc; ++optind)
		{
			for (std::list<std::string>::const_iterator iterator = filePaths.begin(); iterator != filePaths.end(); ++iterator)
			{
				if (*iterator == argv[optind])
				{
						fprintf(stderr, _("File path \"%s\" has already been added to list.\n"), argv[optind]);

						continue;
				}
			}

			filePaths.push_back(argv[optind]);
		}

	}
	else
	{
		std::cerr << _("Missing file arguments.") << std::endl;

		return EXIT_FAILURE;
	}

	if (pages && !html)
		std::cerr << _("Warning: Although there won't be created any HTML files you have used the --pages option.") << std::endl;

	if (title.empty())
		title = _("vJass API Documentation");

	boost::scoped_ptr<Vjassdoc> program(new Vjassdoc(jass, debug, parsePrivate, textmacros, functions, html, pages, specialPages, syntax, compileFilePath, databaseFilePath, verbose, time, alphabetical, parseObjectsOfList, title, dir, importDirs, filePaths, databases));

#ifdef SQLITE
	program->initClasses();
#endif

	program->run();

	return EXIT_SUCCESS;
}
