/* Copyright 2016 Mikhail Kita
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "programmingWidget.h"

#include <QtGui/QKeyEvent>

#include <trikKernel/paths.h>
#include <QsLog.h>

#include "commandsListWidget.h"

using namespace trikGui;

ProgrammingWidget::ProgrammingWidget(Controller &controller, QWidget *parent)
	: TrikGuiDialog(parent)
	, mTitle(tr("Add commands to list"))
	, mController(controller)
	, mEmptyCommandsCounter(1)
	, mScript(script.c_str())
{
	mLayout.addWidget(&mTitle);

	mCommands.addItem(tr("< add command >"));
	mCommands.selectionModel()->select(
		mCommands.model()->index(0, 0)
		, QItemSelectionModel::ClearAndSelect
	);
	mCommands.setFocus();
	mLayout.addWidget(&mCommands);

	mRunButton.setText(tr("Run program"));
	mRunButton.setAutoFillBackground(true);
	mLayout.addWidget(&mRunButton);

	connect(&mRunButton, SIGNAL(upPressed()), this, SLOT(focus()));
	connect(&mRunButton, SIGNAL(downPressed()), this, SLOT(focus()));

	connect(&mCommands, SIGNAL(upPressed()), this, SLOT(focus()));
	connect(&mCommands, SIGNAL(downPressed()), this, SLOT(focus()));

	setLayout(&mLayout);
}

QString ProgrammingWidget::menuEntry()
{
	return tr("Programming");
}

void ProgrammingWidget::renewFocus()
{
	mCommands.setFocus();
}

void ProgrammingWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_PowerDown: {
		mController.abortExecution();
		break;
	}
	case Qt::Key_Up: {
		focus();
		break;
	}
	case Qt::Key_Down: {
		focus();
		break;
	}
	case Qt::Key_Return: {
		if (mCommands.hasFocus()) {
			addCommand();
		} else if (mRunButton.hasFocus()) {
			mRunButton.animateClick();
			QString temp = mScript;
			temp.append(QString("    return;\n}"));
			mController.runScript(temp);
		}
		break;
	}
	default: {
		TrikGuiDialog::keyPressEvent(event);
		break;
	}
	}
}

void ProgrammingWidget::addCommand()
{
	QString text(mCommands.currentItem()->text());
	CommandsListWidget commandsListWidget(mController, text);
	emit newWidget(commandsListWidget);
	commandsListWidget.exec();

	QString value(commandsListWidget.value());
	mCommands.currentItem()->setText(value);

	if (text == tr("< add command >") && value != text) {
		mEmptyCommandsCounter--;
	}

	if (mEmptyCommandsCounter == 0) {
		mCommands.addItem(tr("< add command >"));
		mEmptyCommandsCounter++;
		mScript.append(commandsListWidget.script());
	}
}

void ProgrammingWidget::focus()
{
	if (mCommands.hasFocus()) {
		mRunButton.setFocus();
	} else {
		mCommands.setFocus();
	}
}
