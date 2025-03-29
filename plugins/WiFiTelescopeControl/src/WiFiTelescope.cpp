#include "WiFiTelescope.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

WiFiTelescope::WiFiTelescope(QObject* parent)
    : QObject(parent)
    , networkManager(new QNetworkAccessManager(this))
    , serverPort(8082)
    , isConnectedFlag(false)
    , currentRa(0.0)
    , currentDec(0.0)
    , currentAlt(0.0)
    , currentAz(0.0)
{
    // Set up a timer for periodic status updates
    QTimer* statusTimer = new QTimer(this);
    
    // Change from new-style connection:
    // connect(statusTimer, &QTimer::timeout, this, &WiFiTelescope::processStatusUpdate);
    
    // To old-style connection:
    QObject::connect(statusTimer, SIGNAL(timeout()), this, SLOT(processStatusUpdate()));
    
    statusTimer->start(2000); // Update every 2 seconds
    
    // Change from new-style connection:
    // connect(networkManager, &QNetworkAccessManager::finished, this, &WiFiTelescope::handleNetworkReply);
    
    // To old-style connection:
    QObject::connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleNetworkReply()));
}

WiFiTelescope::~WiFiTelescope()
{
    disconnect();
}

bool WiFiTelescope::connect(const QString& ipAddress, int port)
{
    serverAddress = ipAddress;
    serverPort = port;
    
    qDebug() << "Connecting to telescope at" << ipAddress << ":" << port;
    
    // Simulated successful connection for now
    QTimer::singleShot(500, [this]() {
        isConnectedFlag = true;
        authToken = "dummy_auth_token";
        emit connectionEstablished();
        currentStatus = "Connected";
        emit statusUpdated(currentStatus);
    });
    
    return true;
}

void WiFiTelescope::disconnect()
{
    if (isConnectedFlag) {
        isConnectedFlag = false;
        emit disconnected();
    }
}

bool WiFiTelescope::isConnected() const
{
    return isConnectedFlag;
}

QNetworkRequest WiFiTelescope::createRequest(const QString& endpoint)
{
    QString url = QString("http://%1:%2%3").arg(serverAddress).arg(serverPort).arg(endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    if (!authToken.isEmpty()) {
        addAuthenticationHeaders(request);
    }
    
    return request;
}

void WiFiTelescope::addAuthenticationHeaders(QNetworkRequest& request)
{
    request.setRawHeader("Authorization", authToken.toUtf8());
}

bool WiFiTelescope::sendCommand(const QString& endpoint, const QJsonObject& payload)
{
    if (!isConnectedFlag) {
        return false;
    }
    
    QNetworkRequest request = createRequest(endpoint);
    QJsonDocument doc(payload);
    QByteArray data = doc.toJson();
    
    networkManager->post(request, data);
    return true;
}

void WiFiTelescope::handleNetworkReply()
{
    // This would process actual network replies
    // For now, this is a placeholder
}

bool WiFiTelescope::gotoCoordinates(double ra, double dec, const QString& objectName)
{
    if (!isConnectedFlag) {
        qWarning() << "Cannot goto: not connected to telescope";
        return false;
    }
    
    qDebug() << "GOTO command: RA =" << ra << "DEC =" << dec << "Object:" << objectName;
    
    // For this skeleton implementation, we'll simulate the telescope movement
    currentRa = ra;
    currentDec = dec;
    currentAlt = 45.0;  // Dummy value
    currentAz = 180.0;  // Dummy value
    currentTarget = objectName;
    
    // In a real implementation, these would be calculated from RA/Dec
    // using Stellarium's coordinate conversion functions
    
    QJsonObject payload;
    payload["ALT"] = currentAlt;
    payload["AZ"] = currentAz;
    
    QString endpoint = "/v1/motors/goAbsolute";
    return sendCommand(endpoint, payload);
}

bool WiFiTelescope::takeControl()
{
    QString endpoint = "/v1/app/takeControl";
    QJsonObject payload;
    return sendCommand(endpoint, payload);
}

bool WiFiTelescope::startObservation(double ra, double dec, const QString& objectName, 
                                     double exposure, double gain)
{
    if (!isConnectedFlag) {
        return false;
    }
    
    // Convert to internal format
    int exposureUs = static_cast<int>(exposure * 1000000); // to microseconds
    int gainInt = static_cast<int>(gain * 10);            // to internal gain units
    
    QJsonObject payload;
    payload["ra"] = ra;
    payload["de"] = dec;
    payload["isJ2000"] = true;
    payload["rot"] = 0;
    payload["objectId"] = objectName.simplified().replace(" ", "_");
    payload["objectName"] = objectName;
    payload["gain"] = gainInt;
    payload["exposureMicroSec"] = exposureUs;
    payload["doStacking"] = true;
    payload["histogramEnabled"] = true;
    payload["histogramLow"] = -0.75;
    payload["histogramMedium"] = 5;
    payload["histogramHigh"] = 0;
    payload["backgroundEnabled"] = true;
    payload["backgroundPolyorder"] = 4;
    
    QString endpoint = "/v1/general/startObservation";
    return sendCommand(endpoint, payload);
}

bool WiFiTelescope::stopObservation()
{
    QString endpoint = "/v1/general/stopObservation";
    QJsonObject payload;
    return sendCommand(endpoint, payload);
}

bool WiFiTelescope::park()
{
    QString endpoint = "/v1/general/park";
    QJsonObject payload;
    return sendCommand(endpoint, payload);
}

bool WiFiTelescope::focus()
{
    QString endpoint = "/v1/general/adjustObservationFocus";
    QJsonObject payload;
    return sendCommand(endpoint, payload);
}

bool WiFiTelescope::openArm()
{
    QString endpoint = "/v1/general/openForMaintenance";
    QJsonObject payload;
    return sendCommand(endpoint, payload);
}

bool WiFiTelescope::autoInitialize(double latitude, double longitude)
{
    QJsonObject payload;
    payload["latitude"] = latitude;
    payload["longitude"] = longitude;
    payload["time"] = static_cast<int>(QDateTime::currentMSecsSinceEpoch());
    
    QString endpoint = "/v1/general/startAutoInit";
    return sendCommand(endpoint, payload);
}

// Getters
QString WiFiTelescope::getStatus() const { return currentStatus; }
double WiFiTelescope::getAltitude() const { return currentAlt; }
double WiFiTelescope::getAzimuth() const { return currentAz; }
QString WiFiTelescope::getTargetName() const { return currentTarget; }
