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

#include <sstream>
#include <cstdlib>
#include <cctype>

#include <boost/cast.hpp>

#include "objects.hpp"
#include "sourcefile.hpp"
#include "doccomment.hpp"
#include "parser.hpp"
#include "vjassdoc.hpp"
#include "internationalisation.hpp"
#include "file.hpp"

namespace vjassdoc
{

#ifdef SQLITE
std::string Object::sqlFilteredString(const std::string &usedString)
{
	std::ostringstream result;
	// TODO set buffer size to usedString.length() + 2
	result << "\"";

	for (std::string::size_type i = 0; i < usedString.length(); ++i)
	{
		char character = usedString[i];

		switch (character)
		{
			case '\'':
				result << "\'\'";
				break;

			case '\"':
				result << "\"\"";
				break;

			default:
				result << character;
				break;
		}
	}

	result << "\"";

	return result.str();
}

std::string Object::sqlTableHeader(const std::string &tableName, const std::string &entries)
{
	return "CREATE TABLE " + tableName + "(Id INT PRIMARY KEY," + entries + ')';
}
#endif

Object::Object(Parser *parser, const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment) : m_container(0), m_scope(0), m_library(0), m_id(parser->nextId()), m_parser(parser), m_identifier(identifier), m_sourceFile(sourceFile), m_line(line), m_docComment(docComment)
{
	if (docComment != 0)
		docComment->setObject(this);
}

Object::Object(Parser *parser) :  m_container(0), m_scope(0), m_library(0), m_sourceFile(0), m_docComment(0), m_parser(parser)
{
}

Object::~Object()
{
}

#ifdef SQLITE
std::size_t Object::sqlSize() const
{
	return 4;
}

Object::SqlColumn Object::sqlColumn() const
{
	SqlColumn result;
	result.reserve(sqlSize());

	return result;
}

Object::SqlColumn Object::sqlNames() const
{
	SqlColumn result = sqlColumn();
	result.push_back("Identifier");
	result.push_back("SourceFile");
	result.push_back("Line");
	result.push_back("DocComment");

	return result;
}

Object::SqlColumn Object::sqlTypes() const
{
	SqlColumn result = sqlColumn();
	result.push_back("VARCHAR(255)");
	result.push_back("INT");
	result.push_back("INT");
	result.push_back("INT");

	return result;
}

Object::SqlColumn Object::sqlValues() const
{
	SqlColumn result = sqlColumn();
	result.push_back(sqlFilteredString(this->identifier()));
	result.push_back(this->objectIdString(this->sourceFile()));
	result.push_back(boost::lexical_cast<std::string>(this->line()));
	result.push_back(this->objectIdString(this->docComment()));

	return result;
}

void Object::initBySql(const SqlColumn &column)
{
	this->m_id = boost::lexical_cast<Parser::IdType>(column[0]);
	this->m_identifier = column[1];
	this->m_line = boost::lexical_cast<Parser::IdType>(column[2]);
	const Parser::IdType sourceFileId = boost::lexical_cast<Parser::IdType>(column[3]);
	this->m_sourceFile = boost::polymorphic_cast<class SourceFile*>(this->parser()->searchObjectInLastDatabase(sourceFileId));
	const Parser::IdType docCommentId = boost::lexical_cast<Parser::IdType>(column[4]);
	this->m_docComment = boost::polymorphic_cast<class DocComment*>(this->parser()->searchObjectInLastDatabase(docCommentId));

	if (this->m_docComment != 0)
		this->m_docComment->setObject(this);
}

std::string Object::sqlStatement() const
{
	std::ostringstream sstream;

	for (std::size_t i = 0; i < sqlSize(); ++i)
	{
		sstream << sqlNames()[i] << "=" << sqlValues()[i];

		if (i < sqlSize() - 1)
			sstream << ",";
	}

	return sstream.str();
}
#endif

//empty condition methods
class Object* Object::container() const
{
	return m_container;
}

class Scope* Object::scope() const
{
	return m_scope;
}

class Library* Object::library() const
{
	return m_library;
}

bool Object::IsInContainer::operator()(const class Object *thisObject, const class Object *container) const
{
	return thisObject->container() == container;
}

bool Object::IsInScope::operator()(const class Object *thisObject, const class Object *scope) const
{
	return thisObject->scope() == static_cast<const class Scope*>(scope);
}

bool Object::IsInLibrary::operator()(const class Object *thisObject, const class Object *library) const
{
	return thisObject->library() == static_cast<const class Library*>(library);
}

bool Object::hasToSearchValueObject(class Object *type, const std::string &expression)
{
	if (type == 0)
		return false;

	if (dynamic_cast<class Type*>(type) != 0)
	{
		do
		{
			if (type == type->parser()->integerType())
			{
				if (isdigit(expression[0]) || expression[0] == '-' || expression[0] == '+')
					return false;

				return true;
			}
			else if (type == type->parser()->realType())
			{
				if (isdigit(expression[0]) || expression[0] == '-' || expression[0] == '+' || expression[0] == '.')
					return false;

				return true;
			}
			else if (type == type->parser()->stringType())
			{
				if (expression[0] == '\"' || expression == "null")
					return false;

				return true;
			}
			else if (type == type->parser()->booleanType())
			{
				if (expression == "true" || expression == "false")
					return false;

				return true;
			}
			else if (type == type->parser()->handleType() || type == type->parser()->codeType()) /// @todo Code type == null?!
			{
				if (expression == "null")
					return false;

				return true;
			}

			type = static_cast<class Type*>(type)->type();
		}
		while (type != 0);
	}
	else if (dynamic_cast<class FunctionInterface*>(type) != 0 || dynamic_cast<class Interface*>(type) != 0 || dynamic_cast<class Struct*>(type) != 0)
	{
		if (isdigit(expression[0]) || expression[0] == '-' || expression[0] == '+')
			return false;

		return true;
	}

	return true;
}

class Object* Object::findValue(class Object *type, std::string &valueExpression)
{
	class Object *value = 0;

	if (!valueExpression.empty())
	{
		//std::cout << "Before has to search." << std::endl;

		//FIXME The value can be a function or method call with literal arguments.
		if (Object::hasToSearchValueObject(type, valueExpression))
		{
			//std::cout << "Has to search!" << std::endl;
			//FIXME Detect . separators correctly.
			class Object *valueContainer = 0;
			std::string::size_type separatorPosition = valueExpression.find('.');

			if (separatorPosition != std::string::npos)
			{
				//std::cout << "Found . in value expression at position " << separatorPosition << std::endl;
				std::string containerIdentifier = valueExpression.substr(0, separatorPosition);
				//std::cout << "Container identifier: " << containerIdentifier << std::endl;

				if (containerIdentifier == File::expressionText[File::ThistypeExpression])
				{
					valueContainer = this->container();
					//std::cout << "thistype" << std::endl;
				}
				else if (containerIdentifier == File::expressionText[File::SuperExpression])
				{
					//FIXME Error? Test it!
					//valueContainer = static_cast<class Struct*>(this->container())->extension();
					//std::cout << "super" << std::endl;
				}

				//FIXME, does not work
				valueExpression.erase(separatorPosition);
				//std::cout << "New value expression " << valueExpression << std::endl;
				//std::cout << "New position " << separatorPosition << std::endl;
			}

			//FIXME Detect _ separators correctly?!

			bool functionCall = false;
			std::string::size_type position = valueExpression.find('(');

			if (position != std::string::npos)
				functionCall = true;
			else
				position = valueExpression.find('[');

			std::string newExpression;

			if (functionCall || position != std::string::npos)
			{
				newExpression = valueExpression.substr(0, position);

				if (!functionCall)
				{
					valueExpression = valueExpression.substr(position + 1);
					valueExpression =  valueExpression.substr(0, valueExpression.length() - 1); // FIXME Do not use two function calls.
				}
				else
					valueExpression = valueExpression.substr(position);
			}
			else
				newExpression = valueExpression;

			if (functionCall)
			{
				//std::cout << "Is function call " << std::endl;

				//methods only
				if (valueContainer != 0)
				{
					//std::cout << "with value container." << std::endl;
					Parser::SpecificObjectList list = parser()->getSpecificList<IsInContainer>(Parser::Methods, valueContainer);
					value = Parser::searchObjectInCustomList(list, newExpression);
				}
				//functions only
				else
				{
					//std::cout << "Has no container " << std::endl;
					value = this->parser()->searchObjectInList(newExpression, Parser::Functions, this);
					//std::cout << "Value = " << value << std::endl;
				}
			}
			else
			{
				// members only
				if (valueContainer != 0)
				{
					Parser::SpecificObjectList list = parser()->getSpecificList<IsInContainer>(Parser::Members, valueContainer);
					value = Parser::searchObjectInCustomList(list, newExpression);
				}
				// globals only
				else
					value = this->parser()->searchObjectInList(newExpression, Parser::Globals, this);
			}

			if (position == std::string::npos && value == 0)
				valueExpression.clear();
		}
		//else
			//std::cout << "has not to search" << std::endl;
	}
	else
		valueExpression = '-';

	return value;
}

std::string Object::anchor() const
{
	std::ostringstream sstream;

	if (parser()->parent()->optionPages())
		return this->pageLink();

	return this->m_identifier;
}

}
