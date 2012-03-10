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

#ifndef VJASSDOC_GUI_WIDGET_HPP
#define VJASSDOC_GUI_WIDGET_HPP

#include <QWidget>

#include <KAboutData>
#include <KAboutApplicationDialog>

#include "ui_widget.h"
#include "../vjassdoc.hpp"

namespace vjassdoc
{

namespace gui
{

class Widget : public QWidget, Ui::Widget
{
	Q_OBJECT

	public slots:
		void run();
		void selectFile();
		void selectImportDirectory();
		void selectDatabase();
		void showAbout();

	public:
		Widget(const KAboutData &about, class MainWindow *parent);
		~Widget();

		virtual void readSettings();
		virtual void writeSettings();

	private:
		KAboutData m_about;
		KAboutApplicationDialog *m_aboutDialog;
		QScopedPointer<vjassdoc::Vjassdoc> m_app;
};

}

}

#endif
