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

#ifndef VJASSDOC_OBJECT_HPP
#define VJASSDOC_OBJECT_HPP

#include <string>
#include <vector>
#include <fstream>
#include <functional>

#include "parser.hpp"
#include "internationalisation.hpp"
#include "vjassdoc.hpp"
#include "vjassdocConfig.h"

namespace vjassdoc
{

class SourceFile;
class DocComment;
class Scope;
class Library;
class Type;

class Object
{
	public:
		typedef unsigned int IdType;
		typedef unsigned int LineType;
#ifdef SQLITE
		typedef const unsigned char* VectorDataType;
#endif

		//static const char *sqlTableName;
		static unsigned int sqlColumns;
		static std::string sqlColumnStatement;

		static Object::IdType maxIds();
		static void initClass();
		static std::string objectPageLink(const class Object *object, const std::string &identifier = "-");
		static std::string objectLink(const class Object *object, const std::string &identifier = "-"); //should use the id
		static Object::IdType objectId(const class Object *object);
		static void setMaxIds(Object::IdType maxIds); //only used by loading objects from database
#ifdef SQLITE
		static std::string sqlFilteredString(const std::string &usedString);
		/// @todo This function should be used to distribute all table header creations to their classes.
		static std::string sqlTableHeader(const std::string &tableName, const std::string &entries);
#endif

		Object(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment);
#ifdef SQLITE
		Object(std::vector<Object::VectorDataType> &columnVector);
#endif
		virtual ~Object();

		virtual void init() = 0; //Some Objects has to be initialized after finding all objects of all files.
#ifdef SQLITE
		virtual void initByVector();
		void clearVector();
		virtual std::string sqlStatement() const;
#endif
		virtual void pageNavigation(std::ofstream &file) const = 0;
		virtual void page(std::ofstream &file) const = 0;
		std::string pageLink() const;
		void setId(Object::IdType id); /// @todo Friend relation to @class Parser.
		Object::IdType id() const;
		void addIdentifier(const std::string &identifier); //required by doc comments
		std::string identifier() const;
		class SourceFile* sourceFile() const;
		Object::LineType line() const;
		class DocComment* docComment() const;
#ifdef SQLITE
		std::vector<Object::VectorDataType> columnVector() const;
#endif

		//empty condition methods, used by the search algorithm's comparison
		virtual class Object* container() const;
		virtual class Scope* scope() const;
		virtual class Library* library() const;

		struct AlphabeticalComparator : public std::binary_function<const class Object*, const class Object*, bool>
		{
			bool operator()(const class Object *first, const class Object *second) const;
		};

		struct IsInContainer : public Parser::Comparator
		{
			virtual bool operator()(const class Object *thisObject, const class Object *container) const;
		};

		struct IsInScope : public Parser::Comparator
		{
			virtual bool operator()(const class Object *thisObject, const class Object *scope) const;
		};

		struct IsInLibrary : public Parser::Comparator
		{
			virtual bool operator()(const class Object *thisObject, const class Object *library) const;
		};

	protected:
		/**
		* Checks if the value @param expression which has type @param type is a literal or not.
		* If it is no literal method will return true.
		*/
		static bool hasToSearchValueObject(class Object *type, const std::string &expression);

		/**
		* Shows a boolean value.
		* This method has been written for showing boolean members of classes in a HTML file.
		* Instead of 0 and 1 it returns the strings 'No' and 'Yes' in the local language.
		*/
		static std::string showBooleanProperty(const bool &property);

		void setIdentifier(const std::string &identifier); //required by DocComment
		std::string anchor() const;
		class Object* searchObjectInList(const std::string &identifier, const enum Parser::List &list, enum Parser::SearchMode searchMode = Parser::Unspecified);

		/**
		* Checks if @param valueExpression is a literal or an object. If it's an object (like a global or function call) it will be searched in parser lists.
		*/
		class Object* findValue(class Object *type, std::string &valueExpression);
#ifdef SQLITE
		void prepareVector();
#endif

		class Object *m_container; /// @todo Interface?
		class Scope *m_scope;
		class Library *m_library;

	private:
		//static std::string getAnotherLink(const Object *object);
		static Object::IdType m_maxIds;

		//Object(Object&);

		Object::IdType m_id;
		std::string m_identifier;
		class SourceFile *m_sourceFile;
		Object::LineType m_line;
		class DocComment *m_docComment;
#ifdef SQLITE
		std::vector<Object::VectorDataType> m_columnVector;
#endif
};

inline Object::IdType Object::maxIds()
{
	return Object::m_maxIds;
}

inline std::string Object::objectPageLink(const class Object *object, const std::string &identifier)
{
	if (object == 0)
		return identifier;

	return object->pageLink();
}

inline std::string Object::objectLink(const class Object *object, const std::string &identifier)
{
	if (object == 0)
		return identifier;

	std::ostringstream sstream;
	sstream << "<a href=\"#n" << object->m_id << "\">" << object->m_identifier << "</a>";

	return sstream.str();
}

inline Object::IdType Object::objectId(const class Object *object)
{
	if (object == 0)
		return -1;

	return object->m_id;
}

inline void Object::setMaxIds(Object::IdType maxIds)
{
	Object::m_maxIds = maxIds;
}

inline std::string Object::pageLink() const
{
	std::ostringstream sstream;
	sstream << "<a href=\"" << this->m_id << ".html\">";

	if (this->m_identifier.empty())
		sstream << this->m_id;
	else
		sstream << this->m_identifier;

	sstream << "</a>"; //if empty show id
	return sstream.str();
}

inline void Object::setId(Object::IdType id)
{
	this->m_id = id;
}

inline Object::IdType Object::id() const
{
	return this->m_id;
}

inline void Object::addIdentifier(const std::string &identifier)
{
	this->m_identifier += identifier;
}

inline std::string Object::identifier() const
{
	return this->m_identifier;
}

inline class SourceFile* Object::sourceFile() const
{
	return this->m_sourceFile;
}

inline Object::LineType Object::line() const
{
	return this->m_line;
}

inline class DocComment* Object::docComment() const
{
	return this->m_docComment;
}

#ifdef SQLITE
inline std::vector<Object::VectorDataType> Object::columnVector() const
{
	return this->m_columnVector;
}
#endif

inline bool Object::AlphabeticalComparator::operator()(const class Object *first, const class Object *second) const
{
	return first->identifier().compare(second->identifier()) < 0;
}

inline std::string Object::showBooleanProperty(const bool &property)
{
	if (property)
		return _("Yes");

	return _("No");
}

inline void Object::setIdentifier(const std::string &identifier)
{
	m_identifier = identifier;
}

inline std::string Object::anchor() const
{
	std::ostringstream sstream;

	if (Vjassdoc::optionPages())
		return this->pageLink();

	return this->m_identifier;
}

inline class Object* Object::searchObjectInList(const std::string &identifier, const enum Parser::List &list, enum Parser::SearchMode searchMode)
{
	return Vjassdoc::parser()->searchObjectInList(identifier, list, searchMode, this);
}

}

#endif
