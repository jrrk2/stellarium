#ifndef WIFITELESCOPECONTROL_HPP
#define WIFITELESCOPECONTROL_HPP

#include "StelModule.hpp"
#include "StelPluginInterface.hpp"

class WiFiTelescopeControlDialog;
class WiFiTelescope;
class StelButton;

class WiFiTelescopeControl : public StelModule, public StelPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID StelPluginInterface_iid)
    Q_INTERFACES(StelPluginInterface)

public:
    WiFiTelescopeControl();
    virtual ~WiFiTelescopeControl();

    // StelModule methods
    virtual void init() override;
    virtual void deinit() override;
    virtual void update(double deltaTime) override;
    virtual void draw(StelCore* core) override;
    virtual double getCallOrder(StelModuleActionName actionName) const override;

    // StelPluginInterface methods
    virtual bool configureGui(bool show) override;
    virtual StelPluginInfo getPluginInfo() const override;

public slots:
    void slotControlTelescopeFromGui();
    void slotGotoTarget(double ra, double dec, const QString& objectName);

private:
    // GUI elements
    WiFiTelescopeControlDialog* configDialog;
    StelButton* toolbarButton;

    // Telescope control
    WiFiTelescope* telescope;

    void setupToolbarButton();
};

#endif // WIFITELESCOPECONTROL_HPP
