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

#ifndef VJASSDOC_FILE_HPP
#define VJASSDOC_FILE_HPP

#include <list>
#include <string>
#include <fstream>

#include "library.hpp"

namespace vjassdoc
{

class Parser;
class SourceFile;
class DocComment;
class Library;
class Scope;
class Interface;
class Struct;
class Module;
class Function;

/**
* A Jass or vJass code file which is treated by a @class Parser instance.
* This class knows the whole Jass and vJass syntax and is able to parse it.
* Parsing runs automatically after creation of an instance in constructor.
*/
class File
{
	public:
		enum Expression
		{
			DocCommentExpression, //This tool
			CommentExpression,
			BlockDocCommentExpression, //has to be checked before block comment expression!
			EndBlockDocCommentExpression,
			TypeExpression,
			ConstantExpression,
			NativeExpression,
			FunctionExpression,
			EndfunctionExpression,
			GlobalsExpression,
			EndglobalsExpression,
			LocalExpression,
			SetExpression,
			CallExpression,
			IfExpression,
			ElseExpression,
			ElseifExpression,
			EndifExpression,
			ReturnExpression,
			LoopExpression,
			EndloopExpression,
			ExitwhenExpression,
			//vJass stand-alone expressions
			PreprocessorExpression,
			MethodExpression, //Operator is not required because the syntax is "method operator..."
			EndmethodExpression, //Required for function blocks, there can not be custom types
			PrivateExpression,
			PublicExpression,
			StaticExpression,
			DebugExpression,
			KeywordExpression,
			LibraryExpression,
			Library_onceExpression,
			EndlibraryExpression,
			ScopeExpression,
			EndscopeExpression,
			StructExpression,
			EndstructExpression,
			InterfaceExpression,
			EndinterfaceExpression,
			DelegateExpression,
			StubExpression,
			ModuleExpression,
			EndmoduleExpression,
			ImplementExpression,
			HookExpression,
			BlockCommentExpression,
			EndBlockCommentExpression,
			KeyExpression,
			//Jass none-start-line expressions
			NothingExpression,
			TakesExpression,
			ReturnsExpression,
			ExtendsExpression,
			ArrayExpression,
			//vJass none-start-line expressions
			ImportExpression,
			DovjassinitExpression,
			InjectExpression,
			EndinjectExpression,
			NovjassExpression,
			EndnovjassExpression,
			LoaddataExpression,
			ExternalExpression,
			TextmacroExpression,
			TextmacroonceExpression,
			EndtextmacroExpression,
			RuntextmacroExpression, //An instance of the macro
			RequiresExpression,
			NeedsExpression,
			UsesExpression,
			InitializerExpression,
			DefaultsExpression,
			OptionalExpression,
			SuperExpression,
			ThistypeExpression,
			OperatorExpression,
			ExecuteExpression,
			EvaluateExpression,
			MaxExpressions,
			NoExpression,
			InvalidExpression,
			CustomExpression
		};

		static const char *expressionText[File::MaxExpressions];

		File();
		/**
		  * @return Returns number of parsed lines.
		*/
		std::string::size_type parse(class Parser *parser, std::ifstream &ifstream);

	private:
		File::Expression getFirstLineExpression(std::string &line, std::string::size_type &index);
		void truncateComments(std::string &line, std::string::size_type index);
		void getDocComment(const std::string &line, std::string::size_type index);
		void getBlockDocComment(const std::string &line);
		void clearDocComment();
		void getKeyword(const std::string &line, std::string::size_type &index, bool isPrivate);
		void getKey(const std::string &line, std::string::size_type &index, bool isPrivate);
		bool getGlobal(const std::string &line, std::string::size_type &index, bool isPrivate, bool isPublic, bool isConstant, bool isStatic, bool isDelegate);
		bool getFunction(const std::string  &line, std::string::size_type &index, bool isPrivate, bool isPublic, bool isConstant, bool isNative, bool isStatic, bool isStub);
		void getCall(const std::string &line, std::string::size_type &index);
		void getImplementation(const std::string &line, std::string::size_type &index);
		void getHook(const std::string &line, std::string::size_type &index);
		void getLibrary(const std::string &line, std::string::size_type &index, bool isOnce);
		void getScope(const std::string &line, std::string::size_type &index, bool isPrivate);
		void getInterface(const std::string &line, std::string::size_type &index, bool isPrivate);
		void getStruct(const std::string &line, std::string::size_type &index, bool isPrivate);
		void getModule(const std::string &line, std::string::size_type &index, bool isPrivate);
		void getLocal(const std::string &line, std::string::size_type &index);
		std::string removeFirstSpace(const std::string &line, std::string::size_type index) const;
		void getTextMacro(const std::string &line, std::string::size_type &index, bool isOnce);
		void getTextMacroInstance(const std::string &line, std::string::size_type &index);
		Library::RequirementsContainer* getLibraryRequirement(const std::string &line, std::string::size_type &index) const;
		bool isInVjassBlock() const;
		class Object* getCurrentContainer() const;

		class Parser *m_parser;
		File::Expression m_notRequiredSpace;
		bool m_isInGlobals;
		bool m_isInLibrary;
		bool m_isInScope;
		bool m_isInInterface;
		bool m_isInStruct;
		bool m_isInModule;
		bool m_isInBlockComment;
		bool m_isInBlockDocComment;

		std::string::size_type m_currentLine;
		class DocComment *m_currentDocComment;
		class Library *m_currentLibrary;
		class Scope *m_currentScope;
		//containers
		class Interface *m_currentInterface;
		class Struct *m_currentStruct;
		class Module *m_currentModule;
		class Function *m_currentFunction;
		std::string m_currentBlockComment;
		std::string m_currentBlockDocComment;

		bool m_gotDocComment;
};

}

#endif
