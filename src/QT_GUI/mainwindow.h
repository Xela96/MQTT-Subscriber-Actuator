#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include "MQTTClient.h"
#include "ui_mainwindow.h"

#define ADDRESS     "tcp://127.0.0.1:1883"
#define CLIENTID    "rpi2"
#define AUTHMETHOD  "alex"
#define AUTHTOKEN   "toilet11"
#define TOPIC       "ee513/ADXLdata"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int AccelerationX, AccelerationY, AccelerationZ;
    float pitch, roll, CPUTemp;
    int switchNum=0;
private slots:
    void on_downButton_clicked();
    void on_upButton_clicked();
    void on_pushButton_clicked();
    void parseJSON(QString payload);
    void onListWidgetItemClicked(QListWidgetItem* item);
    void onListWidget_2ItemClicked(QListWidgetItem* item);
    void on_connectButton_clicked();
    void on_disconnectButton_clicked();
    void on_MQTTmessage(QString message);

signals:
    void messageSignal(QString message);

private:
    Ui::MainWindow *ui;
    void update();
    int count, time;
    MQTTClient client;
    volatile MQTTClient_deliveryToken deliveredtoken;

    friend void delivered(void *context, MQTTClient_deliveryToken dt);
    friend int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
    friend void connlost(void *context, char *cause);
};

void delivered(void *context, MQTTClient_deliveryToken dt);
int  msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void connlost(void *context, char *cause);

#endif // MAINWINDOW_H

