// WiFiTelescopeControlDialog.hpp
#ifndef WIFITELESCOPECONTROLDIALOG_HPP
#define WIFITELESCOPECONTROLDIALOG_HPP

#include "StelDialog.hpp"
#include "WiFiTelescope.hpp"

class Ui_WiFiTelescopeControlDialogForm;

class WiFiTelescopeControlDialog : public StelDialog
{
    Q_OBJECT
    
public:
    WiFiTelescopeControlDialog(WiFiTelescope* telescope);
    ~WiFiTelescopeControlDialog();

protected:
    void createDialogContent() override;
    
private slots:
    void connectToTelescope();
    void disconnectTelescope();
    void gotoSelectedObject();
    void startObservation();
    void stopObservation();
    void parkTelescope();
    void updateConnectionStatus(bool connected);
    void updateTelescopeStatus(const QString& status);
    
private:
    std::unique_ptr<Ui_WiFiTelescopeControlDialogForm> ui;
    WiFiTelescope* telescope;
    
    void setupConnections();
    void updateUI();
};

#endif // WIFITELESCOPECONTROLDIALOG_HPP
