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

#ifndef VJASSDOC_PARSER_HPP
#define VJASSDOC_PARSER_HPP

#include <ostream>
#include <functional>
#include <list>
#include <string>
#include <sstream>
#include <vector>

#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <boost/foreach.hpp>
#include <boost/cast.hpp>

#include "object.hpp"
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
* \ref File is the class which handles real specific Jass and vJass parsing. Parser does create one or several File instances by getting all file paths from class @class Vjassdoc.
* After parsing all of those files it is able to create an HTML documentation and an SQLite3 database.
* Besides it provides several search functions which are required for object initialization which usually is runned after parsing for each parsed object.
* \sa Vjassdoc, File
*/
class Parser
{
	public:
		typedef boost::ptr_unordered_multimap<std::string, Object> ObjectList;
		typedef boost::unordered_multimap<std::string, Object*> SpecificObjectList;

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
		* @param searchMode Used search mode. Check out @ref Parser::SearchMode.
		* @param object Object instance used for comparisons. If searchMode is \ref Parser::SearchMode::Unspecified this value can be 0.
		* @return If no object was found it will return 0.
		* \sa Parser::getSpecificList, Parser::searchObjectInLastDatabase, Parser::searchObjectInList
		*/
		template<typename ListType>
		static class Object* searchObjectInCustomList(ListType &objectList, const std::string &identifier, Parser::SearchMode searchMode = Unspecified, const class Object *object = 0);

		Parser(class Vjassdoc *parent);
		~Parser();

		Object::IdType nextId();

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
		* @param list Parser list which is searched in. Check out @ref Parser::List.
		* @param searchMode Used search mode. Check out @ref Parser::SearchMode.
		* @param object Object instance used for comparisons. If searchMode is @ref Parser::SearchMode::Unspecified this value can be 0.
		* @return If no object was found it will return 0.
		*/
		class Object* searchObjectInList(const std::string &identifier, Parser::List list,  Parser::SearchMode searchMode = Unspecified, const class Object *object = 0);
		class Object* searchObjectInList(const std::string &identifier, Parser::List list, const class Object *object);

		/**
		* Returns a user-specific list by iterating one of the parsers lists and calling a comparator.
		* @param list List which is iterated.
		* @param ComparatorType Comparator which is used for comparisions. If it returns true object will be added to user-specific list.
		* @param object Comparators allows to compare two objects. If you don't have to compare two objects this value can be 0.
		* @return Returns the user-specific list.
		* @see Parser::searchObjectInCustomList
		*/
		template<typename ComparatorType>
		SpecificObjectList getSpecificList(List list, const class Object *object = 0);
		ObjectList& getList(enum List list);
		const ObjectList& getList(enum List list) const;

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
		class Vjassdoc* parent() const;
		class Type* integerType() const;
		class Type* realType() const;
		class Type* stringType() const;
		class Type* booleanType() const;
		class Type* handleType() const;
		class Type* codeType() const;
		class SourceFile* currentSourceFile() const;

		void add(class SyntaxError *syntaxError);

		/**
		* Parses line @p line from index @p index to the end of line and generates a list of possible objects.
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
		Object::IdType m_currentId;
		class Vjassdoc *m_parent;
		class Type *m_integerType;
		class Type *m_realType;
		class Type *m_stringType;
		class Type *m_booleanType;
		class Type *m_handleType;
		class Type *m_codeType;
		ObjectList m_comments;
		ObjectList m_keywords;
		ObjectList m_keys;
		ObjectList m_textMacros;
		ObjectList m_textMacroInstances;
		ObjectList m_types;
		ObjectList m_locals;
		ObjectList m_globals;
		ObjectList m_members;
		ObjectList m_parameters;
		ObjectList m_functionInterfaces;
		ObjectList m_functions;
		ObjectList m_methods;
		ObjectList m_calls;
		ObjectList m_implementations;
		ObjectList m_hooks;
		ObjectList m_interfaces;
		ObjectList m_structs;
		ObjectList m_modules;
		ObjectList m_scopes;
		ObjectList m_libraries;
		ObjectList m_sourceFiles;
		ObjectList m_docComments;
		class SourceFile *m_currentSourceFile;
		std::list<class SyntaxError*> m_syntaxErrors;
#ifdef SQLITE
		std::vector<struct Database*> m_databases;
#endif

		void parseFile(const std::string &path);

		//HTML
		std::ostream& addObjectList(std::ostream &output, Parser::List list);

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

template<typename ListType>
Object* Parser::searchObjectInCustomList(ListType &objectList, const std::string &identifier, Parser::SearchMode searchMode, const Object *object)
{
	if (objectList.size() == 0)
		return 0;

	boost::iterator_range<typename ListType::iterator> range = objectList.equal_range(identifier);

	if (range.empty())
		return 0;

	Object *resultObject = 0;
	bool checkContainer = false;
	bool checkScope = false;
	bool checkLibrary = false;

	if ((object != 0 && object->container() != 0) || searchMode & CheckContainer)
		checkContainer = true;

	if ((object != 0 && object->scope() != 0) || searchMode & CheckScope)
		checkScope = true;

	if ((object != 0 && object->library() != 0) || searchMode & checkLibrary)
		checkLibrary = true;

	for (typename ListType::iterator iterator = range.begin(); iterator != range.end(); ++iterator)
	{
		if (resultObject == 0 && searchMode == Unspecified)
		{
			resultObject = iterator->second;

			if (checkContainer)
			{
				if (resultObject->container() == object->container())
					checkContainer = false;
			}

			if (checkScope)
			{
				if (resultObject->scope() == object->scope())
					checkScope = false;
			}

			if (checkLibrary)
			{
				if (resultObject->library() == object->library())
					checkLibrary = false;
			}

			//found the object
			if (!checkContainer && !checkScope && !checkLibrary)
				break;

			continue;
		}


		if (checkContainer)
		{
			if (iterator->second->container() == object->container())
			{
				checkContainer = false;
				resultObject = iterator->second;

				continue;
			}
		}

		if (checkScope)
		{
			if (iterator->second->scope() == object->container())
			{
				checkScope = false;
				resultObject = iterator->second;

				continue;
			}
		}

		if (checkLibrary)
		{
			if (iterator->second->library() == object->library())
			{
				checkLibrary = false;
				resultObject = iterator->second;

				continue;
			}
		}
	}

	return resultObject;
}

template<typename ComparatorType>
Parser::SpecificObjectList Parser::getSpecificList(Parser::List list, const Object *object)
{
	ObjectList &objectList = this->getList(list);
	SpecificObjectList result;
	ComparatorType comp;

	BOOST_FOREACH(ObjectList::reference ref, objectList)
	{
		if (comp(boost::polymorphic_cast<typename ComparatorType::first_argument_type>(ref.second), boost::polymorphic_cast<typename ComparatorType::second_argument_type>(object)))
		{
			SpecificObjectList::value_type value = SpecificObjectList::value_type(ref.first, ref.second);
			result.insert(value);
		}
	}

	return result;
}

inline Object::IdType Parser::nextId()
{
	Object::IdType result = m_currentId;
	++m_currentId;

	return result;
}

inline class Vjassdoc* Parser::parent() const
{
	return this->m_parent;
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
