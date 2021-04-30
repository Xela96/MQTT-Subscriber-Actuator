#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include <QString>
#include <jsoncpp/json/json.h>

MainWindow *handle;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->count = 50;
    this->time = 0;
    this->setWindowTitle("EE513 Assignment 2");
    this->ui->customPlot->addGraph();
    this->ui->customPlot->yAxis->setLabel("Value");
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    this->ui->customPlot->xAxis->setTicker(timeTicker);
    this->ui->customPlot->yAxis->setRange(-180,180);
    this->ui->customPlot->replot();

    this->ui->listWidget->addItem("ee513/CPUTemp");
    this->ui->listWidget->addItem("ee513/ADXLdata");
    this->ui->listWidget->addItem("ee513/LastWill");
    QString cur = this->ui->listWidget->item(0)->text();
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                this, SLOT(onListWidgetItemClicked(QListWidgetItem*)));

    this->ui->listWidget_2->addItem("Acceleration X");
    this->ui->listWidget_2->addItem("Acceleration Y");
    this->ui->listWidget_2->addItem("Acceleration Z");
    this->ui->listWidget_2->addItem("Pitch");
    this->ui->listWidget_2->addItem("Roll");
    this->ui->listWidget_2->addItem("CPUTemp");
    QString cur2 = this->ui->listWidget_2->item(0)->text();
    connect(ui->listWidget_2, SIGNAL(itemClicked(QListWidgetItem*)),
                this, SLOT(onListWidget_2ItemClicked(QListWidgetItem*)));


    QObject::connect(this, SIGNAL(messageSignal(QString)),
                     this, SLOT(on_MQTTmessage(QString)));
    ::handle = this;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update(){
    // For more help on real-time plots, see: http://www.qcustomplot.com/index.php/demos/realtimedatademo
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
    ui->customPlot->graph(0)->addData(key,count);
    ui->customPlot->graph(0)->rescaleKeyAxis(true);
    ui->customPlot->replot();
    QString text = QString("Value added is %1").arg(this->count);
    ui->outputEdit->setText(text);
}

void MainWindow::on_downButton_clicked()
{
    this->count-=10;
    this->update();
}

void MainWindow::on_upButton_clicked()
{
    this->count+=10;
    this->update();
}

void MainWindow::on_connectButton_clicked()
{
    MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
    int rc;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    opts.keepAliveInterval = 20;
    opts.cleansession = 1;
    opts.username = AUTHMETHOD;
    opts.password = AUTHTOKEN;

    if (MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)==0){
        ui->outputText->appendPlainText(QString("Callbacks set correctly"));
    }
    if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
        ui->outputText->appendPlainText(QString("Failed to connect, return code %1").arg(rc));
    }
    ui->outputText->appendPlainText(QString("Subscribing to topic " TOPIC " for client " CLIENTID));
    int x = MQTTClient_subscribe(client, TOPIC, QOS);
    ui->outputText->appendPlainText(QString("Result of subscribe is %1 (0=success)").arg(x));
}

void MainWindow::onListWidgetItemClicked(QListWidgetItem* item)
{
//    ui->outputText->appendPlainText(QString("Subscribing to topic " TOPIC " for client " CLIENTID));
//    MQTTClient_subscribe(client, TOPIC, QOS);
    qDebug() << item->text()<<endl;
    QString str1 = item->text();
    QByteArray ba = str1.toLocal8Bit();
    const char *selectedTopic = ba.data();

    QString str2 = QString("Subscribing to topic ");
    QString str3 = str2.append(str1);
    QString str4 = " for client " CLIENTID;
    QString str5 = str3.append(str4);

    MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
    int rc;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    opts.keepAliveInterval = 20;
    opts.cleansession = 1;
    opts.username = AUTHMETHOD;
    opts.password = AUTHTOKEN;

    if (MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)==0){
        ui->outputText->appendPlainText(QString("Callbacks set correctly"));
    }
    if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
        ui->outputText->appendPlainText(QString("Failed to connect, return code %1").arg(rc));
    }
    //ui->outputText->appendPlainText(QString("Subscribing to topic " selectedTopic " for client " CLIENTID));
    ui->outputText->appendPlainText(QString(str5));
    int x = MQTTClient_subscribe(client, selectedTopic, QOS);
    ui->outputText->appendPlainText(QString("Result of subscribe is %1 (0=success)").arg(x));
}



void MainWindow::on_pushButton_clicked()
{
    // Get the pointer to the currently selected item.
    QListWidgetItem *item = ui->listWidget->currentItem();

    // Set the text color and its background color using the pointer to the item.
    item->setTextColor(Qt::white);
    item->setBackgroundColor(Qt::blue);
}

void delivered(void *context, MQTTClient_deliveryToken dt) {
    (void)context;
    // Please don't modify the Window UI from here
    qDebug() << "Message delivery confirmed";
    handle->deliveredtoken = dt;
}

void MainWindow::onListWidget_2ItemClicked(QListWidgetItem* item)
{
    QString val = item->text();
    ui->customPlot->clearGraphs();
    ui->customPlot->addGraph();
    ui->customPlot->replot();
    if(val=="Pitch")
    {
        this->ui->customPlot->yAxis->setRange(-180,180);
        switchNum=0;
        qDebug() << "SwitchNum: " << switchNum << endl;
    }
    else if(val=="Roll"){
        this->ui->customPlot->yAxis->setRange(-180,180);
        switchNum=1;
        qDebug() << "SwitchNum: " << switchNum << endl;
    }
    else if(val=="Acceleration X"){
        this->ui->customPlot->yAxis->setRange(-80,80);
        switchNum=2;
        qDebug() << "SwitchNum: " << switchNum << endl;
    }
    else if(val=="Acceleration Y"){
        this->ui->customPlot->yAxis->setRange(-80,80);
        switchNum=3;
        qDebug() << "SwitchNum: " << switchNum << endl;
    }
    else if(val=="Acceleration Z"){
        this->ui->customPlot->yAxis->setRange(0,300);
        switchNum=4;
        qDebug() << "SwitchNum: " << switchNum << endl;
    }
    if(val=="CPUTemp")
    {
        this->ui->customPlot->yAxis->setRange(0,100);
        switchNum=5;
        qDebug() << "SwitchNum: " << switchNum << endl;
    }
}

/* This is a callback function and is essentially another thread. Do not modify the
 * main window UI from here as it will cause problems. Please see the Slot method that
 * is directly below this function. To ensure that this method is thread safe I had to
 * get it to emit a signal which is received by the slot method below */
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    int i;
    QString string1;
    char ch;
    char* payloadptr;
    (void)context; (void)topicLen;
    qDebug() << "Message arrived (topic is " << topicName << ")";
    qDebug() << "Message payload length is " << message->payloadlen;
    QString payload;
    payload.sprintf("%s", (char *) message->payload).truncate(message->payloadlen);
    emit handle->messageSignal(payload);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

/** This is the slot method. Do all of your message received work here. It is also safe
 * to call other methods on the object from this point in the code */
void MainWindow::on_MQTTmessage(QString payload){
    ui->outputText->appendPlainText(payload);
    ui->outputText->ensureCursorVisible();

    //ADD YOUR CODE HERE
    this->parseJSON(payload);
    this->update();
}

void MainWindow::parseJSON(QString payload){
    QJsonDocument d = QJsonDocument::fromJson(payload.toUtf8());
    QJsonObject obj = d.object();
    qDebug() << "Payload: " << payload << endl;

    this->AccelerationX = obj["X-Acceleration"].toInt();
    this->AccelerationY = (double)obj["Y-Acceleration"].toDouble();
    this->AccelerationZ = (double)obj["Z-Acceleration"].toDouble();
    this->pitch = (double)obj["Pitch"].toDouble();
    this->roll = (double)obj["Roll"].toDouble();
    this->CPUTemp = (double)obj["CPUTemp"].toDouble();


    qDebug() << "Parsed data x " << this->AccelerationX << ")";
    qDebug() << "Parsed data y " << this->AccelerationY << ")";
    qDebug() << "Parsed data z " << this->AccelerationZ << ")";
    qDebug() << "Parsed data pitch " << this->pitch << ")";
    qDebug() << "Parsed data roll " << this->roll << ")";
    qDebug() << "Parsed data CPUtemp " << this->CPUTemp << ")";

    switch(switchNum){
    case 0: this->count = this->pitch;
        qDebug() << "pitch: " << this->pitch << endl;
        break;
    case 1: this->count = this->roll;
        qDebug() << "roll: " << this->roll << endl;
        break;
    case 2: this->count = this->AccelerationX;
        qDebug() << "xAcc: " << this->AccelerationX << endl;
        break;
    case 3: this->count = this->AccelerationY;
        qDebug() << "Yacc: " << this->AccelerationY << endl;
        break;
    case 4: this->count = this->AccelerationZ;
        qDebug() << "zAcc: " << this->AccelerationZ << endl;
        break;
    case 5: this->count = this->CPUTemp;
        qDebug() << "CPUTemp: " << this->CPUTemp << endl;
        break;
    }
}

void connlost(void *context, char *cause) {
    (void)context; (void)*cause;
    // Please don't modify the Window UI from here
    qDebug() << "Connection Lost" << endl;
}

void MainWindow::on_disconnectButton_clicked()
{
    qDebug() << "Disconnecting from the broker" << endl;
    MQTTClient_disconnect(client, 10000);
    //MQTTClient_destroy(&client);
}
