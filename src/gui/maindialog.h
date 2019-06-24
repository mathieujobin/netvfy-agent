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

#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDesktopWidget>
#include <QDialog>
#include <QMessageBox>
#include <QSize>
#include <QString>
#include <QSystemTrayIcon>

#include "ui_maindialog.h"

class AccountSettings;
class LogSettings;
class GeneralSettings;

class MainDialog: public QDialog
{
	Q_OBJECT

	public:
		MainDialog();
		virtual ~MainDialog();

		/* Interface between C backend and Qt GUI */
		static void onLog(const char *);
		static void onConnect(const char *ip);
		static void onDisconnect();
		static void onListNetworks(const char *network);

	public slots:
		void slotExit();
		void slotToggleAutoConnect(int);
		void slotAddNetwork();
		void slotDeleteNetwork();
		void slotResetNetworkList();
		void slotFireConnection();
		void slotResetAccount();
		void trayIconClicked(QSystemTrayIcon::ActivationReason);

	private:
		Ui::MainDialog ui;

		AccountSettings *accountSettings;
		LogSettings *logSettings;
		GeneralSettings *generalSettings;

		QString ProvKey;
		QSystemTrayIcon *trayIcon;
	
		void NowRun();	
		void createTrayIcon();
		void setTrayIcon();

		/* Override the window's close event */
		void closeEvent(QCloseEvent *);
		void centerWidget(QWidget *w);
};

#endif // MAINDIALOG
