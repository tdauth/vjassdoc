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

#include <cstdio>
#include <iostream>

#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

#include "vjassdoc.hpp"
#include "internationalisation.hpp"
#include "parser.hpp"
#include "objects.hpp"

namespace vjassdoc
{

Compiler::Compiler(Vjassdoc* parent) : m_parent(parent)
{
}

void Compiler::compile()
{
	std::fstream fstream(parent()->optionCompile().c_str());

	if (!fstream.good())
	{
		if (parent()->optionVerbose())
			fprintf(stderr, _("Error while opening or creating file \"%s\" for compilation process.\n"), parent()->optionCompile().c_str());

		return;
	}

	if (parent()->optionVerbose())
		fprintf(stderr, _("Opening or creating file \"%s\" for compilation process.\n"), parent()->optionCompile().c_str());

	fstream << "globals" << std::endl;
	this->writeGlobals(fstream);
	this->writeMembers(fstream);
	this->writeFunctionPrototypeGlobals(fstream);
	this->writeMethodPrototypeGlobals(fstream);
	fstream << "endglobals" << std::endl;

	fstream.close();
}

void Compiler::writeGlobals(std::ostream &fstream)
{
	if (parent()->optionVerbose())
		std::cout << _("Writing globals.") << std::endl;

	const Parser::ObjectList &list = parent()->parser()->getList(Parser::Globals);

	for (Parser::ObjectList::const_iterator iterator = list.begin(); iterator != list.end(); ++iterator)
	{
		const Global *global = boost::polymorphic_downcast<const Global*>(iterator->second);

		if (global->type() != 0)
			fstream << global->type()->identifier();
		else
			fstream << global->typeExpression();

		if (global->size() != 0 || global->sizeLiteral() != 0)
			fstream << " array ";

		if (global->library() != 0)
			fstream << global->library()->identifier() << "__";

		if (global->scope() != 0)
			fstream << global->scope()->identifier() << "__";

		fstream << global->identifier() << '\n';
	}
}

void Compiler::writeMembers(std::ostream &fstream)
{
	if (parent()->optionVerbose())
		std::cout << _("Writing members.") << std::endl;

	const Parser::ObjectList &list = parent()->parser()->getList(Parser::Members);

	for (Parser::ObjectList::const_iterator iterator = list.begin(); iterator != list.end(); ++iterator)
	{
		const Member *member = boost::polymorphic_downcast<const Member*>(iterator->second);

		if (member->isDelegate())
			continue;

		if (member->type() != 0)
		{
			// Check if type is an interface or struct and use integers for those types.
			if (dynamic_cast<class Interface*>(member->type()) != 0)
				fstream << "integer";
			else
				fstream << member->type()->identifier();
		}
		else
			fstream << member->typeExpression();

		if (member->size() != 0 || member->sizeLiteral() != 0 || !member->isStatic())
			fstream << " array ";

		if (member->library() != 0)
			fstream << member->library()->identifier() << "__";

		if (member->scope() != 0)
			fstream << member->scope()->identifier() << "__";

		fstream << member->container()->identifier() << "__";

		fstream << member->identifier() << '\n';
	}
}

void Compiler::writeFunctionPrototypeGlobals(std::ostream &fstream)
{
	if (parent()->optionVerbose())
		std::cout << _("Writing function prototype globals.") << std::endl;

	// check if there are .execute or .evaluate calls of a function.
}

void Compiler::writeMethodPrototypeGlobals(std::ostream &fstream)
{
	if (parent()->optionVerbose())
		std::cout << _("Writing method prototype globals.") << std::endl;

	// check if there are .execute or .evaluate calls of a method.
	// Note that .evaluate and .execute aren't used automatically like in vJass.
}

void Compiler::writeLibraries(std::ostream &fstream)
{
	if (parent()->optionVerbose())
		std::cout << _("Writing libraries.") << std::endl;

	// write all functions and methods and text macro instances in the right order.
	// text macros and preprocessors should be called before.
}

}
