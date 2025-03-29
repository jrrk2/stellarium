#include "WiFiTelescopeControlDialog.hpp"
// Assuming the UI file is generated and the header is created with the UI class
#include "WiFiTelescopeControl-static_autogen/include/ui_WiFiTelescopeControlDialog.h"
#include "WiFiTelescope.hpp"

#include "StelApp.hpp"
#include "StelCore.hpp"
#include "StelModuleMgr.hpp"
#include "StelObjectMgr.hpp"
#include "StelUtils.hpp"
#include "StelTranslator.hpp"

#include <QDebug>
#include <QTimer>



WiFiTelescopeControlDialog::WiFiTelescopeControlDialog(WiFiTelescope* telescope)
    : StelDialog("WiFiTelescopeControl")
    , ui(new Ui_WiFiTelescopeControlDialogForm())
    , telescope(telescope)
{
    // Setup the dialog from the script
    if (telescope == nullptr)
    {
        qWarning() << "WiFiTelescopeControlDialog: telescope is null!";
    }
}

WiFiTelescopeControlDialog::~WiFiTelescopeControlDialog()
{
    delete ui;
}

void WiFiTelescopeControlDialog::createDialogContent()
{
    ui->setupUi(dialog);
    
    // Connect signals and slots
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(connectToTelescope()));
    connect(ui->gotoButton, SIGNAL(clicked()), this, SLOT(gotoSelectedObject()));
    connect(ui->observeButton, SIGNAL(clicked()), this, SLOT(startObservation()));
    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopObservation()));
    connect(ui->parkButton, SIGNAL(clicked()), this, SLOT(parkTelescope()));
    connect(ui->focusButton, SIGNAL(clicked()), this, SLOT(focusTelescope()));
    
    // Connect telescope status updates
    if (telescope)
    {
        connect(telescope, SIGNAL(statusUpdated(QString)), 
                ui->statusLabel, SLOT(setText(QString)));
        
        connect(telescope, SIGNAL(connected()), 
                this, SLOT(updateConnectionStatus()));
        
        connect(telescope, SIGNAL(disconnected()), 
                this, SLOT(updateConnectionStatus()));
    }
    
    // Set default values
    ui->ipAddressEdit->setText("10.0.0.1");
    ui->portSpinBox->setValue(8082);
    ui->exposureSpinBox->setValue(30.0);
    ui->gainSpinBox->setValue(20.0);
    
    updateUI();
}

void WiFiTelescopeControlDialog::connectToTelescope()
{
    if (!telescope)
        return;
    
    if (!telescope->isConnected())
    {
        QString ip = ui->ipAddressEdit->text();
        int port = ui->portSpinBox->value();
        
        ui->connectButton->setEnabled(false);
        ui->statusLabel->setText(q_("Connecting to telescope..."));
        
        telescope->connect(ip, port);
    }
    else
    {
        telescope->disconnect();
        updateConnectionStatus();
    }
}

void WiFiTelescopeControlDialog::updateConnectionStatus()
{
    bool connected = telescope && telescope->isConnected();
    
    ui->connectButton->setText(connected ? q_("Disconnect") : q_("Connect"));
    ui->connectButton->setEnabled(true);
    ui->gotoButton->setEnabled(connected);
    ui->observeButton->setEnabled(connected);
    ui->stopButton->setEnabled(connected);
    ui->parkButton->setEnabled(connected);
    ui->focusButton->setEnabled(connected);
    
    if (!connected)
    {
        ui->statusLabel->setText(q_("Disconnected"));
    }
}

void WiFiTelescopeControlDialog::gotoSelectedObject()
{
    if (!telescope || !telescope->isConnected())
        return;
    
    StelObjectMgr* objMgr = GETSTELMODULE(StelObjectMgr);
    if (!objMgr)
        return;
    
    const QList<StelObjectP> selectedObjects = objMgr->getSelectedObject();
    if (selectedObjects.isEmpty())
    {
        ui->statusLabel->setText(q_("No object selected"));
        return;
    }
    
    StelObjectP obj = selectedObjects[0];
    
    // Get the J2000 coordinates
    StelCore* core = StelApp::getInstance().getCore();
    Vec3d objPos = obj->getJ2000EquatorialPos(core);

    // Convert to spherical coordinates (RA/Dec)
    double ra, dec;
    StelUtils::rectToSphe(&ra, &dec, objPos);
    
    // Convert to degrees for the telescope
    double raDeg = ra * 180.0 / M_PI;
    double decDeg = dec * 180.0 / M_PI;
    
    QString objectName = obj->getNameI18n();
    
    ui->statusLabel->setText(q_("Slewing to %1").arg(objectName));
    telescope->gotoCoordinates(raDeg, decDeg, objectName);
}

void WiFiTelescopeControlDialog::startObservation()
{
    if (!telescope || !telescope->isConnected())
        return;
    
    StelObjectMgr* objMgr = GETSTELMODULE(StelObjectMgr);
    if (!objMgr)
        return;
    
    const QList<StelObjectP> selectedObjects = objMgr->getSelectedObject();
    if (selectedObjects.isEmpty())
    {
        ui->statusLabel->setText(q_("No object selected"));
        return;
    }
    
    StelObjectP obj = selectedObjects[0];
    
    
    // Get the J2000 coordinates
    StelCore* core = StelApp::getInstance().getCore();
    Vec3d objPos = obj->getJ2000EquatorialPos(core);

    // Convert to spherical coordinates (RA/Dec)
    double ra, dec;
    StelUtils::rectToSphe(&ra, &dec, objPos);
    
    // Convert to degrees for the telescope
    double raDeg = ra * 180.0 / M_PI;
    double decDeg = dec * 180.0 / M_PI;
    
    QString objectName = obj->getNameI18n();
    double exposure = ui->exposureSpinBox->value();
    double gain = ui->gainSpinBox->value();
    
    ui->statusLabel->setText(q_("Starting observation of %1").arg(objectName));
    telescope->startObservation(raDeg, decDeg, objectName, exposure, gain);
}

void WiFiTelescopeControlDialog::stopObservation()
{
    if (!telescope || !telescope->isConnected())
        return;
    
    ui->statusLabel->setText(q_("Stopping observation"));
    telescope->stopObservation();
}

void WiFiTelescopeControlDialog::parkTelescope()
{
    if (!telescope || !telescope->isConnected())
        return;
    
    ui->statusLabel->setText(q_("Parking telescope"));
    telescope->park();
}

void WiFiTelescopeControlDialog::focusTelescope()
{
    if (!telescope || !telescope->isConnected())
        return;
    
    ui->statusLabel->setText(q_("Focusing"));
    telescope->focus();
}

void WiFiTelescopeControlDialog::updateUI()
{
    updateConnectionStatus();
}

void WiFiTelescopeControlDialog::retranslate()
{
    if (dialog)
    {
        ui->setupUi(dialog);
        dialog->setWindowTitle(q_("WiFi Telescope Control"));
    }
}

