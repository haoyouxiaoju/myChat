#pragma once

#include <QObject>
#include <QStandardPaths>
#include <QFile>
#include <QAudioSource>
#include <QAudioSink>
#include <QMediaDevices>


class SoundRecorder : public QObject
{
	Q_OBJECT
	const QString RECORD_PATH = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + "/sound/tmpRecord.pcm";
	const QString PLAY_PATH = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + "/sound/tmpPlay.wav";

public:
	static SoundRecorder* getInstance();
	~SoundRecorder();
	//开始录音
	void startRecord();
	//停止录音
	void stopRecord();
	//开始播放
	void startPlay(const QByteArray& content);
	//结束播放
	void stopPlay();

signals:
	//录音结束
	void soundRecordDone(const QString& path);
	//播放结束
	void soundPlayDone();

private:
	explicit SoundRecorder(QObject *parent);
	static SoundRecorder* instance;



private:
	//录制
	QFile soundFile;
	QAudioSource* audioSource;
	//播放
	QAudioSink* audioSink;
	QMediaDevices* outputDevices;
	QAudioDevice outputDevice;
	QFile inputFile;
};
