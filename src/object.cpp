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
#include <iostream> //debug

#include "objects.hpp"
#include "sourcefile.hpp"
#include "doccomment.hpp"
#include "vjassdoc.hpp"
#include "internationalisation.hpp"
#include "file.hpp"

namespace vjassdoc
{

unsigned int Object::sqlColumns;
std::string Object::sqlColumnStatement;
Object::IdType Object::m_maxIds = 0;

void Object::initClass()
{
	Object::sqlColumns = 4;
	Object::sqlColumnStatement =
	"Identifier VARCHAR(255),"
	"SourceFile INT,"
	"Line INT,"
	"DocComment INT";
}

#ifdef SQLITE
std::string Object::sqlFilteredString(const std::string &usedString)
{
	std::string result;
	
	for (std::string::size_type i = 0; i < usedString.length(); ++i)
	{
		char character = usedString[i];
		
		switch (character)
		{
			case '\'':
				result += "\'\'";
				break;
			
			case '\"':
				result += "\"\"";
				break;
			
			default:
				result += character;
				break;
		}
	}

	return result;
}

std::string Object::sqlTableHeader(const std::string &tableName, const std::string &entries)
{
	return "CREATE TABLE " + tableName + "(Id INT PRIMARY KEY," + entries + ')';
}
#endif

Object::Object(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment) : m_container(0), m_scope(0), m_library(0), m_id(m_maxIds), m_identifier(identifier), m_sourceFile(sourceFile), m_line(line), m_docComment(docComment)
{
	++m_maxIds;
	
	if (docComment != 0)
		docComment->setObject(this);
}

#ifdef SQLITE
Object::Object(std::vector<Object::VectorDataType> &columnVector) :  m_container(0), m_scope(0), m_library(0), m_sourceFile(0), m_docComment(0), m_columnVector(columnVector)
{
}
#endif

Object::~Object()
{
}

#ifdef SQLITE
void Object::initByVector()
{
	this->m_sourceFile = static_cast<class SourceFile*>(Vjassdoc::parser()->searchObjectInLastDatabase(atoi((const char*)this->m_columnVector[0])));
	this->m_docComment = static_cast<class DocComment*>(Vjassdoc::parser()->searchObjectInLastDatabase(atoi((const char*)this->m_columnVector[1])));
	
	//drop elements
	
	if (this->m_docComment != 0)
		this->m_docComment->setObject(this);
}

void Object::clearVector()
{
	this->m_columnVector.clear();
}

std::string Object::sqlStatement() const
{
	std::ostringstream sstream;
	sstream
	<< "Identifier=\"" << Object::sqlFilteredString(this->identifier()) << "\", "
	<< "SourceFile=" << Object::objectId(this->sourceFile()) << ", "
	<< "Line=" << this->line() << ", "
	<< "DocComment=" << Object::objectId(this->docComment());

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
			if (type == Vjassdoc::parser()->integerType())
			{
				if (isdigit(expression[0]) || expression[0] == '-' || expression[0] == '+')
					return false;
				
				return true;
			}
			else if (type == Vjassdoc::parser()->realType())
			{
				if (isdigit(expression[0]) || expression[0] == '-' || expression[0] == '+' || expression[0] == '.')
					return false;
				
				return true;
			}
			else if (type == Vjassdoc::parser()->stringType())
			{
				if (expression[0] == '\"' || expression == "null")
					return false;
				
				return true;
			}
			else if (type == Vjassdoc::parser()->booleanType())
			{
				if (expression == "true" || expression == "false")
					return false;
				
				return true;
			}
			else if (type == Vjassdoc::parser()->handleType() || type == Vjassdoc::parser()->codeType()) /// @todo Code type == null?!
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
					std::cout << "super" << std::endl;
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
					std::list<class Object*> list = Vjassdoc::parser()->getSpecificList(Parser::Methods, Object::IsInContainer(), valueContainer);
					value = Parser::searchObjectInCustomList(list, newExpression);
				}
				//functions only
				else
				{
					//std::cout << "Has no container " << std::endl;
					value = this->searchObjectInList(newExpression, Parser::Functions);
					//std::cout << "Value = " << value << std::endl;
				}
			}
			else
			{
				//members only
				if (valueContainer != 0)
				{

					std::list<class Object*> list = Vjassdoc::parser()->getSpecificList(Parser::Members, Object::IsInContainer(), valueContainer);
					value = Parser::searchObjectInCustomList(list, newExpression);
				}
				//globals only
				else
					value = this->searchObjectInList(newExpression, Parser::Globals);
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

#ifdef SQLITE
void Object::prepareVector()
{
	this->m_id = atoi((const char*)this->m_columnVector.front());
	this->m_columnVector.erase(this->m_columnVector.begin()); //drop element
	
	this->m_identifier = (const char*)(this->m_columnVector.front());
	this->m_columnVector.erase(this->m_columnVector.begin()); //drop element
	
	this->m_line = atoi((const char*)this->m_columnVector[1]);
	this->m_columnVector.erase(this->m_columnVector.begin() + 1); //drop element
}
#endif

}
