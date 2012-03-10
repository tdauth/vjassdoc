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

#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>

#include "internationalisation.hpp"
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
		typedef uint64_t IdType;
		typedef std::size_t LineType;

		static std::string objectPageLink(const class Object *object, const std::string &identifier = "-");
		static std::string objectLink(const class Object *object, const std::string &identifier = "-"); //should use the id
		static IdType objectId(const class Object *object);
		static std::string objectIdString(const class Object *object);
#ifdef SQLITE
		static std::string sqlFilteredString(const std::string &usedString);
		/// @todo This function should be used to distribute all table header creations to their classes.
		static std::string sqlTableHeader(const std::string &tableName, const std::string &entries);
#endif

		Object(class Parser *parser, const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment);
		Object(class Parser *parser);
		virtual ~Object();

		virtual void init() = 0; //Some Objects has to be initialized after finding all objects of all files.
#ifdef SQLITE
		virtual const char* sqlTableName() const = 0;
		typedef std::vector<std::string> SqlColumn;
		virtual std::size_t sqlSize() const;
		virtual SqlColumn sqlColumn() const;
		virtual SqlColumn sqlNames() const;
		virtual SqlColumn sqlTypes() const;
		virtual SqlColumn sqlValues() const;

		virtual void initBySql(const SqlColumn &column);

		virtual std::string sqlStatement() const;
#endif
		virtual void pageNavigation(std::ofstream &file) const = 0;
		virtual void page(std::ofstream &file) const = 0;
		std::string pageLink() const;
		void setId(IdType id); /// @todo Friend relation to \ref Parser.
		IdType id() const;
		class Parser* parser() const;
		void addIdentifier(const std::string &identifier); //required by doc comments
		std::string identifier() const;
		class SourceFile* sourceFile() const;
		Object::LineType line() const;
		class DocComment* docComment() const;

		//empty condition methods, used by the search algorithm's comparison
		virtual class Object* container() const;
		virtual class Scope* scope() const;
		virtual class Library* library() const;

		struct AlphabeticalComparator : public std::binary_function<const class Object*, const class Object*, bool>
		{
			bool operator()(const class Object *first, const class Object *second) const;
		};

		struct IsInContainer : public std::binary_function<const class Object*, const class Object*, bool>
		{
			virtual bool operator()(const class Object *thisObject, const class Object *container) const;
		};

		struct IsInScope : public std::binary_function<const class Object*, const class Object*, bool>
		{
			virtual bool operator()(const class Object *thisObject, const class Object *scope) const;
		};

		struct IsInLibrary : public std::binary_function<const class Object*, const class Object*, bool>
		{
			virtual bool operator()(const class Object *thisObject, const class Object *library) const;
		};

	protected:
		/**
		* Checks if the value \p expression which has type \p type is a literal or not.
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

		/**
		* Checks if \p valueExpression is a literal or an object. If it's an object (like a global or function call) it will be searched in parser lists.
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

		//Object(Object&);

		IdType m_id;
		class Parser *m_parser;
		std::string m_identifier;
		class SourceFile *m_sourceFile;
		Object::LineType m_line;
		class DocComment *m_docComment;
};

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

inline std::string Object::objectIdString(const Object* object)
{
	return boost::lexical_cast<std::string>(objectId(object));
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

inline Parser* Object::parser() const
{
	return this->m_parser;
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

}

#endif
