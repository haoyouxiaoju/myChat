#include "datacenter.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>
#include <QStandardPaths>



namespace model {

	DataCenter::DataCenter()
		:netClient(this)
	{

		friendList = new QList<UserInfo>();
		chatSessionList = new QList<ChatSessionInfo>();
		applyList = new QList<UserInfo>();
		searchUserResult = new QList<UserInfo>();
		searchMessageResult = new QList<Message>();
		recentMessages = new QHash<QString, QList<Message>>();
		unreadMessageCount = new QHash<QString, int>();


	}

	DataCenter::~DataCenter()
	{
		//��Ҫdeleteǰ��new������QList��QHash
		//
		delete myself;//���˳���ɾ��
		// ...
		// detodo

	}
	DataCenter* DataCenter::getInstance()
	{
		static DataCenter instance;

		return &instance;
	}


	void DataCenter::initDataFile()
	{
		QString f_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "ChatClient.json";
		
		QFile file(f_path);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			qCritical() << POSITION << "Cannot open file:" << file.errorString() <<",file path:" << f_path;
			return;
		}

		//д���ʼ����
		QString data("{\n\n}");
		file.write(data.toUtf8());
		file.close();


	}

	void DataCenter::saveDataFile()
	{
		QString f_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/ChatClient.json";

		QFile file(f_path);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			qCritical() << POSITION << "Cannot open file:" << file.errorString() <<",file path:" << f_path;
			return;
		}

		QJsonObject jsonObject;
		jsonObject["loginSessionId"] = loginSessionId;

		QJsonObject jsonUnread;
		for (auto it = unreadMessageCount->begin(); it != unreadMessageCount->end(); ++it) {
			jsonUnread[it.key()] = it.value();
		}
		jsonObject["unread"] = jsonUnread;

		QJsonDocument jsonDoc(jsonObject);

		QString s = jsonDoc.toJson();
		file.write(s.toUtf8());

		file.close();

	}

	void DataCenter::loadDataFile()
	{
		//  appData Ŀ¼
		QString bashPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

		QString f_path = bashPath + "/ChatClient.json";

		QDir dir;
		if (!dir.exists(bashPath)) {//�ļ��в������򴴽�
			dir.mkpath(bashPath);
		}

		QFileInfo fileInfo(f_path);
		if (!fileInfo.exists()) {//�ļ��������򴴽�
			initDataFile();
		}

		QFile file(f_path);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qCritical() << POSITION << "Cannot open file:" << file.errorString() <<",file path:" << f_path;
			return;
		}

		//��ȡ �������� json�ļ�
		QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
		if (jsonDoc.isNull()) {
			qCritical() << POSITION << "Invalid JSON format";
		}

		QJsonObject jObject = jsonDoc.object();

		loginSessionId = jObject["loginSessionId"].toString();

		this->unreadMessageCount->clear();
		QJsonObject unread = jObject["unread"].toObject();
		for (auto i = unread.begin(); i != unread.end(); ++i) {
			(*this->unreadMessageCount)[i.key()] = i.value().toInt();
		}

		//���ǰ������������Ƿ���ȷ
		if (loginSessionId == "") {
			qCritical() << POSITION << "SessionId is null";
			return;
		}


		file.close();

			

	}

}
