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

#ifndef VJASSDOC_SYNTAXERROR_HPP
#define VJASSDOC_SYNTAXERROR_HPP

#include <string>

#include "sourcefile.hpp"

namespace vjassdoc
{

class SyntaxError
{
	public:
		static bool Comparator(class SyntaxError *syntaxError0, class SyntaxError *syntaxError1);

		SyntaxError(class SourceFile *sourceFile, std::size_t line, const std::string &message);
		class SourceFile* sourceFile() const;
		unsigned int line() const;
		std::string message() const;

	private:
		class SourceFile *m_sourceFile;
		std::size_t m_line;
		std::string m_message;
};

inline bool SyntaxError::Comparator(class SyntaxError *syntaxError0, class SyntaxError *syntaxError1)
{
	return syntaxError0->sourceFile()->identifier() < syntaxError1->sourceFile()->identifier();
}

inline class SourceFile* SyntaxError::sourceFile() const
{
	return this->m_sourceFile;
}

inline unsigned int SyntaxError::line() const
{
	return this->m_line;
}

inline std::string SyntaxError::message() const
{
	return this->m_message;
}

}

#endif
