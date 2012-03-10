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

#ifndef VJASSDOC_LIBRARY_HPP
#define VJASSDOC_LIBRARY_HPP

#include <boost/scoped_ptr.hpp>

#include "object.hpp"

namespace vjassdoc
{

class Library : public Object
{
	public:
		struct HasRequirement : public std::binary_function<const Library*, const Library*, bool>
		{
			virtual bool operator()(const Library *thisObject, const Library *library) const;
		};

		class Requirement
		{
			public:
				Requirement(const std::string &expression, Library *library, bool isOptional);

				std::string& expression();
				const std::string& expression() const;
				void setLibrary(Library *library);
				Library* library() const;
				bool isOptional() const;

			private:
				std::string m_expression;
				Library *m_library;
				bool m_isOptional;
		};

		typedef std::list<Requirement> RequirementsContainer;
		typedef boost::scoped_ptr<RequirementsContainer> Requirements;

#ifdef SQLITE
		static const char *sqlTableName;
		static unsigned int sqlColumns;
		static std::string sqlColumnStatement;

		static void initClass();
#endif
		Library(class Parser *parser, const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, bool isOnce, const std::string &initializerExpression, RequirementsContainer *requirements);
		Library(std::vector<const unsigned char*> &columnVector);
		virtual ~Library();
		virtual void init();
		virtual void pageNavigation(std::ofstream &file) const;
		virtual void page(std::ofstream &file) const;
#ifdef SQLITE
		virtual std::string sqlStatement() const;
#endif
		bool isOnce() const;
		class Function* initializer() const; //Function, Method (static)
		Requirements& requirements();
		const Requirements& requirements() const;

	protected:
		bool m_isOnce;
		std::string initializerExpression;

		class Function *m_initializer; //Function is the parent class of Method, so it can be a method, too.
		Requirements m_requirements;
};

inline Library::Requirement::Requirement(const std::string &expression, Library *library, bool isOptional) : m_expression(expression), m_library(library), m_isOptional(isOptional)
{

}

inline std::string& Library::Requirement::expression()
{
	return m_expression;
}

inline const std::string& Library::Requirement::expression() const
{
	return m_expression;
}

inline void Library::Requirement::setLibrary(Library *library)
{
	this->m_library = library;
}

inline Library* Library::Requirement::library() const
{
	return m_library;
}

inline bool Library::Requirement::isOptional() const
{
	return m_isOptional;
}

inline bool Library::isOnce() const
{
	return this->m_isOnce;
}

inline class Function* Library::initializer() const
{
	return this->m_initializer;
}

inline Library::Requirements& Library::requirements()
{
	return m_requirements;
}

inline const Library::Requirements& Library::requirements() const
{
	return this->m_requirements;
}

}

#endif
