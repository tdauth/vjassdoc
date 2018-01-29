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

#ifndef VJASSDOC_FUNCTIONINTERFACE_HPP
#define VJASSDOC_FUNCTIONINTERFACE_HPP

#include "object.hpp"

namespace vjassdoc
{

class Parameter;
class Parser;

class FunctionInterface : public Object
{
	public:
#ifdef SQLITE
		static const char *sqlTableName;
		static unsigned int sqlColumns;
		static std::string sqlColumnStatement;

		static void initClass();
#endif
		typedef std::list<Parameter*> Parameters;

		FunctionInterface(Parser *parser, const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Library *library, class Scope *scope, bool isPrivate, Parameters parameters, const std::string &returnTypeExpression);
#ifdef SQLITE
		FunctionInterface(std::vector<const unsigned char*> &columnVector);
#endif
		virtual ~FunctionInterface();
		virtual void init();
		virtual void pageNavigation(std::ofstream &file) const;
		virtual void page(std::ofstream &file) const;
#ifdef SQLITE
		virtual std::string sqlStatement() const;
#endif
		virtual class Library* library() const;
		virtual class Scope* scope() const;
		bool isPrivate() const;
		const Parameters& parameters() const;
		class Object* returnType() const; //Type, Function Interface, Interface, Struct
		const std::string& returnTypeExpression() const;

	protected:
		static const int maxParameters;
		class Library *m_library;
		class Scope *m_scope;
		bool m_isPrivate;
		Parameters m_parameters;
		class Object *m_returnType;
		std::string m_returnTypeExpression;
};

inline bool FunctionInterface::isPrivate() const
{
	return this->m_isPrivate;
}

inline const FunctionInterface::Parameters& FunctionInterface::parameters() const
{
	return this->m_parameters;
}

inline class Object* FunctionInterface::returnType() const
{
	return this->m_returnType;
}

inline const std::string& FunctionInterface::returnTypeExpression() const
{
	return this->m_returnTypeExpression;
}

}

#endif
