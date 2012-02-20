/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#include <QtGui>
#include <list>
#include <string>

#include "mainwindow.hpp"
#include "widget.hpp"

namespace vjassdoc
{

namespace gui
{

const KAboutData *MainWindow::aboutData = new KAboutData("vjassdocgui",
"",
ki18n("vjassdocgui"),
"0.1",
ki18n("Graphical user interface for vjassdoc."),
KAboutData::License_GPL_V2,
ki18n("(C) 2009 Tamino Dauth"),
ki18n("Bla bla bla"),
"http://sourceforge.net/projects/vjasssdk/");

MainWindow::MainWindow(QWidget *parent) : KMainWindow(parent), m_widget(new Widget(this))
{
	//this->m_widget->setupUi(this);
	this->setCentralWidget(this->m_widget);
}

MainWindow::~MainWindow()
{
}

}

}
