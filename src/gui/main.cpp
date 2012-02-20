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

#include <KApplication>
#include <KCmdLineArgs>

#include "application.hpp"
#include "mainwindow.hpp"

int main(int argc, char *argv[])
{
	KAboutData aboutData(
		"vjassdocgui",
		0,
		ki18n("vjassdocgui"),
		"0.1",
		ki18n("GUI for vjassdoc."),
		KAboutData::License_GPL_V2,
		ki18n("Copyright 2009, Tamino Dauth"),
		KLocalizedString(),
		"https://gitorious.org/wc3lib/vjassdoc"
	);
	aboutData.addAuthor(ki18n("Tamino Dauth"), KLocalizedString(), "tamino@cdauth.de");
	//aboutData.setOrganizationDomain("https://gitorious.org/wc3lib/vjassdoc");

	KCmdLineArgs::init(argc, argv, &aboutData);

	KCmdLineOptions options;
	KCmdLineArgs::addCmdLineOptions(options);

	vjassdoc::gui::Application app;
	class vjassdoc::gui::MainWindow *mainWindow = new vjassdoc::gui::MainWindow();
	mainWindow->show();

	return app.exec();
}
