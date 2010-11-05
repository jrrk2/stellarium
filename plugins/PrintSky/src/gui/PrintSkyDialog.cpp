/*
 * Copyright (C) 2010 Pep Pujols
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "PrintSkyDialog.hpp"
#include "ui_printskyDialog.h"
#include "PrintSky.hpp"
#include <QAction>
#include <QGLWidget>
#include <QPrintDialog>
#include <QTimer>
#include <QGraphicsWidget>

#include "StelApp.hpp"
#include "StelGui.hpp"
#include "StelFileMgr.hpp"
#include "StelModuleMgr.hpp"
#include "StelMainGraphicsView.hpp"
#include "StelTranslator.hpp"
#include "StelNavigator.hpp"
#include "StelCore.hpp"
#include "StelLocaleMgr.hpp"
#include "StelMovementMgr.hpp"
#include "StelSkyDrawer.hpp"
#include "StarMgr.hpp"
#include "SolarSystem.hpp"
#include "Planet.hpp"

#include <QDataWidgetMapper>
#include <QDebug>
#include <QFrame>
#include <QSettings>


PrintSkyDialog::PrintSkyDialog()
{
	ui = new Ui_printskyDialogForm;
}

PrintSkyDialog::~PrintSkyDialog()
{
	//These exist only if the window has been shown once:
	if (dialog)
	{
	}

	delete ui;
	ui = NULL;

}

/* ********************************************************************* */
#if 0
#pragma mark -
#pragma mark StelModule Methods
#endif
/* ********************************************************************* */
void PrintSkyDialog::languageChanged()
{
	if (dialog) {
		ui->retranslateUi(dialog);
	}
}

void PrintSkyDialog::updateStyle()
{
	if(dialog) {
		gui = dynamic_cast<StelGui*>(StelApp::getInstance().getGui());
		Q_ASSERT(gui);
		const StelStyle pluginStyle = GETSTELMODULE(PrintSky)->getModuleStyleSheet(gui->getStelStyle());
		dialog->setStyleSheet(pluginStyle.qtStyleSheet);
		//ui->textBrowser->document()->setDefaultStyleSheet(QString(pluginStyle.htmlStyleSheet));
	}
}

void PrintSkyDialog::styleChanged()
{
	// Nothing for now
}

/* ********************************************************************* */
#if 0
#pragma mark -
#pragma mark Slot Methods
#endif
/* ********************************************************************* */
void PrintSkyDialog::closeWindow()
{
	setVisible(false);
	StelMainGraphicsView::getInstance().scene()->setActiveWindow(0);
}


/* ********************************************************************* */
#if 0
#pragma mark -
#pragma mark Private Slot Methods
#endif
/* ********************************************************************* */
void PrintSkyDialog::invertColorsStateChanged(int state)
{
	bool shouldInvert = (state == Qt::Checked);

	try
	{
		StelFileMgr::Flags flags = (StelFileMgr::Flags)(StelFileMgr::Directory|StelFileMgr::Writable);
		QString printskyIniPath = StelFileMgr::findFile("modules/PrintSky/", flags) + "printsky.ini";
		QSettings settings(printskyIniPath, QSettings::IniFormat);
		bool useInvertColors = settings.value("use_invert_colors", 0.0).toBool();
		if (state != useInvertColors)
		{
			settings.setValue("use_invert_colors", shouldInvert);
			emit(invertColorsChanged(shouldInvert));
		}
	}
	catch (std::runtime_error& e)
	{
		qWarning() << "WARNING: unable to locate printsky.ini file or create a default one for PrintSky plugin: " << e.what();
	}

}

/* ********************************************************************* */
#if 0
#pragma mark -
#pragma mark Private Slot Methods
#endif
/* ********************************************************************* */
void PrintSkyDialog::scaleToFitStateChanged(int state)
{
	bool shouldScale = (state == Qt::Checked);

	try
	{
		StelFileMgr::Flags flags = (StelFileMgr::Flags)(StelFileMgr::Directory|StelFileMgr::Writable);
		QString printskyIniPath = StelFileMgr::findFile("modules/PrintSky/", flags) + "printsky.ini";
		QSettings settings(printskyIniPath, QSettings::IniFormat);
		bool useScaleToFit = settings.value("use_scale_to_fit", 0.0).toBool();
		if (state != useScaleToFit)
		{
			settings.setValue("use_scale_to_fit", shouldScale);
			emit(scaleToFitChanged(shouldScale));
		}
	}
	catch (std::runtime_error& e)
	{
		qWarning() << "WARNING: unable to locate printsky.ini file or create a default one for PrintSky plugin: " << e.what();
	}

}

/* ********************************************************************* */
#if 0
#pragma mark -
#pragma mark Private Slot Methods
#endif
/* ********************************************************************* */
void PrintSkyDialog::orientationStateChanged(bool state)
{
	QString newOrientation=(ui->orientationPortraitRadioButton->isChecked()? "Portrait": "Landscape");

	try
	{
		StelFileMgr::Flags flags = (StelFileMgr::Flags)(StelFileMgr::Directory|StelFileMgr::Writable);
		QString printskyIniPath = StelFileMgr::findFile("modules/PrintSky/", flags) + "printsky.ini";
		QSettings settings(printskyIniPath, QSettings::IniFormat);
		QString currentOrientation = settings.value("orientation", "Portrait").toString();
		if (newOrientation != currentOrientation)
		{
			settings.setValue("orientation", newOrientation);
			emit(orientationChanged(true));
		}
	}
	catch (std::runtime_error& e)
	{
		qWarning() << "WARNING: unable to locate printsky.ini file or create a default one for PrintSky plugin: " << e.what();
	}

}

/* ********************************************************************* */
#if 0
#pragma mark -
#pragma mark Private Slot Methods
#endif
/* ********************************************************************* */
void PrintSkyDialog::printDataStateChanged(int state)
{
	bool shouldPrint = (state == Qt::Checked);

	try
	{
		StelFileMgr::Flags flags = (StelFileMgr::Flags)(StelFileMgr::Directory|StelFileMgr::Writable);
		QString printskyIniPath = StelFileMgr::findFile("modules/PrintSky/", flags) + "printsky.ini";
		QSettings settings(printskyIniPath, QSettings::IniFormat);
		bool printData = settings.value("print_data", 0.0).toBool();
		if (state != printData)
		{
			settings.setValue("print_data", shouldPrint);
			emit(printDataChanged(shouldPrint));
		}
	}
	catch (std::runtime_error& e)
	{
		qWarning() << "WARNING: unable to locate printsky.ini file or create a default one for PrintSky plugin: " << e.what();
	}

}

/* ********************************************************************* */
#if 0
#pragma mark -
#pragma mark Private Slot Methods
#endif
/* ********************************************************************* */
void PrintSkyDialog::printSSEphemeridesStateChanged(int state)
{
	bool shouldPrint = (state == Qt::Checked);

	try
	{
		StelFileMgr::Flags flags = (StelFileMgr::Flags)(StelFileMgr::Directory|StelFileMgr::Writable);
		QString printskyIniPath = StelFileMgr::findFile("modules/PrintSky/", flags) + "printsky.ini";
		QSettings settings(printskyIniPath, QSettings::IniFormat);
		bool printSSEphemerides = settings.value("print_SS_ephemerides", 0.0).toBool();
		if (state != printSSEphemerides)
		{
			settings.setValue("print_SS_ephemerides", shouldPrint);
			emit(printSSEphemeridesChanged(shouldPrint));
		}
	}
	catch (std::runtime_error& e)
	{
		qWarning() << "WARNING: unable to locate printsky.ini file or create a default one for PrintSky plugin: " << e.what();
	}

}


/* ********************************************************************* */
#if 0
#pragma mark -
#pragma mark Protected Methods
#endif
/* ********************************************************************* */
void PrintSkyDialog::createDialogContent()
{

	ui->setupUi(dialog);

	//Now the rest of the actions.
	connect(ui->closeStelWindow, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui->invertColorsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(invertColorsStateChanged(int)));
	connect(ui->scaleToFitCheckBox, SIGNAL(stateChanged(int)), this, SLOT(scaleToFitStateChanged(int)));
	connect(ui->previewSkyPushButton, SIGNAL(clicked()), this, SLOT(previewSky()));
	connect(ui->printSkyPushButton, SIGNAL(clicked()), this, SLOT(printSky()));
	connect(ui->orientationPortraitRadioButton, SIGNAL(toggled(bool)), this, SLOT(orientationStateChanged(bool)));
	connect(ui->printDataCheckBox, SIGNAL(stateChanged(int)), this, SLOT(printDataStateChanged(int)));
	connect(ui->printSSEphemeridesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(printSSEphemeridesStateChanged(int)));

	// set the initial state
	try
		{
		StelFileMgr::Flags flags = (StelFileMgr::Flags)(StelFileMgr::Directory|StelFileMgr::Writable);
		QString printskyIniPath = StelFileMgr::findFile("modules/PrintSky/", flags) + "printsky.ini";
		QSettings settings(printskyIniPath, QSettings::IniFormat);
		bool useInvertColors = settings.value("use_invert_colors", false).toBool();
		if (useInvertColors)
		{
			ui->invertColorsCheckBox->setCheckState(Qt::Checked);
		}
		bool useScaleToFit = settings.value("use_scale_to_fit", true).toBool();
		if (useScaleToFit)
		{
			ui->scaleToFitCheckBox->setCheckState(Qt::Checked);
		}
		QString orientation=settings.value("orientation", "Portrait").toString();
		if (orientation=="Portrait")
			ui->orientationPortraitRadioButton->setChecked(true);
		if (orientation=="Landscape")
			ui->orientationLandscapeRadioButton->setChecked(true);
		bool printData = settings.value("print_data", true).toBool();
		if (printData)
		{
			ui->printDataCheckBox->setCheckState(Qt::Checked);
		}
		bool printSSEphemerides = settings.value("print_SS_ephemerides", true).toBool();
		if (printSSEphemerides)
		{
			ui->printSSEphemeridesCheckBox->setCheckState(Qt::Checked);
		}

	}
	catch (std::runtime_error& e)
	{
		qWarning() << "WARNING: unable to locate printsky.ini file or create a default one for PrintSky plugin: " << e.what();
	}


	//Initialize the style
	updateStyle();
}

//! Print report on a preview window
void PrintSkyDialog::previewSky()
{
	currentVisibilityGui = gui->getVisible();
	gui->setVisible(false);
	dialog->setVisible(false);

	outputOption = true;

	QTimer::singleShot(50, this, SLOT(executePrinterOutputOption()));
}

//! Draw contents report
void PrintSkyDialog::printDataSky(QPrinter * printer)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QPainter painter(printer);

	QGLWidget* glQGLWidget=(QGLWidget *) StelMainGraphicsView::getInstance().getStelQGLWidget();
	Q_ASSERT(glQGLWidget);


	QImage img=glQGLWidget->grabFrameBuffer();

	int imageYPos=(printDataOption? 300: 0);

	QSize sizeReal=printer->pageRect().size();
	sizeReal.setHeight(sizeReal.height()-imageYPos);

	if (scaleToFitOption)
		img=img.scaled(sizeReal, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	if (invertColorsOption)
		img.invertPixels();

	int imageXPos=(printer->pageRect().width()-img.width())/2;

	painter.drawImage(imageXPos, 0, img);

	StelCore* core = StelApp::getInstance().getCore();
	StelLocation locationData=core->getNavigator()->getCurrentLocation();
	double jd = core->getNavigator()->getJDay();

	QFont font("DejaVu Sans", 10, QFont::Normal);
	painter.setFont(font);
	qDebug() << "PrintSky: printer debugging information:";
	qDebug() << "Current printer resolution:" << printer->resolution();
	qDebug() << "Supported printer resolutions:" << printer->supportedResolutions();
	qDebug() << "Page size (size index, 0-30)" << printer->paperSize();
	//For the paper size index, see http://doc.qt.nokia.com/qprinter.html#PaperSize-enum

	if (printDataOption)
	{
		int posY=img.height()+50;

		QRect surfaceData(printer->paperRect().left(), posY, printer->pageRect().width(), imageYPos-50);

		//	painter.drawRect(surfaceData);


		painter.drawText(surfaceData.adjusted(0, 0, 0, -200), Qt::AlignCenter, "CHART INFORMATION");

		QString printLatitude=StelUtils::radToDmsStr((std::fabs(locationData.latitude)/180.)*M_PI);
		QString printLongitude=StelUtils::radToDmsStr((std::fabs(locationData.longitude)/180.)*M_PI);

		QString location = QString("Location: %1\t%2\t%3\t%4\t%5\t%6m")
							 .arg(locationData.name)
							 .arg(locationData.country)
							 .arg(locationData.planetName)
							 .arg(locationData.latitude<0 ? QString("%1S").arg(printLatitude) : QString("%1N").arg(printLatitude))
							 .arg(locationData.longitude<0 ? QString("%1W").arg(printLongitude) : QString("%1E").arg(printLongitude))
							 .arg(locationData.altitude);
		painter.drawText(surfaceData.adjusted(50, 50, 0, 0), Qt::AlignLeft, location);

		QString time="Local time: ";
		QString newDate = StelApp::getInstance().getLocaleMgr().getPrintableDateLocal(jd) +"   "
							+StelApp::getInstance().getLocaleMgr().getPrintableTimeLocal(jd)+QString(" (GMT%1%2)").arg(StelApp::getInstance().getLocaleMgr().getGMTShift(jd)>=0? '+':'-').arg(StelApp::getInstance().getLocaleMgr().getGMTShift(jd));
		time+=newDate;
		painter.drawText(surfaceData.adjusted(50, 100, 0, 0), Qt::AlignLeft, time);

		QString str;
		QTextStream wos(&str);
		wos << "FOV: " << qSetRealNumberPrecision(3) << core->getMovementMgr()->getCurrentFov() << QChar(0x00B0);
		painter.drawText(surfaceData.adjusted(50, 150, 0, 0), Qt::AlignLeft, *wos.string());

		painter.drawText(surfaceData.adjusted(surfaceData.width()-700, 0, 0, 0), Qt::AlignLeft, "Radius-magnitude relation");

		StarMgr* smgr = GETSTELMODULE(StarMgr);
		QList< QPair<float,float> > listPairsMagnitudesRadius=smgr->getListMagnitudeRadius(core);

		int xPos=-600, yPos=50;
		for (int icount=1; icount<=listPairsMagnitudesRadius.count(); ++icount)
		{
			painter.drawText(surfaceData.adjusted(surfaceData.width()+xPos, yPos, 0, 0), Qt::AlignLeft, QString("%1").arg(listPairsMagnitudesRadius.at(icount-1).first));
			painter.setBrush(Qt::SolidPattern);
			painter.drawEllipse(QPoint(surfaceData.left()+surfaceData.width()+xPos-40, surfaceData.top()+yPos+30), std::ceil(listPairsMagnitudesRadius.at(icount-1).second), std::ceil(listPairsMagnitudesRadius.at(icount-1).second));
			yPos+=50;
			if (yPos>=surfaceData.height())
			{
				xPos+=200;
				yPos=50;
			}
		}
	}

		// Print solar system ephemerides
	if (printSSEphemeridesOption)
	{
		printer->newPage();
		painter.drawText(0, 0, printer->paperRect().width(), 50, Qt::AlignCenter, "SOLAR SYSTEM EPHEMERIDES");
		painter.drawText(QRect(50, 150, 300, 50), Qt::AlignCenter, "Name");
		painter.drawText(QRect(400, 150, 300, 50), Qt::AlignCenter, "RA");
		painter.drawText(QRect(700, 150, 300, 50), Qt::AlignCenter, "Dec");
		painter.drawText(QRect(950, 100, 750, 50), Qt::AlignCenter, "Local Time");
		painter.drawText(QRect(950, 150, 250, 50), Qt::AlignCenter, "Rising");
		painter.drawText(QRect(1200, 150, 250, 50), Qt::AlignCenter, "Transit");
		painter.drawText(QRect(1450, 150, 250, 50), Qt::AlignCenter, "Setting");
		painter.drawText(QRect(1700, 150, 300, 50), Qt::AlignCenter, "Distance (AU)");
		painter.drawText(QRect(2000, 150, 300, 50), Qt::AlignCenter, "Ap.Magnitude");

		SolarSystem* ssmgr = GETSTELMODULE(SolarSystem);

		double geographicLongitude=-locationData.longitude*M_PI/180.;
		double geographicLatitude=locationData.latitude*M_PI/180.;

		PlanetP pHome=ssmgr->searchByEnglishName(locationData.planetName);
		double standardSideralTime=pHome->getSiderealTime(((int) jd)+0.5)*M_PI/180.;

		//After the introduction of the Comets and Asteroids plug-in,
		//there will be *lots* of planet names. At the very least,
		//this needs a mechanism for creating page breaks.
		QStringList allBodiesNames=ssmgr->getAllPlanetEnglishNames();
		allBodiesNames.sort();
		for (int iBodyName=1, yPos=200; iBodyName<=allBodiesNames.count(); ++iBodyName)
		{
			QString englishName=allBodiesNames.at(iBodyName-1);
			PlanetP p=ssmgr->searchByEnglishName(englishName);
			double dec, ra;
			StelUtils::rectToSphe(&ra,&dec, p->getEquinoxEquatorialPos(core->getNavigator()));
			double standardAltitude=-0.5667;
			if (englishName=="Sun")
				standardAltitude=-0.8333;
			if (englishName=="Moon")
				standardAltitude=0.125;
			standardAltitude*=M_PI/180.;

			double cosH=(std::sin(standardAltitude)-(std::sin(geographicLatitude)*std::sin(dec)))/(std::cos(geographicLatitude)*std::cos(dec));

			if (englishName!=locationData.planetName && cosH>=-1. && cosH<=1.)
			{

				double angleH=std::acos(cosH);
				double transit=((ra+geographicLongitude-standardSideralTime)/(2*M_PI));
				if (transit>1.)
					transit-=1.;
				if (transit<0)
					transit+=1.;

				double rising=transit-angleH/(2*M_PI);
				if (rising>1.)
					rising-=1.;
				if (rising<0.)
					rising+=1.;

				double setting=transit+angleH/(2*M_PI);
				if (setting>1.)
					setting-=1.;
				if (setting<0.)
					setting+=1.;

				painter.drawText(QRect(50, yPos, 300, 50), Qt::AlignLeft, englishName);
				int shift=StelApp::getInstance().getLocaleMgr().getGMTShift(jd);
				painter.drawText(QRect(350, yPos, 300, 50), Qt::AlignRight, QString("%1").arg(StelUtils::radToHmsStr(ra)));
				painter.drawText(QRect(650, yPos, 300, 50), Qt::AlignRight, QString("%1").arg(StelUtils::radToDmsStr(dec)));
				painter.drawText(QRect(950, yPos, 250, 50), Qt::AlignCenter, QString("%1").arg(printableTime(rising, shift)));
				painter.drawText(QRect(1200, yPos, 250, 50), Qt::AlignCenter, QString("%1").arg(printableTime(transit, shift)));
				painter.drawText(QRect(1450, yPos, 250, 50), Qt::AlignCenter, QString("%1").arg(printableTime(setting, shift)));
				painter.drawText(QRect(1700, yPos, 300, 50), Qt::AlignRight, QString("%1").arg(p->getDistance(), 0, 'g', 5));
				painter.drawText(QRect(2000, yPos, 300, 50), Qt::AlignRight, QString("%1").arg(p->getVMagnitude(core->getNavigator()), 0, 'g', 3));

				yPos+=50;
			}
		}
	}
	QApplication::restoreOverrideCursor();
}

//! Print report direct to printer
void PrintSkyDialog::printSky()
{
	currentVisibilityGui=gui->getVisible();
	gui->setVisible(false);
	dialog->setVisible(false);

	outputOption=false;

	QTimer::singleShot(50, this, SLOT(executePrinterOutputOption()));

}

//! Read the printer parameters and run the output option selected (Print/Preview)
void PrintSkyDialog::executePrinterOutputOption()
{
	//Options for printing image.
	invertColorsOption=false;
	scaleToFitOption=true;
	orientationOption="Portrait";
	printDataOption=true;
	printSSEphemeridesOption=true;

	try
	{
		StelFileMgr::Flags flags = (StelFileMgr::Flags)(StelFileMgr::Directory|StelFileMgr::Writable);
		QString printskyIniPath = StelFileMgr::findFile("modules/PrintSky/", flags) + "printsky.ini";
		QSettings settings(printskyIniPath, QSettings::IniFormat);
		invertColorsOption=settings.value("use_invert_colors", false).toBool();
		scaleToFitOption=settings.value("use_scale_to_fit", true).toBool();
		orientationOption=settings.value("orientation", "Portrait").toString();
		printDataOption=settings.value("print_data", true).toBool();
		printSSEphemeridesOption=settings.value("print_SS_ephemerides", true).toBool();
	}
	catch (std::runtime_error& e)
	{
		qWarning() << "WARNING: unable to locate printsky.ini file or create a default one for PrintSky plugin: " << e.what();
	}


	//QPrinter printer(QPrinter::HighResolution);
	QPrinter printer(QPrinter::ScreenResolution);
	printer.setResolution(300);
	printer.setDocName("STELLARIUM REPORT");
	printer.setOrientation((orientationOption=="Portrait"? QPrinter::Portrait: QPrinter::Landscape));

	if (outputOption)
	{
		//QPrintPreviewDialog oPrintPreviewDialog(&printer, &StelMainGraphicsView::getInstance());
		QPrintPreviewDialog oPrintPreviewDialog(&printer);
		connect(&oPrintPreviewDialog, SIGNAL(paintRequested(QPrinter *)), this, SLOT(printDataSky(QPrinter *)));
		oPrintPreviewDialog.exec();
	}
	else
	{
		//QPrintDialog dialogPrinter(&printer, &StelMainGraphicsView::getInstance());
		QPrintDialog dialogPrinter(&printer);
		if (dialogPrinter.exec() == QDialog::Accepted)
			printDataSky(&printer);
	}

	gui->setVisible(currentVisibilityGui);
	((QGraphicsWidget*)StelMainGraphicsView::getInstance().getStelAppGraphicsWidget())->setFocus(Qt::OtherFocusReason);
}



void PrintSkyDialog::enableOutputOptions(bool enable)
{
	ui->buttonsFrame->setVisible(enable);
}


QString PrintSkyDialog::printableTime(double time, int shift)
{
	time*=24.;
	time+=shift;
	if (time>=24.)
		time-=24.;
	if (time<0)
		time+=24.;
	int hour=(int) time;
	time-=hour;
	time*=60;
	int minute=(int) time;
	return(QString("%1:%2").arg(hour, 2, 10, QChar('0')).arg(minute, 2, 10, QChar('0')));
}
