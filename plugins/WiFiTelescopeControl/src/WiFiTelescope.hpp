#ifndef WIFITELESCOPE_HPP
#define WIFITELESCOPE_HPP

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>

class WiFiTelescope : public QObject
{
    Q_OBJECT

public:
    WiFiTelescope(QObject* parent = nullptr);
    ~WiFiTelescope();

    bool connect(const QString& ipAddress, int port = 8082);
    void disconnect();
    bool isConnected() const;

    // Telescope control functions
    bool takeControl();
    bool gotoCoordinates(double ra, double dec, const QString& objectName = QString());
    bool startObservation(double ra, double dec, const QString& objectName, 
                        double exposure = 30.0, double gain = 20.0);
    bool stopObservation();
    bool park();
    bool focus();
    bool openArm();
    bool autoInitialize(double latitude, double longitude);

    // Status information
    QString getStatus() const;
    double getAltitude() const;
    double getAzimuth() const;
    QString getTargetName() const;

signals:
    void connected();
    void disconnected();
    void connectionError(const QString& errorMessage);
    void statusUpdated(const QString& status);
    void coordinatesUpdated(double ra, double dec, double alt, double az);

private slots:
    void handleNetworkReply();
    void processStatusUpdate();

private:
    QNetworkAccessManager* networkManager;
    QString serverAddress;
    int serverPort;
    bool connected;
    
    // Telescope state
    QString currentStatus;
    double currentRa;
    double currentDec;
    double currentAlt;
    double currentAz;
    QString currentTarget;
    
    // Authentication data
    QString authToken;
    
    // Helper methods
    QNetworkRequest createRequest(const QString& endpoint);
    void addAuthenticationHeaders(QNetworkRequest& request);
    bool sendCommand(const QString& endpoint, const QJsonObject& payload);
    void processResponse(const QByteArray& response);
};

#endif // WIFITELESCOPE_HPP
