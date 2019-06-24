/*
 * NetVirt - Network Virtualization Platform
 * Copyright (C) 2009-2014
 * Nicolas J. Bouliane <admin@netvirt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 3
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <QFile>
#include <QLabel>
#include <QDebug>

#include "maindialog.h"
#include "accountsettings.h"
#include "logsettings.h"
#include "generalsettings.h"

/* Hack to access this from static method */
static void *obj_this;

MainDialog::MainDialog()
{
	NowRun();
	this->raise();
}

MainDialog::~MainDialog()
{
}

void MainDialog::NowRun()
{
	centerWidget(this);
	this->show();

	obj_this = this;

	ui.setupUi(this);
	QIcon accountIcon(QLatin1String(":rc/user.png"));
	QListWidgetItem *account = new QListWidgetItem(accountIcon, "Account", ui.labelWidget);
	account->setSizeHint(QSize(0, 32));
	this->accountSettings = new AccountSettings(this);
	ui.stack->addWidget(this->accountSettings);

	QIcon syslogIcon(QLatin1String(":rc/loop_alt4.png"));
	QListWidgetItem *syslog = new QListWidgetItem(syslogIcon, "Log Activity", ui.labelWidget);
	syslog->setSizeHint(QSize(0, 32));
	this->logSettings = new LogSettings;
	ui.stack->addWidget(this->logSettings);

	QIcon generalIcon(QLatin1String(":rc/cog.png"));
	QListWidgetItem *general = new QListWidgetItem(generalIcon, "General", ui.labelWidget);
	general->setSizeHint(QSize(0, 32));
	this->generalSettings = new GeneralSettings(this);
	ui.stack->addWidget(this->generalSettings);

	ui.labelWidget->setCurrentRow(ui.labelWidget->row(account));

	connect(ui.labelWidget, SIGNAL(currentRowChanged(int)),
		ui.stack, SLOT(setCurrentIndex(int)));

	connect(this->ui.exitButton, SIGNAL(clicked()), this, SLOT(slotExit()));

	createTrayIcon();
	setTrayIcon();
	trayIcon->show();	
}

void MainDialog::slotToggleAutoConnect(int checked)
{
}

void MainDialog::slotFireConnection(void)
{
}

void MainDialog::slotResetAccount()
{
	QMessageBox::StandardButton reply;
	reply = QMessageBox::warning(this, "NetVirt Agent", "Exit now, and manually restart me.",
					QMessageBox::Yes|QMessageBox::No);

	if (reply == QMessageBox::Yes) {
		qApp->quit();
	}
}

void MainDialog::slotExit()
{
	QMessageBox::StandardButton reply;
	reply = QMessageBox::warning(this, "NetVirt Agent", "Are you sure you want to exit ?",
					QMessageBox::Yes|QMessageBox::No);

	if (reply == QMessageBox::Yes) {
		trayIcon->setVisible(false);
		qApp->quit();
	}
}

void MainDialog::onLog(const char *logline)
{
	MainDialog *_this = static_cast<MainDialog*>(obj_this);	
	QMetaObject::invokeMethod(_this->logSettings, "slotUpdateLog",
	Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(logline)));
}

void MainDialog::onConnect(const char *ip)
{
	MainDialog *_this = static_cast<MainDialog*>(obj_this);	
	QMetaObject::invokeMethod(_this->accountSettings, "slotOnConnect",
	Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(ip)));
}

void MainDialog::onDisconnect()
{
	MainDialog *_this = static_cast<MainDialog*>(obj_this);	
	QMetaObject::invokeMethod(_this->accountSettings, "slotConnWaiting",
	Qt::QueuedConnection);
}

void MainDialog::createTrayIcon()
{
	trayIcon = new QSystemTrayIcon(this);

	connect(trayIcon,
		SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this,
		SLOT(trayIconClicked(QSystemTrayIcon::ActivationReason))
	);
}

void MainDialog::setTrayIcon()
{
	trayIcon->setIcon(QIcon(":rc/share.png"));
}

void MainDialog::trayIconClicked(QSystemTrayIcon::ActivationReason reason)
{
	(void)(reason); /* unused */

	if (this->isVisible())
		this->hide();
	else {
		centerWidget(this);
		this->show();
	}
}

void MainDialog::closeEvent(QCloseEvent *event)
{
	event->ignore();
	this->isVisible();
	this->hide();
}

void MainDialog::centerWidget(QWidget *w)
{
	if (w->isFullScreen())
		return;

	QSize size;
	size = w->size();

	QDesktopWidget *d = QApplication::desktop();
	int ws = d->width();
	int h = d->height();
	int mw = size.width();
	int mh = size.height();
	int cw = (ws/2) - (mw/2);
	int ch = (h/2) - (mh/2);
	w->move(cw,ch);
}

