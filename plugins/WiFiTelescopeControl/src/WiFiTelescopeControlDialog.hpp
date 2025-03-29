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
    
    // Implement the required retranslate method from StelDialog
    void retranslate() override;

private slots:
    void connectToTelescope();
    void gotoSelectedObject();
    void startObservation();
    void stopObservation();
    void parkTelescope();
    void focusTelescope();
    void updateConnectionStatus();
    
private:
    Ui_WiFiTelescopeControlDialogForm* ui;
    WiFiTelescope* telescope;
    
    void updateUI();
};

#endif // WIFITELESCOPECONTROLDIALOG_HPP
