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

#ifndef VJASSDOC_UTILITIES_HPP
#define VJASSDOC_UTILITIES_HPP

#include <string>
#include <ostream>
#include "internationalisation.hpp"

namespace vjassdoc
{

extern const char dirSeparator;
extern std::string getToken(const std::string &line, std::string::size_type &index, bool endOfLine = false);
extern void cutFilePath(std::string &filePath);
extern bool fileExists(const std::string &fileName);
extern void createHtmlHeader(std::ostream &ostream, const std::string &title, const std::string &language = _("en"));

}

#endif
