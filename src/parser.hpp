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

#ifndef VJASSDOC_PARSER_HPP
#define VJASSDOC_PARSER_HPP

#include <ostream>
#include <functional>
#include <list>
#include <string>
#include <sstream>
#include <vector>

#include "vjassdocConfig.h"

namespace vjassdoc
{

class Comment;
class Keyword;
class TextMacro;
class TextMacroInstance;
class Type;
class Local;
class Global;
class Member;
class Parameter;
class FunctionInterface;
class Function;
class Method;
class Call;
class Implementation;
class Interface;
class Struct;
class Module;
class Scope;
class Library;
class SourceFile;
class DocComment;
class SyntaxError;

/**
* Provides methods for parsing Jass and vJass files. The Parser class has the ability to create a simple HTML
* API documentation and/or a SQLite3 database.
* It contains a list for each Object child class.
* @class File is the class which handles real specific Jass and vJass parsing. Parser does create one or several File instances by getting all file paths from class @class Vjassdoc.
* After parsing all of those files it is able to create an HTML documentation and an SQLite3 database.
* Besides it provides several search functions which are required for object initialization which usually is runned after parsing for each parsed object.
* @see Vjassdoc, File
*/
class Parser
{
	public:
		struct Comparator : public std::binary_function<const class Object*, const class Object*, bool>
		{
			virtual bool operator()(const class Object *thisObject, const class Object *otherObject) const;
		};

		enum List
		{
			Comments,
			Keywords,
			Keys,
			TextMacros,
			TextMacroInstances,
			Types,
			Locals,
			Globals,
			Members,
			Parameters,
			FunctionInterfaces,
			Functions,
			Methods,
			Calls,
			Implementations,
			Hooks,
			Interfaces,
			Structs,
			Modules,
			Scopes,
			Libraries,
			SourceFiles,
			DocComments,
			MaxLists
		};

		enum SearchMode
		{
			Unspecified = 0, /// No specific search mode.
			CheckContainer = 0x02, /// Container has to be the same.
			CheckScope = 0x04,
			CheckLibrary = 0x08
		};


		/**
		* Sometimes it can be really useful to create your own small object list. In that case you'll probably want to be able to search in your custom list.
		* @param objectList User-defined custom list where the method has to search.
		* @param identifier Object identifier which is searched for.
		* @param searchMode Used search mode. Check out @enum Parser::SearchMode.
		* @param object Object instance used for comparisons. If searchMode is @enum Parser::SearchMode::Unspecified this value can be 0.
		* @return If no object was found it will return 0.
		* @see Parser::getSpecificList, Parser::searchObjectInLastDatabase, Parser::searchObjectInList
		*/
		static class Object* searchObjectInCustomList(const std::list<class Object*> &objectList, const std::string &identifier, const enum Parser::SearchMode &searchMode = Unspecified, const class Object *object = 0);

		Parser();
		~Parser();
		void createInheritanceListPage();
		void createRequirementListPage();
		void sortAlphabetically();
		void createHtmlFiles();
		void showSyntaxErrors();
		void parse(const std::list<std::string> &filePaths);

#ifdef SQLITE
		void createDatabase(const std::string &filePath);
		int addDatabase(const std::string &filePath);
		void removeDatabase(const int &index);
		/**
		* Searches for an object with id @param id in last added database. Each id has to be unique so there only can be one hit.
		* @return If no object was found it will return 0.
		*/
		class Object* searchObjectInLastDatabase(const unsigned int &id);
#endif

		/**
		* Searches for an object in one of the parsers lists.
		* @param identifier Object identifier which is searched for.
		* @param list Parser list which is searched in. Check out @enum Parser::List.
		* @param searchMode Used search mode. Check out @enum Parser::SearchMode.
		* @param object Object instance used for comparisons. If searchMode is @enum Parser::SearchMode::Unspecified this value can be 0.
		* @return If no object was found it will return 0.
		*/
		class Object* searchObjectInList(const std::string &identifier, const enum Parser::List &list,  const enum Parser::SearchMode &searchMode = Unspecified, const class Object *object = 0);

		/**
		* Returns a user-specific list by iterating one of the parsers lists and calling a comparator.
		* @param list List which is iterated.
		* @param comparator Comparator which is used for comparisions. If it returns true object will be added to user-specific list.
		* @param object Comparators allows to compare two objects. If you don't have to compare two objects this value can be 0.
		* @return Returns the user-specific list.
		* @see Parser::searchObjectInCustomList
		*/
		std::list<class Object*> getSpecificList(const enum List &list, const struct Comparator &comparator, const class Object *object = 0);

		void add(class Comment *comment);
		void add(class Keyword *keyword);
		void add(class Key *key);
		void add(class TextMacro *textMacro);
		void add(class TextMacroInstance *textMacroInstance);
		void add(class Type *type);
		void add(class Local *local);
		void add(class Global *global);
		void add(class Member *member);
		void add(class Parameter *parameter);
		void add(class FunctionInterface *functionInterface);
		void add(class Function *function);
		void add(class Method *method);
		void add(class Call *call);
		void add(class Implementation *implementation);
		void add(class Hook *hook);
		void add(class Interface *interface);
		void add(class Struct *usedStruct);
		void add(class Module *module);
		void add(class Scope *scope);
		void add(class Library *library);
		void add(class SourceFile *sourceFile);
		void add(class DocComment *docComment);
		class Type* integerType() const;
		class Type* realType() const;
		class Type* stringType() const;
		class Type* booleanType() const;
		class Type* handleType() const;
		class Type* codeType() const;
		class SourceFile* currentSourceFile() const;

		void add(class SyntaxError *syntaxError);

		/**
		* Parses line @param line from index @param index to the end of line and generates a list of possible objects.
		* This can be useful for IDEs or other editors to create an auto completion for their users.
		* @param line The code line.
		* @param index Start index of code line for the parser.
		* @return A list filled with possible objects.
		*/
		std::list<class Object*> autoCompletion(const std::string &line, std::size_t &index);
	private:
#ifdef SQLITE
		struct Database
		{
			std::list<enum List> listList;
			std::list<class Object*> objectList;
		};
#endif

		static const char *title[Parser::MaxLists];
		class Type *m_integerType;
		class Type *m_realType;
		class Type *m_stringType;
		class Type *m_booleanType;
		class Type *m_handleType;
		class Type *m_codeType;
		std::list<class Comment*> m_comments;
		std::list<class Keyword*> m_keywords;
		std::list<class Key*> m_keys;
		std::list<class TextMacro*> m_textMacros;
		std::list<class TextMacroInstance*> m_textMacroInstances;
		std::list<class Type*> m_types;
		std::list<class Local*> m_locals;
		std::list<class Global*> m_globals;
		std::list<class Member*> m_members;
		std::list<class Parameter*> m_parameters;
		std::list<class FunctionInterface*> m_functionInterfaces;
		std::list<class Function*> m_functions;
		std::list<class Method*> m_methods;
		std::list<class Call*> m_calls;
		std::list<class Implementation*> m_implementations;
		std::list<class Hook*> m_hooks;
		std::list<class Interface*> m_interfaces;
		std::list<class Struct*> m_structs;
		std::list<class Module*> m_modules;
		std::list<class Scope*> m_scopes;
		std::list<class Library*> m_libraries;
		std::list<class SourceFile*> m_sourceFiles;
		std::list<class DocComment*> m_docComments;
		class SourceFile *m_currentSourceFile;
		std::list<class SyntaxError*> m_syntaxErrors;
#ifdef SQLITE
		std::vector<struct Database*> m_databases;
#endif

		void parseFile(const std::string &path);
		std::list<class Object*>& getList(enum List list);

		//HTML
		std::ostream& addObjectList(std::ostream &output, const enum Parser::List &list);

		//SQLite
#ifdef SQLITE
		static std::string getTableName(const enum Parser::List &list);
		static unsigned int getTableColumns(const enum Parser::List &list);
		static std::string getTableCreationStatement(const enum Parser::List &list);
		static class Object* createObjectByVector(std::vector<const unsigned char*> &columnVector, const enum Parser::List &list);
#endif

		void getStructInheritanceList(const class Interface *extension, const std::string &prefix, std::ostream &ostream);
		void getLibraryRequirementList(const class Library *requirement, const std::string &prefix, std::ostream &ostream);
};

inline void Parser::add(class Comment *comment)
{
	this->m_comments.push_back(comment);
}

inline void Parser::add(class Key *key)
{
	this->m_keys.push_back(key);
}

inline void Parser::add(class Keyword *keyword)
{
	this->m_keywords.push_back(keyword);
}

inline void Parser::add(class TextMacro *textMacro)
{
	this->m_textMacros.push_back(textMacro);
}

inline void Parser::add(class TextMacroInstance *textMacroInstance)
{
	this->m_textMacroInstances.push_back(textMacroInstance);
}

inline void Parser::add(class Type *type)
{
	this->m_types.push_back(type);
}

inline void Parser::add(class Local *local)
{
	this->m_locals.push_back(local);
}

inline void Parser::add(class Global *global)
{
	this->m_globals.push_back(global);
}

inline void Parser::add(class Member *member)
{
	this->m_members.push_back(member);
}

inline void Parser::add(class Parameter *parameter)
{
	this->m_parameters.push_back(parameter);
}

inline void Parser::add(class FunctionInterface *functionInterface)
{
	this->m_functionInterfaces.push_back(functionInterface);
}

inline void Parser::add(class Function *function)
{
	this->m_functions.push_back(function);
}

inline void Parser::add(class Method *method)
{
	this->m_methods.push_back(method);
}

inline void Parser::add(class Call *call)
{
	this->m_calls.push_back(call);
}

inline void Parser::add(class Implementation *implementation)
{
	this->m_implementations.push_back(implementation);
}

inline void Parser::add(class Hook *hook)
{
	this->m_hooks.push_back(hook);
}

inline void Parser::add(class Interface *interface)
{
	this->m_interfaces.push_back(interface);
}

inline void Parser::add(class Struct *usedStruct)
{
	this->m_structs.push_back(usedStruct);
}

inline void Parser::add(class Module *module)
{
	this->m_modules.push_back(module);
}

inline void Parser::add(class Scope *scope)
{
	this->m_scopes.push_back(scope);
}

inline void Parser::add(class Library *library)
{
	this->m_libraries.push_back(library);
}

inline void Parser::add(class SourceFile *sourceFile)
{
	this->m_sourceFiles.push_back(sourceFile);
}

inline void Parser::add(class DocComment *docComment)
{
	this->m_docComments.push_back(docComment);
}

inline class Type* Parser::integerType() const
{
	return this->m_integerType;
}

inline class Type* Parser::realType() const
{
	return m_realType;
}

inline class Type* Parser::stringType() const
{
	return m_stringType;
}

inline class Type* Parser::booleanType() const
{
	return m_booleanType;
}

inline class Type* Parser::handleType() const
{
	return m_handleType;
}

inline class Type* Parser::codeType() const
{
	return m_codeType;
}

inline class SourceFile* Parser::currentSourceFile() const
{
	return this->m_currentSourceFile;
}

inline void Parser::add(class SyntaxError *syntaxError)
{
	this->m_syntaxErrors.push_back(syntaxError);
}

}

#endif
