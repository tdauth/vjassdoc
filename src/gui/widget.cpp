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

#include <KMessageBox>
#include <KUrlCompletion>
#include <KFileDialog>

#include "widget.hpp"
#include "mainwindow.hpp"
#include "vjassdoc.hpp"

namespace vjassdoc
{

namespace gui
{

void Widget::run()
{
	if (this->filesEditListBox->items().isEmpty())
	{
		KMessageBox::error(this, tr("Missing file paths."), tr("Error"));

		return;
	}

	if (!QFile::exists(this->outputDirectoryUrlRequester->url().path()))
	{
		KMessageBox::error(this, tr("Missing output directory \"%1\".").arg(this->outputDirectoryUrlRequester->url().path()), tr("Error"));

		return;
	}


	bool parseObjectsOfList[vjassdoc::Parser::MaxLists];

	for (int i = 0; i < vjassdoc::Parser::MaxLists; ++i)
		parseObjectsOfList[i] = true;

	std::list<std::string> importDirs;

	foreach (QString iterator, this->importDirectoriesEditListBox->items())
		importDirs.push_back(iterator.toAscii().data()); //iterator.toStdString()

	std::list<std::string> filePaths;

	foreach (QString iterator, this->filesEditListBox->items())
		filePaths.push_back(iterator.toAscii().data()); //iterator.toStdString()

	std::list<std::string> databases;

	foreach (QString iterator, this->databasesEditListBox->items())
		databases.push_back(iterator.toAscii().data()); //iterator.toStdString()

	vjassdoc::Vjassdoc::configure(
	this->optionJassCheckBox->isChecked(),
	this->optionDebugCheckBox->isChecked(),
	this->optionPrivateCheckBox->isChecked(),
	this->optionTextmacrosCheckBox->isChecked(),
	this->optionFunctionsCheckBox->isChecked(),
	this->optionHtmlCheckBox->isChecked(),
	this->optionPagesCheckBox->isChecked(),
	this->optionSpecialpagesCheckBox->isChecked(),
	this->optionSyntaxCheckBox->isChecked(),
	this->compilationDirectoryUrlRequester->url().path().toUtf8().data(),
	this->databaseDirectoryUrlRequester->url().path().toUtf8().data(),
	this->optionVerboseCheckBox->isChecked(),
	this->optionTimeCheckBox->isChecked(),
	this->optionAlphabeticalCheckBox->isChecked(),
	parseObjectsOfList,
	this->titleLineEdit->text().toUtf8().data(),
	this->outputDirectoryUrlRequester->url().path().toUtf8().data(),
	importDirs,
	filePaths,
	databases);
#ifdef SQLITE
	Vjassdoc::initClasses();
#endif
	Vjassdoc::run();
	Vjassdoc::clear();
}

void Widget::selectFile()
{
	QString fileName = KFileDialog::getOpenFileName(KUrl(), tr("*|All Files\n*.j|JASS files"), this, tr("Select File"));

	if (!fileName.isEmpty())
		filesEditListBox->lineEdit()->setText(fileName);
}

void Widget::selectImportDirectory()
{
	QString fileName = KFileDialog::getExistingDirectory(KUrl(), this, tr("Select Import Directory"));

	if (!fileName.isEmpty())
		importDirectoriesEditListBox->lineEdit()->setText(fileName);
}

void Widget::selectDatabase()
{
	QString fileName = KFileDialog::getOpenFileName(KUrl(), tr("*|All Files"), this, tr("Select Database"));

	if (!fileName.isEmpty())
		databasesEditListBox->lineEdit()->setText(fileName);
}

Widget::Widget(class MainWindow *parent) : QWidget(parent)
{
	setupUi(this);
	connect(this->dialogButtonBox, SIGNAL(accepted()), this, SLOT(run()));
	connect(this->dialogButtonBox, SIGNAL(rejected()), parent, SLOT(close()));

	filesEditListBox->lineEdit()->setCompletionObject(new KUrlCompletion(KUrlCompletion::FileCompletion));
	importDirectoriesEditListBox->lineEdit()->setCompletionObject(new KUrlCompletion(KUrlCompletion::FileCompletion));
	databasesEditListBox->lineEdit()->setCompletionObject(new KUrlCompletion(KUrlCompletion::FileCompletion));

	connect(selectFilePushButton, SIGNAL(pressed()), this, SLOT(selectFile()));
	connect(selectImportDirectoryPushButton, SIGNAL(pressed()), this, SLOT(selectImportDirectory()));
	connect(selectDatabasePushButton, SIGNAL(pressed()), this, SLOT(selectDatabase()));



	//settings
	QSettings settings;
	settings.beginGroup("widget");
	titleLineEdit->setText(settings.value("title", tr("My API Documentation")).toString());
	QStringList files;
	int size = settings.beginReadArray("files");
	int i = 0;

	for ( ; i < size; ++i)
	{
		settings.setArrayIndex(i);
		files << settings.value("file").toString();
	}

	settings.endArray();
	this->filesEditListBox->setItems(files);
	this->optionJassCheckBox->setChecked(settings.value("jass", false).toBool());
	this->optionJassCheckBox->setChecked(settings.value("debug", true).toBool());
	this->optionJassCheckBox->setChecked(settings.value("private", false).toBool());
	this->optionJassCheckBox->setChecked(settings.value("textmacros", false).toBool());
	this->optionJassCheckBox->setChecked(settings.value("functions", false).toBool());
	this->optionJassCheckBox->setChecked(settings.value("html", true).toBool());
	this->optionJassCheckBox->setChecked(settings.value("pages", true).toBool());
	this->optionJassCheckBox->setChecked(settings.value("specialpages", true).toBool());
	this->optionJassCheckBox->setChecked(settings.value("syntax", false).toBool());
	this->optionJassCheckBox->setChecked(settings.value("verbose", false).toBool());
	this->optionJassCheckBox->setChecked(settings.value("time", true).toBool());
	this->optionJassCheckBox->setChecked(settings.value("alphabetical", true).toBool());
	this->outputDirectoryUrlRequester->setUrl(settings.value("dir").toUrl());
	this->compilationDirectoryUrlRequester->setUrl(settings.value("compile").toUrl());
	this->databaseDirectoryUrlRequester->setUrl(settings.value("database").toUrl());
	QStringList import;
	size = settings.beginReadArray("import");
	i = 0;

	for ( ; i < size; ++i)
	{
		settings.setArrayIndex(i);
		import << settings.value("file").toString();
	}

	settings.endArray();
	QStringList databases;
	size = settings.beginReadArray("databases");
	i = 0;

	for ( ; i < size; ++i)
	{
		settings.setArrayIndex(i);
		databases << settings.value("file").toString();
	}

	settings.endArray();
	settings.endGroup();
}

Widget::~Widget()
{
	//settings
	QSettings settings;
	settings.beginGroup("widget");
	settings.setValue("title", this->titleLineEdit->text());
	settings.beginWriteArray("files", this->filesEditListBox->items().size());
	int i = 0;

	foreach (QString iterator, this->filesEditListBox->items())
	{
		settings.setArrayIndex(i);
		settings.setValue("file", iterator);
		++i;
	}

	settings.endArray();
	settings.setValue("jass", this->optionJassCheckBox->isChecked());
	settings.setValue("debug", this->optionDebugCheckBox->isChecked());
	settings.setValue("private", this->optionPrivateCheckBox->isChecked());
	settings.setValue("textmacros", this->optionTextmacrosCheckBox->isChecked());
	settings.setValue("functions", this->optionFunctionsCheckBox->isChecked());
	settings.setValue("html", this->optionHtmlCheckBox->isChecked());
	settings.setValue("pages", this->optionPagesCheckBox->isChecked());
	settings.setValue("specialpages", this->optionSpecialpagesCheckBox->isChecked());
	settings.setValue("syntax", this->optionSyntaxCheckBox->isChecked());
	settings.setValue("verbose", this->optionVerboseCheckBox->isChecked());
	settings.setValue("time", this->optionTimeCheckBox->isChecked());
	settings.setValue("alphabetical", this->optionAlphabeticalCheckBox->isChecked());
	settings.setValue("dir", this->outputDirectoryUrlRequester->url());
	settings.setValue("compile", this->compilationDirectoryUrlRequester->url());
	settings.setValue("database", this->databaseDirectoryUrlRequester->url());
	settings.beginWriteArray("import", this->importDirectoriesEditListBox->items().size());
	i = 0;

	foreach (QString iterator, this->importDirectoriesEditListBox->items())
	{
		settings.setArrayIndex(i);
		settings.setValue("file", iterator);
		++i;
	}

	settings.endArray();
	settings.beginWriteArray("databases", this->databasesEditListBox->items().size());
	i = 0;

	foreach (QString iterator, this->databasesEditListBox->items())
	{
		settings.setArrayIndex(i);
		settings.setValue("file", iterator);
		++i;
	}

	settings.endArray();
	settings.endGroup();
}

}

}
