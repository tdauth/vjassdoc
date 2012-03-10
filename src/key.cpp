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

#include "objects.hpp"

namespace vjassdoc
{

Key::Key(Parser *parser, const std::string &identifier, SourceFile *sourceFile, unsigned int line, DocComment *docComment, Library *library, Scope *scope, bool isPrivate) : Keyword(parser, identifier, sourceFile, line, docComment, library, scope, isPrivate)
{

}

Key::Key(Parser *parser) : Keyword(parser)
{

}

#ifdef SQLITE
const char* Key::sqlTableName() const
{
	return "Keys";
}
#endif

}
