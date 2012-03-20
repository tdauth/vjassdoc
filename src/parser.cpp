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

#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>

#include <boost/scoped_ptr.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/cast.hpp>
#include <boost/range.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>

#ifdef SQLITE
#include <sqlite3.h>
#endif
#include <sstream>

#include "parser.hpp"
#include "internationalisation.hpp"
#include "objects.hpp"
#include "vjassdoc.hpp"
#include "file.hpp"
#include "syntaxerror.hpp"
#include "utilities.hpp"

namespace vjassdoc
{

const char *Parser::title[Parser::MaxLists] =
{
	_("Comments"),
	_("Keywords"),
	_("Keys"),
	_("Text Macros"),
	_("Text Macro Instances"),
	_("Types"),
	_("Locals"),
	_("Globals"),
	_("Members"),
	_("Parameters"),
	_("Function Interfaces"),
	_("Functions"),
	_("Methods"),
	_("Calls"),
	_("Implementations"),
	_("Hooks"),
	_("Interfaces"),
	_("Structs"),
	_("Modules"),
	_("Scopes"),
	_("Libraries"),
	_("Source Files"),
	_("Documentation Comments")
};

Parser::Parser(Vjassdoc *parent) : m_parent(parent),
		m_currentId(0),
		m_integerType(new Type(this, "integer", 0, 0, 0, "", "")),
		m_realType(new Type(this, "real", 0, 0, 0, "", "")),
		m_stringType(new Type(this, "string", 0, 0, 0, "", "")),
		m_booleanType(new Type(this, "boolean", 0, 0, 0, "", "")),
		m_handleType(new Type(this, "handle", 0, 0, 0, "", "")),
		m_codeType(new Type(this, "code", 0, 0, 0, "", ""))
{
	//add default types
	if (parent->optionVerbose())
		std::cout << _("Adding default Jass types.") << std::endl;

	this->add(m_integerType);
	this->add(m_realType);
	this->add(m_stringType);
	this->add(m_booleanType);
	this->add(m_handleType);
	this->add(m_codeType);
}

//Default Jass types are in lists!
Parser::~Parser()
{
#ifdef SQLITE
	for (std::vector<struct Database*>::iterator iterator = this->m_databases.begin(); iterator != this->m_databases.end(); ++iterator)
		delete *iterator;

	this->m_databases.clear();
#endif
}

void Parser::createInheritanceListPage()
{
	std::ofstream fstream((parent()->optionDir() + dirSeparator + "inheritancelist.html").c_str());

	if (!fstream)
	{
		fstream.close();
		std::cerr << _("Was unable to create file \"inheritancelist.html\".") << std::endl;

		return;
	}

	createHtmlHeader(fstream, _("Inheritance List"));
	fstream
	<< "\t<body>\n"
	<< "\t\t<a href=\"index.html\">" << _("Return to start page") << "</a>\n"
	<< "\t\t<h1>" << _("Inheritance List") << "</h1>\n"
	<< "\t\t<ul>\n"
	<< "\t\t\t<li><a href=\"#Interfaces\">"	<< _("Interfaces") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Structs\">"	<< _("Structs") << "</a></li>\n"
	<< "\t\t</ul>\n"
	<< "\t\t<h2><a name=\"Interfaces\">" << _("Interfaces") << "</a></h2>\n"
	;

	if (!this->m_interfaces.empty())
	{
		fstream << "\t\t<ul>\n";

		BOOST_FOREACH(ObjectList::reference ref, this->m_interfaces)
		{
			fstream << "\t\t\t<li>" << ref.second->pageLink() << '\n';
			this->getStructInheritanceList(boost::polymorphic_downcast<Interface*>(ref.second), "\t\t\t\t", fstream);
			fstream << "\t\t\t</li>\n";
		}

		fstream << "\t\t</ul>\n";
	}
	else
		fstream << "\t\t<p>-</p>\n";

	fstream
	<< "\t\t<h2><a name=\"Structs\">" << _("Structs") << "</a></h2>\n";

	if (!this->m_structs.empty())
	{
		fstream << "\t\t<ul>\n";

		BOOST_FOREACH(ObjectList::reference ref, this->m_structs)
		{
			if (boost::polymorphic_downcast<Struct*>(ref.second)->extension() == 0)
			{
				fstream << "\t\t\t<li>" << ref.second->pageLink() << '\n';
				this->getStructInheritanceList(boost::polymorphic_downcast<Struct*>(ref.second), "\t\t\t\t", fstream);
				fstream << "\t\t\t</li>\n";
			}
		}

		fstream << "\t\t</ul>\n";
	}
	else
		fstream << "\t\t<p>-</p>\n";

	fstream
	<< "\t</body>\n"
	<< "</html>\n"
	;

	fstream.close();
}

void Parser::createRequirementListPage()
{
	std::ofstream fstream((parent()->optionDir() + dirSeparator + "requirementlist.html").c_str());

	if (!fstream)
	{
		fstream.close();
		std::cerr << _("Was unable to create file \"requirementlist.html\".") << std::endl;

		return;
	}

	createHtmlHeader(fstream, _("Requirement List"));
	fstream
	<< "\t<body>\n"
	<< "\t\t<a href=\"index.html\">" << _("Return to start page") << "</a>\n"
	<< "\t\t<h1>" << _("Requirement List") << "</h1>\n"
	<< "\t\t<ul>\n"
	;

	BOOST_FOREACH(ObjectList::reference ref, this->m_libraries)
	{
		Library *library = boost::polymorphic_downcast<Library*>(ref.second);

		//requirement has to be 0
		if (library->requirements().get() != 0 && !library->requirements()->empty())
			continue;

		fstream << "\t\t\t<li>" << library->pageLink() << '\n';
		this->getLibraryRequirementList(library, "\t\t\t\t", fstream);
		fstream << "\t\t\t</li>\n";
	}

	fstream
	<< "\t\t</ul>\n"
	<< "\t</body>\n"
	<< "</html>\n"
	;

	fstream.close();
}

void Parser::showSyntaxErrors()
{
	/// @todo compare all object identifiers with Jass and vJass keywords.
	std::cout << boost::format(_("%1% syntax errors.")) % this->m_syntaxErrors.size() << std::endl;
	this->m_syntaxErrors.sort(SyntaxError::Comparator);

	BOOST_FOREACH(SyntaxError *error, this->m_syntaxErrors)
		std::cout << boost::format(_("File %1%, line %2%: %3%")) % error->sourceFile()->identifier() % error->line() % error->message() << std::endl;
}

void Parser::sortAlphabetically()
{
	if (parent()->optionVerbose())
		std::cout << _("Sorting alphabetically.") << std::endl;

	for (int i = 0; i < Parser::MaxLists; ++i)
	{
		if (!parent()->optionParseObjectsOfList(Parser::List(i)))
			continue;

		// TODO FIXME
		//this->getList(Parser::List(i)).sort(Object::AlphabeticalComparator());
	}
}

void Parser::createHtmlFiles()
{
	if (parent()->optionVerbose())
		std::cout << _("Creating HTML files.") << std::endl;

	std::ofstream fstream((parent()->optionDir() + dirSeparator + "index.html").c_str());

	if (!fstream)
	{
		fstream.close();
		std::cerr << _("Was unable to create file \"index.html\".") << std::endl;

		return;
	}

	std::string title = parent()->optionTitle();
	createHtmlHeader(fstream, title);
	fstream
	<< "\t<body>\n"
	<< "\t\t<h1>" << title << "</h1>\n";
	char buffer[50];
	sprintf(buffer, _("Generated by vjassdoc %s."), Vjassdoc::version);
	fstream
	<< "\t\t<p>" << buffer << "</p>\n"
	<< "\t\t<ul>\n"
	<< std::endl;

	for (int i = 0; i < Parser::MaxLists; ++i)
	{
		if (!parent()->optionParseObjectsOfList(Parser::List(i)) || this->getList(Parser::List(i)).empty())
			continue;

		fstream << "\t\t\t<li><a href=\"#" << Parser::title[i] << "\">" << Parser::title[i] << " (" << this->getList(Parser::List(i)).size() << ")</a></li>" << std::endl;
	}

	if (parent()->optionSpecialpages())
	{
		fstream
		<< "\t\t\t<li>" << "<a href=\"inheritancelist.html\">" << _("Inheritance List") << "</a></li>\n"
		<< "\t\t\t<li>" << "<a href=\"requirementlist.html\">" << _("Requirement List") << "</a></li>\n"
		<< "\t\t\t<li>" << "<a href=\"#Undocumented Objects\">" << _("Undocumented Objects") << "</a></li>\n"
		//<< "\t\t\t<li>" << "<a href=\"#Authors\">" << _("Authors") << "</a></li>\n"
		//<< "\t\t\t<li>" << "<a href=\"#Todos\">" << _("Todos") << "</a></li>\n"
		//<< "\t\t\t<li>" << "<a href=\"#States\">" << _("States") << "</a></li>\n"
		//<< "\t\t\t<li>" << "<a href=\"#Sources\">" << _("Sources") << "</a></li>\n"
		;
	}

	fstream << "\t\t</ul>\n";

	// link lists for all object types are created
	for (int i = 0; i < Parser::MaxLists; ++i)
	{
		if (!parent()->optionParseObjectsOfList(Parser::List(i)) || this->getList(Parser::List(i)).empty())
			continue;

		fstream
		<< "\t\t<h2><a name=\"" <<  Parser::title[i] << "\">" << Parser::title[i] << " (" << this->getList(Parser::List(i)).size() << ")</a></h2>" << std::endl;
		this->addObjectList(fstream, List(i));
	}

	if (parent()->optionSpecialpages())
	{
		this->createInheritanceListPage();
		//this->createRequirementListPage(); /// @todo Bugged

		fstream
		<< "\t\t<h2><a name=\"Undocumented Objects\">" << _("Undocumented Objects") << "</h2>\n"
		<< "\t\t<ul>\n"
		;

		for (int i = 0; i < Parser::MaxLists; ++i)
		{
			if (Parser::List(i) == Parser::SourceFiles || Parser::List(i) == Parser::DocComments || !parent()->optionParseObjectsOfList(Parser::List(i)))
				continue;

			ObjectList &list = this->getList(Parser::List(i));

			BOOST_FOREACH(ObjectList::reference ref, list)
			{
				if (ref.second->docComment() == 0)
					fstream << "\t\t\t<li>" << Object::objectPageLink(ref.second) << "</li>\n";
			}
		}

		fstream
		<< "\t\t</ul>\n"
		;
		/// @todo Implement these special pages:
/*
		<< "\t\t\t<li>" << "<a href=\"#Authors\">" << _("Authors") << "</a></li>\n"
		<< "\t\t\t<li>" << "<a href=\"#Todos\">" << _("Todos") << "</a></li>\n"
		<< "\t\t\t<li>" << "<a href=\"#States\">" << _("States") << "</a></li>\n"
		<< "\t\t\t<li>" << "<a href=\"#Sources\">" << _("Sources") << "</a></li>\n"
*/
	}

	fstream
	<< "\t</body>" << std::endl
	<< "</html>";
	fstream.close();

	//create pages
	if (parent()->optionPages())
	{
		for (int i = 0; i < Parser::MaxLists; ++i)
		{
			if (!parent()->optionParseObjectsOfList(Parser::List(i)))
				continue;

			ObjectList &list = this->getList(Parser::List(i));

			BOOST_FOREACH(ObjectList::reference ref, list)
			{
				std::ostringstream sstream;
				sstream << parent()->optionDir() << '/' << ref.second->id() << ".html";
				std::ofstream fout(sstream.str().c_str());

				if (!fout)
				{
					std::cerr << boost::format(_("Error while creating file \"%1%\".")) % sstream.str() << std::endl;

					return;
				}

				fout
				<< "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
				<< "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n"
				<< "\t\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
				<< "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"" << _("en") << "\">\n"
				<< "<html>\n"
				<< "\t<head>\n"
				<< "\t\t<meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\"/>\n"
				<< "\t\t<title>" << ref.second->identifier() << "</title>\n"
				<< "\t\t<link rel=\"stylesheet\" href=\"style.css\" type=\"text/css\"/>\n"
				<< "\t</head>\n"
				<< "\t<body>\n"
				<< "\t\t<a href=\"index.html\">" << _("Return to start page") << "</a>\n"
				<< "\t\t<h1>" << ref.second->identifier() << "</h1>\n"
				<< "\t\t<ul>\n"
				;
				ref.second->pageNavigation(fout);
				fout << "\t\t</ul>\n";
				ref.second->page(fout);
				fout
				<< "\t</body>\n"
				<< "</html>\n";
				fout.close();
			}
		}
	}
}

void Parser::parse(const std::list<std::string> &filePaths)
{
	BOOST_FOREACH(const std::string &path, filePaths)
	{
		std::string identifier = path;
		cutFilePath(identifier);
		class SourceFile *sourceFile = new SourceFile(this, identifier, path);
		this->add(sourceFile);
		this->m_currentSourceFile = sourceFile;
		this->parseFile(this->m_currentSourceFile->path());
	}

	if (parent()->optionVerbose())
		std::cout << _("Initializing parsed objects.") << std::endl;

	//objects should be initialized before using them
	for (int i = 0; i < Parser::MaxLists; ++i)
	{
		if (!parent()->optionParseObjectsOfList(Parser::List(i)))
			continue;

		ObjectList &list = this->getList(Parser::List(i));

		BOOST_FOREACH(ObjectList::reference ref, list)
		{
#ifdef SQLITE
/*
			bool result = false;

			//if is contained by any database do not init!!!!
			for (std::vector<struct Database*>::iterator databaseIterator = this->databaseVector.end(); databaseIterator != this->databaseVector.end() && !result; ++databaseIterator)
			{
				for (std::list<class Object*>::iterator objectIterator = (*databaseIterator)->objectList.begin(); objectIterator != (*databaseIterator)->objectList.end() && !result; ++objectIterator)
				{
					if (*objectIterator == *iterator)
						result = true;
				}
			}

			if (result)
			{
				std::cout << "Do not initializer object. It was added by database." << std::endl;
				continue;
			}
*/
#endif

			ref.second->init();
		}
	}
}

namespace
{

void addToList(Parser::ObjectList &list, Object *object)
{
	std::string identifier = object->identifier();
	list.insert(identifier, object);
}

}

void Parser::add(class Comment *comment)
{
	addToList(this->m_comments, comment);
}

void Parser::add(class Key *key)
{
	addToList(this->m_keys, key);
}

void Parser::add(class Keyword *keyword)
{
	addToList(this->m_keywords, keyword);
}

void Parser::add(class TextMacro *textMacro)
{
	addToList(this->m_textMacros, textMacro);
}

void Parser::add(class TextMacroInstance *textMacroInstance)
{
	addToList(this->m_textMacroInstances, textMacroInstance);
}

void Parser::add(class Type *type)
{
	addToList(this->m_types, type);
}

void Parser::add(class Local *local)
{
	addToList(this->m_locals, local);
}

void Parser::add(class Global *global)
{
	addToList(this->m_globals, global);
}

void Parser::add(class Member *member)
{
	addToList(this->m_members, member);
}

void Parser::add(class Parameter *parameter)
{
	addToList(this->m_parameters, parameter);
}

void Parser::add(class FunctionInterface *functionInterface)
{
	addToList(this->m_functionInterfaces, functionInterface);
}

void Parser::add(class Function *function)
{
	addToList(this->m_functions, function);
}

void Parser::add(class Method *method)
{
	addToList(this->m_methods, method);
}

void Parser::add(class Call *call)
{
	addToList(this->m_calls, call);
}

void Parser::add(class Implementation *implementation)
{
	addToList(this->m_implementations, implementation);
}

void Parser::add(class Hook *hook)
{
	addToList(this->m_hooks, hook);
}

void Parser::add(class Interface *interface)
{
	addToList(this->m_interfaces, interface);
}

void Parser::add(class Struct *usedStruct)
{
	addToList(this->m_structs, usedStruct);
}

void Parser::add(class Module *module)
{
	addToList(this->m_modules, module);
}

void Parser::add(class Scope *scope)
{
	addToList(this->m_scopes, scope);
}

void Parser::add(class Library *library)
{
	addToList(this->m_libraries, library);
}

void Parser::add(class SourceFile *sourceFile)
{
	addToList(this->m_sourceFiles, sourceFile);
}

void Parser::add(class DocComment *docComment)
{
	addToList(this->m_docComments, docComment);
}

#ifdef SQLITE
void Parser::createDatabase(const std::string &filePath)
{
	if (parent()->optionVerbose())
		printf(_("Creating database.\nUsing SQLite version %s.\n"), SQLITE_VERSION);

	if (fileExists(filePath))
	{
		printf(_("Database \"%s\" does already exist. Do you want to replace it by the newer one?\n"), filePath.c_str());
		std::cout << _("Answer possiblities: y, yes, n, no.\n");

		std::string answer;

		do
		{
			std::cin >> answer;

			if (answer == _("y") || answer == _("yes"))
			{
				if (remove(filePath.c_str()) != 0)
				{
					std::cout << _("Was unable to replace old database.") << std::endl;

					return;
				}

				break;
			}
			else if (answer == _("n") || answer == _("no"))
			{
				std::cout << _("Canceled database creation.") << std::endl;

				return;
			}
			else
				std::cout << _("Unknown answer.") << std::endl;
		}
		while (true);
	}

	sqlite3 *database;
	int state = sqlite3_open(filePath.c_str(), &database);

	if (state == SQLITE_OK)
	{
		char *message = 0;
		state = sqlite3_exec(database, "BEGIN TRANSACTION", 0, 0, &message);

		if (state != SQLITE_OK)
		{
			fprintf(stderr, _("Was unable to begin transaction.\nState: %d.\nMessage: %s\n"), state, message);
			sqlite3_free(message);
		}

		for (int i = 0; i < Parser::MaxLists; ++i)
		{
			if (!parent()->optionParseObjectsOfList(Parser::List(i)) || this->getList(Parser::List(i)).empty())
				continue;

			state = sqlite3_exec(database, this->getTableCreationStatement(Parser::List(i)).c_str(), 0, 0, &message);

			if (state != SQLITE_OK)
			{
				/// @todo test output
				std::cout << "Table list " << i << std::endl;
				std::cout << "Table creation statement: " << this->getTableCreationStatement(Parser::List(i)).c_str() << std::endl;

				fprintf(stderr, _("Was unable to create table \"%s\" from list %d.\nState: %d.\nMessage: %s\n"), Parser::getTableName(Parser::List(i)).c_str(), i, state, message);
				sqlite3_free(message);
			}

			std::list<class Object*> &list = this->getList(Parser::List(i));

			for (std::list<class Object*>::iterator iterator = list.begin(); iterator != list.end(); ++iterator)
			{
				std::ostringstream sstream;
				sstream << "INSERT INTO " << Parser::getTableName(Parser::List(i)) << " (Id) VALUES (" << (*iterator)->id() << ')';

				state = sqlite3_exec(database, sstream.str().c_str(), 0, 0, &message);

				//std::cerr << "Execution " << sstream.str().c_str() << std::endl;

				if (state != SQLITE_OK)
				{
					fprintf(stderr, _("Was unable to insert id of list %d into table \"%s\".\nState: %d.\nMessage: %s\n"), i, Parser::getTableName(Parser::List(i)).c_str(), state, message);
					sqlite3_free(message);
				}

				sstream.str("");
				sstream.clear();

				//std::cout << "Before update of object " << (*iterator)->identifier() << std::endl;
				sstream << "UPDATE " << Parser::getTableName(Parser::List(i)).c_str() << " SET " << (*iterator)->sqlStatement() << " WHERE Id=" << (*iterator)->id();
				//std::cout << "After update" << std::endl;
				//std::cout << "Execution command: " << sstream.str().c_str() << std::endl; //NOTE debug


				state = sqlite3_exec(
						database, /* An open database */
						sstream.str().c_str(), /* SQL to be evaluated */ //FIXME
						0, //int (*callback)(void*,int,char**,char**), /* Callback function */
						0, //void *, /* 1st argument to callback */
						&message /* Error msg written here */
				);

				if (state != SQLITE_OK)
				{
					fprintf(stderr, _("Was unable to run execution \"%s\" of table \"%s\" from list %d.\nState: %d.\nMessage: %s\n"), sstream.str().c_str(), Parser::getTableName(Parser::List(i)).c_str(), i, state, message);
					sqlite3_free(message);
				}
			}
		}

		state = sqlite3_exec(database, "COMMIT", 0, 0, &message);

		if (state != SQLITE_OK)
		{
			fprintf(stderr, _("Was unable to commit.\nState: %d.\nMessage: %s\n"), state, message);
			sqlite3_free(message);
		}
	}
	else
		fprintf(stderr, _("Was unable to create database. State %d.\n"), state);

	sqlite3_close(database);
}

int Parser::addDatabase(const std::string &filePath)
{
	if (parent()->optionVerbose())
		printf(_("Loading database %s.\n"), filePath.c_str());

	int result = -1;
	sqlite3 *database;
	int state = sqlite3_open_v2(filePath.c_str(), &database, SQLITE_OPEN_READONLY, NULL);

	if (state == SQLITE_OK)
	{
		Parser::Database *databaseStruct = new Parser::Database;
		result = this->m_databases.size();
		this->m_databases.push_back(databaseStruct);

		for (int i = 0; i < Parser::MaxLists; ++i)
		{
			if (!parent()->optionParseObjectsOfList(Parser::List(i)))
				continue;

			std::cout << "List " << i << std::endl;
			std::ostringstream sstream;
			sstream << "SELECT * FROM " << Parser::getTableName(Parser::List(i));
			sqlite3_stmt *statement;
			std::cout << "Before statement " << sstream.str() << std::endl;
			state = sqlite3_prepare_v2(
				database,            /* Database handle */
				sstream.str().c_str(),       /* SQL statement, UTF-8 encoded */
				-1,              /* Maximum length of zSql in bytes. */
				&statement,  /* OUT: Statement handle */
				NULL);     /* OUT: Pointer to unused portion of zSql */

			std::cout << "After statement " << sstream.str() << std::endl;

			if (state != SQLITE_OK)
				fprintf(stderr, _("Was unable to prepare SQL statement of table %s.\nState %d.\nMessage: \"%s\".\n"),  Parser::getTableName(Parser::List(i)).c_str(), state, sqlite3_errmsg(database));
			else
			{
				if (statement == NULL)
				{
					fprintf(stderr, _("SQL statement of table %s is NULL.\n"), Parser::getTableName(Parser::List(i)).c_str());
				}
				else
				{
					int row = 0;

					for (state = sqlite3_step(statement); state == SQLITE_ROW; state = sqlite3_step(statement), ++row)
					{
						int columns = sqlite3_data_count(statement);
						std::vector<Object::VectorDataType> columnVector;

						for (int column = 0; column < columns; ++column)
						{
							const unsigned char *data = 0;

							if (sqlite3_column_type(statement, column) == SQLITE_NULL)
							{
								fprintf(stderr, _("SQL database entry of table %s in row %d and column %d is NULL.\n"),  Parser::getTableName(Parser::List(i)).c_str(), row, column);
							}
							else
								data = sqlite3_column_text(statement, column);

							columnVector.push_back(data);
							std::cout << "Vector data " << columnVector[column] << std::endl;
						}

						databaseStruct->listList.push_back(Parser::List(i));
						std::cout << "List " << i << std::endl;
						class Object *object = Parser::createObjectByVector(columnVector, Parser::List(i)); /// @todo Don't use the @class Object constructor. Use the specific list class constructor (virtual methods).
						std::cout << "Created object with identifier " << object->identifier() << std::endl;
						databaseStruct->objectList.push_back(object);
					}

					state = sqlite3_finalize(statement);

					if (state != SQLITE_OK)
						fprintf(stderr, _("Was unable to finalize prepared SQL statement of table %s.\nState %d.\n"),  Parser::getTableName(Parser::List(i)).c_str(), state);
				}
			}
		}

		if (parent()->optionVerbose())
		{
			printf(_("Got %d database objects.\n"), databaseStruct->objectList.size());
			std::cout << _("Intializing database objects.") << std::endl;
		}

		/// @todo Initialize all database objects by initVector.
		std::list<class Object*>::iterator objectIterator = databaseStruct->objectList.begin();

		while (objectIterator != databaseStruct->objectList.end())
		{
			(*objectIterator)->initByVector();
			++objectIterator;
		}

		if (parent()->optionVerbose())
			std::cout << _("Adding database objects.") << std::endl;

		/// @todo Add database objects into usual lists and increase ids.
		std::list<enum List>::iterator listIterator = databaseStruct->listList.begin();
		objectIterator = databaseStruct->objectList.begin();

		while (listIterator != databaseStruct->listList.end())
		{
			this->getList(*listIterator).push_back(*objectIterator);
			(*objectIterator)->setId((*objectIterator)->id() + Object::maxIds());
			++listIterator;
			++objectIterator;
		}

		Object::setMaxIds(Object::maxIds() + databaseStruct->listList.size());

		/// @todo Usual initialization if there are identifier properties without corresponding object ids?
	}
	else
		fprintf(stderr, _("Was unable to open database \"%s\".\nState %d.\n"), filePath.c_str(), state);

	sqlite3_close(database);

	return result;
}

void Parser::removeDatabase(const int &index)
{
	/*
	if (index >= this->databaseVector.size())
	{
		fprintf(stderr, _("Invalid index in removeDatabase(): %d."), index);
		return;
	}
	*/

	if (parent()->optionVerbose())
		printf(_("Removing database %d.\n"), index);

	class Database *database = this->m_databases[index];
	std::list<enum List>::iterator iterator0 = database->listList.begin();
	std::list<class Object*>::iterator iterator1 = database->objectList.begin();

	while (iterator0 != database->listList.end())
	{
		this->getList(*iterator0).remove(*iterator1);
		delete *iterator1;
		++iterator0;
		++iterator1;
	}

	database->listList.clear();
	database->objectList.clear();
	this->m_databases.erase(this->m_databases.begin() + index - 1);
	delete database;
}


class Object* Parser::searchObjectInLastDatabase(const Object::IdType &id)
{
	if (this->m_databases.empty())
		return 0;

	std::list<class Object*> objectList = this->m_databases.back()->objectList;

	for (std::list<class Object*>::iterator iterator = objectList.begin(); iterator != objectList.end(); ++iterator)
	{
		if ((*iterator)->id() == id)
			return *iterator;
	}

	return 0;
}
#endif

class Object* Parser::searchObjectInList(const std::string &identifier, List list, SearchMode searchMode, const class Object *object)
{
	if (!parent()->optionParseObjectsOfList(list))
		return 0;

	ObjectList &objectList = this->getList(list);


	return Parser::searchObjectInCustomList(objectList, identifier, searchMode, object);
}

Object* Parser::searchObjectInList(const std::string &identifier, Parser::List list, const Object *object)
{
	return searchObjectInList(identifier, list, Unspecified, object);
}

std::list<class Object*> Parser::autoCompletion(const std::string &line, std::size_t &index)
{
	std::string token = getToken(line, index);

	if (token.empty())
		return std::list<class Object*>();

	if (token == "private" || token == "public" || token == "//!")
		token = getToken(line, index);

	enum Keyword
	{
		None,
		Extension,
		Requirement,
		Delegate,
		Hook,
		ReturnType,
		Call,
		ReturnValue
	};

	Keyword keyword = None;
	std::list<enum Parser::List> lists;

	if (token == "extends")
	{
		lists.push_back(Parser::Structs);
		lists.push_back(Parser::Interfaces);
		keyword = Extension;
	}
	else if (token == "requires" || token == "needs")
		lists.push_back(Parser::Libraries);
	else if (token == "delegate")
		lists.push_back(Parser::Structs);
	else if (token == "implement")
		lists.push_back(Parser::Modules);
	else if (token == "hook")
	{
		lists.push_back(Parser::Functions);
		lists.push_back(Parser::Methods); //statics
	}
	else if (token == "returns")
	{
		lists.push_back(Parser::Types);
		lists.push_back(Parser::FunctionInterfaces);
		lists.push_back(Parser::Interfaces);
		lists.push_back(Parser::Structs);
	}
	else if (token == "call")
	{
		lists.push_back(Parser::FunctionInterfaces);
		lists.push_back(Parser::Functions);
		lists.push_back(Parser::Methods);
		lists.push_back(Parser::Structs); //calling static methods
	}
	else if (token == "return")
	{
		lists.push_back(Parser::Types); // integer(10)
		lists.push_back(Parser::Globals); // bj_MAX_PLAYERS
		lists.push_back(Parser::Locals); // i
		lists.push_back(Parser::FunctionInterfaces); // FunctionInterface.bla
		lists.push_back(Parser::Interfaces); // Widget(1)
		lists.push_back(Parser::Structs); // Widget(1)
	}

	token = getToken(line, index);
	std::list<class Object*> results;

	BOOST_FOREACH(Parser::List list, lists)
	{
		ObjectList &objectList = this->getList(list);
		boost::iterator_range<ObjectList::iterator> range = objectList.equal_range(token);

		BOOST_FOREACH(ObjectList::reference ref, range)
			results.push_back(ref.second);
	}

	return results;
}

void Parser::parseFile(const std::string &path)
{
	boost::filesystem::path realPath = path;

	if (!boost::filesystem::exists(realPath))
	{
		bool found = false;

		BOOST_FOREACH(const std::string &ref, this->parent()->optionImport())
		{
			realPath = boost::filesystem::path(ref) / path;

			if (boost::filesystem::exists(realPath))
			{
				found = true;

				break;
			}
			else
				std::cout << "Not found " << realPath << std::endl;
		}

		if (!found)
		{
			std::cerr << boost::format(_("Was unable to open file \"%1%\".")) % path << std::endl;

			return;
		}
	}

	boost::filesystem::ifstream ifstream(realPath);
	boost::scoped_ptr<File> file(new File());
	const std::size_t lines = file->parse(this, ifstream);
	parent()->addLines(lines);
	parent()->addFile();
	std::cout << boost::format(_("Parsed file \"%1%\" successfully (number %2%, %3% lines).")) % path % parent()->files() % lines << std::endl;

	BOOST_FOREACH(const std::string &ref, file->imports())
		this->parseFile(ref);
}

Parser::ObjectList& Parser::getList(enum Parser::List list)
{
	assert(list >= Parser::Comments && list < Parser::MaxLists);

	switch (list)
	{
		case Parser::Comments:
			return this->m_comments;

		case Parser::Keywords:
			return this->m_keywords;

		case Parser::Keys:
			return this->m_keys;

		case Parser::TextMacros:
			return this->m_textMacros;

		case Parser::TextMacroInstances:
			return this->m_textMacroInstances;

		case Parser::Types:
			return this->m_types;

		case Parser::Locals:
			return this->m_locals;

		case Parser::Globals:
			return this->m_globals;

		case Parser::Members:
			return this->m_members;

		case Parser::Parameters:
			return this->m_parameters;

		case Parser::FunctionInterfaces:
			return this->m_functionInterfaces;

		case Parser::Functions:
			return this->m_functions;

		case Parser::Methods:
			return this->m_methods;

		case Parser::Calls:
			return this->m_calls;

		case Parser::Implementations:
			return this->m_implementations;

		case Parser::Hooks:
			return this->m_hooks;

		case Parser::Interfaces:
			return this->m_interfaces;

		case Parser::Structs:
			return this->m_structs;

		case Parser::Modules:
			return this->m_modules;

		case Parser::Scopes:
			return this->m_scopes;

		case Parser::Libraries:
			return this->m_libraries;

		case Parser::SourceFiles:
			return this->m_sourceFiles;

		case Parser::DocComments:
			return this->m_docComments;
	}
}

const Parser::ObjectList& Parser::getList(Parser::List list) const
{
	return const_cast<Parser*>(this)->getList(list);
}

std::ostream& Parser::addObjectList(std::ostream &output, Parser::List list)
{
	ObjectList &objectsList = this->getList(list);

	if (objectsList.empty())
	{
		output << "-\n";

		return output;
	}

	output << "\t\t<ul>\n";

	BOOST_FOREACH(ObjectList::reference ref, objectsList)
		output << "\t\t\t<li>" << Object::objectPageLink(ref.second) << "</li>\n";

	output << "\t\t</ul>\n";

	return output;
}

#ifdef SQLITE
std::string Parser::getTableName(const enum Parser::List &list)
{
	switch (list)
	{
		case Parser::Comments:
			return Comment::sqlTableName;

		case Parser::Keywords:
			return Keyword::sqlTableName;

		case Parser::Keys:
			return Key::sqlTableName;

		case Parser::TextMacros:
			return TextMacro::sqlTableName;

		case Parser::TextMacroInstances:
			return TextMacroInstance::sqlTableName;

		case Parser::Types:
			return Type::sqlTableName;

		case Parser::Locals:
			return Local::sqlTableName;

		case Parser::Globals:
			return Global::sqlTableName;

		case Parser::Members:
			return Member::sqlTableName;

		case Parser::Parameters:
			return Parameter::sqlTableName;

		case Parser::FunctionInterfaces:
			return FunctionInterface::sqlTableName;

		case Parser::Functions:
			return Function::sqlTableName;

		case Parser::Methods:
			return Method::sqlTableName;

		case Parser::Calls:
			return Call::sqlTableName;

		case Parser::Implementations:
			return Implementation::sqlTableName;

		case Parser::Hooks:
			return Hook::sqlTableName;

		case Parser::Interfaces:
			return Interface::sqlTableName;

		case Parser::Structs:
			return Struct::sqlTableName;

		case Parser::Modules:
			return Module::sqlTableName;

		case Parser::Scopes:
			return Scope::sqlTableName;

		case Parser::Libraries:
			return Library::sqlTableName;

		case Parser::SourceFiles:
			return SourceFile::sqlTableName;

		case Parser::DocComments:
			return DocComment::sqlTableName;
	}

	std::cerr << "Unknown list " << list << std::endl;

	return std::string();
}

unsigned int Parser::getTableColumns(const enum Parser::List &list)
{
	switch (list)
	{
		case Parser::Comments:
			return Comment::sqlColumns;

		case Parser::Keywords:
			return Keyword::sqlColumns;

		case Parser::Keys:
			return Key::sqlColumns;

		case Parser::TextMacros:
			return TextMacro::sqlColumns;

		case Parser::TextMacroInstances:
			return TextMacroInstance::sqlColumns;

		case Parser::Types:
			return Type::sqlColumns;

		case Parser::Locals:
			return Local::sqlColumns;

		case Parser::Globals:
			return Global::sqlColumns;

		case Parser::Members:
			return Member::sqlColumns;

		case Parser::Parameters:
			return Parameter::sqlColumns;

		case Parser::FunctionInterfaces:
			return FunctionInterface::sqlColumns;

		case Parser::Functions:
			return Function::sqlColumns;

		case Parser::Methods:
			return Method::sqlColumns;

		case Parser::Calls:
			return Method::sqlColumns;

		case Parser::Implementations:
			return Implementation::sqlColumns;

		case Parser::Hooks:
			return Hook::sqlColumns;

		case Parser::Interfaces:
			return Interface::sqlColumns;

		case Parser::Structs:
			return Struct::sqlColumns;

		case Parser::Modules:
			return Module::sqlColumns;

		case Parser::Scopes:
			return Scope::sqlColumns;

		case Parser::Libraries:
			return Library::sqlColumns;

		case Parser::SourceFiles:
			return SourceFile::sqlColumns;

		case Parser::DocComments:
			return DocComment::sqlColumns;
	}

	std::cerr << "Unknown list " << list << std::endl;

	return 0;
}

std::string Parser::getTableCreationStatement(const enum Parser::List &list)
{
	std::string result = std::string("CREATE TABLE ") + Parser::getTableName(list) + "(Id INT PRIMARY KEY,";

	switch (list)
	{
		case Parser::Comments:
			result +=
			Comment::sqlColumnStatement;

			break;

		case Parser::Keywords:
			result +=
			Keyword::sqlColumnStatement;

			break;

		case Parser::Keys:
			result +=
			Key::sqlColumnStatement;

			break;

		case Parser::TextMacros:
			result +=
			TextMacro::sqlColumnStatement;

			break;

		case Parser::TextMacroInstances:
			result +=
			TextMacroInstance::sqlColumnStatement;

			break;

		case Parser::Types:
			result +=
			Type::sqlColumnStatement;

			break;

		case Parser::Locals:
			result +=
			Local::sqlColumnStatement;

			break;

		case Parser::Globals:
			result +=
			Global::sqlColumnStatement;

			break;

		case Parser::Members:
			result +=
			Member::sqlColumnStatement;

			break;

		case Parser::Parameters:
			result += Parameter::sqlColumnStatement;

			break;

		case Parser::FunctionInterfaces:
			result +=
			FunctionInterface::sqlColumnStatement;

			break;

		case Parser::Functions:
			result +=
			Function::sqlColumnStatement;

			break;

		case Parser::Methods:
			result +=
			Method::sqlColumnStatement;

			break;

		case Parser::Calls:
			result +=
			Call::sqlColumnStatement;

			break;

		case Parser::Implementations:
			result +=
			Implementation::sqlColumnStatement;

			break;

		case Parser::Hooks:
			result +=
			Hook::sqlColumnStatement;

			break;

		case Parser::Interfaces:
			result +=
			Interface::sqlColumnStatement;

			break;

		case Parser::Structs:
			result +=
			Struct::sqlColumnStatement;

			break;

		case Parser::Modules:
			result +=
			Module::sqlColumnStatement;

			break;

		case Parser::Scopes:
			result +=
			Scope::sqlColumnStatement;

			break;

		case Parser::Libraries:
			result +=
			Library::sqlColumnStatement;

			break;

		case Parser::SourceFiles:
			result +=
			SourceFile::sqlColumnStatement;

			break;

		case Parser::DocComments:
			result +=
			DocComment::sqlColumnStatement;

			break;

		default:
			std::cerr << "Unknown list " << list << std::endl;

			break;
	}

	result += ")";

	return result;
}

class Object* Parser::createObjectByVector(std::vector<const unsigned char*> &columnVector, const enum Parser::List &list)
{
	switch (list)
	{
		case Parser::Comments:
			return static_cast<class Object*>(new Comment(columnVector));

		case Parser::Keywords:
			return static_cast<class Object*>(new Keyword(columnVector));

		case Parser::Keys:
			return static_cast<class Object*>(new Key(columnVector));

		case Parser::TextMacros:
			return static_cast<class Object*>(new TextMacro(columnVector));

		case Parser::TextMacroInstances:
			return static_cast<class Object*>(new TextMacroInstance(columnVector));

		case Parser::Types:
			return static_cast<class Object*>(new Type(columnVector));

		case Parser::Locals:
			return static_cast<class Object*>(new Local(columnVector));

		case Parser::Globals:
			return static_cast<class Object*>(new Global(columnVector));

		case Parser::Members:
			return static_cast<class Object*>(new Member(columnVector));

		case Parser::Parameters:
			return static_cast<class Object*>(new Parameter(columnVector));

		case Parser::FunctionInterfaces:
			return static_cast<class Object*>(new FunctionInterface(columnVector));

		case Parser::Functions:
			return static_cast<class Object*>(new Function(columnVector));

		case Parser::Methods:
			return static_cast<class Object*>(new Method(columnVector));

		case Parser::Calls:
			return static_cast<class Object*>(new Call(columnVector));

		case Parser::Implementations:
			return static_cast<class Object*>(new Implementation(columnVector));

		case Parser::Hooks:
			return static_cast<class Object*>(new Hook(columnVector));

		case Parser::Interfaces:
			return static_cast<class Object*>(new Interface(columnVector));

		case Parser::Structs:
			return static_cast<class Object*>(new Struct(columnVector));

		case Parser::Modules:
			return static_cast<class Object*>(new Module(columnVector));

		case Parser::Scopes:
			return static_cast<class Object*>(new Scope(columnVector));

		case Parser::Libraries:
			return static_cast<class Object*>(new Library(columnVector));

		case Parser::SourceFiles:
			return static_cast<class Object*>(new SourceFile(columnVector));

		case Parser::DocComments:
			return static_cast<class Object*>(new DocComment(columnVector));

		default:
			std::cerr << "Unknown list " << list << std::endl;
			break;
	}

	return 0;
}
#endif

void Parser::getStructInheritanceList(const class Interface *extension, const std::string &prefix, std::ostream &ostream)
{
	SpecificObjectList structList = this->getSpecificList<Struct::HasExtension>(Parser::Structs, extension);

	if (structList.empty())
		return;

	ostream << prefix << "<ul>\n";

	BOOST_FOREACH(SpecificObjectList::reference ref, structList)
	{
		ostream << prefix << "\t<li>" << ref.second->pageLink() << '\n';
		this->getStructInheritanceList(boost::polymorphic_downcast<Interface*>(ref.second), prefix + "\t\t", ostream);
		ostream << prefix << "\t</li>\n";
	}

	ostream << prefix << "</ul>\n";
}

void Parser::getLibraryRequirementList(const class Library *requirement, const std::string &prefix, std::ostream &ostream)
{
	SpecificObjectList specifiedList = this->getSpecificList<Library::HasRequirement>(Parser::Libraries, requirement);

	if (specifiedList.empty())
		return;

	ostream << prefix << "<ul>\n";

	BOOST_FOREACH(SpecificObjectList::reference ref, specifiedList)
	{
		ostream << prefix << "\t<li>" << ref.second->pageLink() << '\n';
		this->getLibraryRequirementList(boost::polymorphic_downcast<Library*>(ref.second), prefix + "\t\t", ostream);
		ostream << prefix << "\t</li>\n";
	}

	ostream << prefix << "</ul>\n";
}

}

