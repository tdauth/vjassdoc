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

#ifndef VJASSDOC_COMPILER_HPP
#define VJASSDOC_COMPILER_HPP

#include <fstream>

#include <list>

namespace vjassdoc
{

class FunctionPrototype;

class Compiler
{
	public:
		Compiler(class Vjassdoc *parent);

		class Vjassdoc* parent() const;

		void compile();

	private:
		void writeGlobals(std::ostream &fstream);
		void writeMembers(std::ostream &fstream);
		/// Writes triggers of methods for TriggerEvaluate() and TriggerExecute().
		void writeFunctionPrototypeGlobals(std::ostream &fstream);
		void writeMethodPrototypeGlobals(std::ostream &fstream);
		void writeLibraries(std::ostream &fstream);

		class Vjassdoc *m_parent;
		std::list<class FunctionPrototype*> m_prototypes;
};

inline Vjassdoc* Compiler::parent() const
{
	return this->m_parent;
}

}

#endif
