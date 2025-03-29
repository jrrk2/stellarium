#include "WiFiTelescopeControl.hpp"
#include "WiFiTelescope.hpp"
#include "WiFiTelescopeControlDialog.hpp"

#include "StelApp.hpp"
#include "StelLocaleMgr.hpp"
#include "StelModule.hpp"
#include "StelModuleMgr.hpp"
#include "StelObjectMgr.hpp"
#include "StelGui.hpp"
#include "StelGuiItems.hpp"
#include "StelTranslator.hpp"

WiFiTelescopeControl::WiFiTelescopeControl()
    : configDialog(nullptr)
    , toolbarButton(nullptr)
    , telescope(nullptr)
{
    setObjectName("WiFiTelescopeControl");
}

WiFiTelescopeControl::~WiFiTelescopeControl()
{
    delete telescope;
}

void WiFiTelescopeControl::init()
{
    telescope = new WiFiTelescope(this);
    
    // Setup GUI
    setupToolbarButton();
    
    // Connect to StelObjectMgr signals for object selection
    StelObjectMgr* objMgr = GETSTELMODULE(StelObjectMgr);
    connect(objMgr, SIGNAL(selectedObjectChanged(StelModule::StelModuleSelectAction)),
            this, SLOT(slotObjectSelected(StelModule::StelModuleSelectAction)));
}

void WiFiTelescopeControl::deinit()
{
    // Cleanup if needed
}

void WiFiTelescopeControl::update(double deltaTime)
{
  (void)deltaTime;
    // Any periodic updates
}

void WiFiTelescopeControl::draw(StelCore* core)
{
  (void)core;
    // Any custom drawing
}

double WiFiTelescopeControl::getCallOrder(StelModuleActionName actionName) const
{
  (void)actionName;
    // Default value that works for most plugins
    return 0;
}

bool WiFiTelescopeControl::configureGui(bool show)
{
    if (show)
    {
        // Create dialog if it doesn't exist
        if (!configDialog)
        {
            configDialog = new WiFiTelescopeControlDialog(telescope);
        }
        configDialog->setVisible(true);
        return true;
    }
    
    if (configDialog)
    {
        configDialog->setVisible(false);
        return true;
    }
    
    return false;
}

StelPluginInfo WiFiTelescopeControl::getPluginInfo() const
{
    StelPluginInfo info;
    info.id = "WiFiTelescopeControl";
    info.displayedName = q_("WiFi Telescope Control");
    info.authors = "Your Name";
    info.contact = "your.email@example.com";
    info.description = q_("Control WiFi-enabled telescopes from Stellarium");
    info.version = "1.0.0";
    
    return info;
}

void WiFiTelescopeControl::setupToolbarButton()
{
    // Add a toolbar button
    StelGui* gui = dynamic_cast<StelGui*>(StelApp::getInstance().getGui());
    if (gui)
    {
        toolbarButton = new StelButton(nullptr, 
                                      QPixmap(":/WiFiTelescopeControl/telescope.png"), 
                                      QPixmap(":/WiFiTelescopeControl/telescope_on.png"), 
                                      QPixmap(":/WiFiTelescopeControl/telescope_hover.png"), 
                                      "actionShow_WiFiTelescopeControl");
        gui->getButtonBar()->addButton(toolbarButton, "065-pluginsGroup");
        connect(toolbarButton, SIGNAL(triggered()), this, SLOT(slotControlTelescopeFromGui()));
    }
}

void WiFiTelescopeControl::slotControlTelescopeFromGui()
{
    configureGui(!configDialog || !configDialog->visible());
}

void WiFiTelescopeControl::slotGotoTarget(double ra, double dec, const QString& objectName)
{
    if (telescope && telescope->isConnected())
    {
        telescope->gotoCoordinates(ra, dec, objectName);
    }
}
