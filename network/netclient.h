#pragma once
#include <QNetworkAccessManager>
#include <QWebSocket>
#include <QProtobufSerializer>
#include <QObject>

#include "../model/datacenter.h"

class NetClient : public QObject
{
	Q_OBJECT

public:
	NetClient(model::DataCenter *dataCenter);
	~NetClient();


private:
	QNetworkAccessManager httpClient;
	QWebSocket websocketClient;

	model::DataCenter* dataCenter;

	//protobuf ÐòÁÐ»¯Æ÷
	QProtobufSerializer serializer;
	


};
