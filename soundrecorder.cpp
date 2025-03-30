#include "soundrecorder.h"
#include <QDir>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QMediaDevices>
#include "model/data.h"
#include "toast.h"

SoundRecorder* SoundRecorder::instance = nullptr;



SoundRecorder::SoundRecorder(QObject *parent)
	: QObject(parent)
{
	//创建目录
	QDir soundRootPath(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation));
	bool isOk = soundRootPath.mkdir("sound");
	if (!isOk) {
		LOG() << "创建文件夹失败";
	}
	//初始化录制模块
	soundFile.setFileName(RECORD_PATH);
	QAudioFormat inputFormat;
	inputFormat.setSampleRate(48000);
	inputFormat.setChannelCount(2);
	inputFormat.setSampleFormat(QAudioFormat::Float);

	QAudioDevice info = QMediaDevices::defaultAudioInput();
	auto supportedFormats = info.supportedSampleFormats();
	for (const auto & format : supportedFormats) {
		LOG() << "Supported format:"
			<< "Sample rate:" << format;
	}
	if (!info.isFormatSupported(inputFormat))
	{
		LOG() << "Default format not supported, trying to use the nearest.";
		return;
	}
	audioSource = new QAudioSource(inputFormat, this);
	connect(audioSource, &QAudioSource::stateChanged, this, [this](QAudio::State state) {
		if (state == QAudio::StoppedState)
		{
			//录音结束
			if (audioSource->error() != QAudio::NoError)
			{
				LOG() << "录音失败"<<audioSource->error();
				return;
			}
		}
		});

	//
	//初始化播放模块
	outputDevices = new QMediaDevices(this);
	outputDevice = outputDevices->defaultAudioOutput();
	QAudioFormat outputFormat;
	outputFormat.setSampleRate(48000);
	outputFormat.setChannelCount(2);
	outputFormat.setSampleFormat(QAudioFormat::Float);
	auto supportedFormats1 = outputDevice.supportedSampleFormats();
	for (const auto& format : supportedFormats1) {
		LOG() << "Supported format:"
			<< "Sample rate:" << format;
	}
	if (!outputDevice.isFormatSupported(outputFormat)) {
		LOG() << "播放格式不支持";
		return;
	}

	audioSink = new QAudioSink(outputDevice, outputFormat);
	 
	connect(audioSink, &QAudioSink::stateChanged, this, [this](QtAudio::State state) {
		if (state == QtAudio::IdleState) {
			LOG() << "idleState";
			this->stopPlay();
			emit this->soundPlayDone();
		}
		else if (state == QtAudio::StoppedState) {
			LOG() << "ActiveState";
		}
		else if (state == QtAudio::StoppedState) {
			LOG() << "StoppedState";
			if (audioSink->error() != QtAudio::NoError) {
				LOG() << audioSink->error();
			}
		}
		});



}

SoundRecorder* SoundRecorder::getInstance()
{
	if (instance == nullptr)
	{
		instance = new SoundRecorder(nullptr);
	}
	return instance;
}

SoundRecorder::~SoundRecorder()
{
}

void SoundRecorder::startRecord()
{
	bool isOk = soundFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
	isOk = soundFile.isOpen();
	LOG() << "is Ok = " << isOk;

	audioSource->start(&soundFile);

}

void SoundRecorder::stopRecord()
{
	audioSource->stop();
	soundFile.close();
	emit this->soundRecordDone(RECORD_PATH);
}

void SoundRecorder::startPlay(const QByteArray& content)
{
	if (content.isEmpty()) {
		Toast::showMessage("数据加载中");
		return;
	}
	//数据存入临时文件
	model::writeQByteArrayToFile(PLAY_PATH, content);
	//播放
	inputFile.setFileName(PLAY_PATH);
	inputFile.open(QIODevice::ReadOnly);
	audioSink->start(&inputFile);

}

void SoundRecorder::stopPlay()
{
	audioSink->stop();
	inputFile.close();
}

