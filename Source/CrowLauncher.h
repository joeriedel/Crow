// CrowLauncher.h
// Copyright (c) 2010 Sunside Inc., All Rights Reserved
// Author: Joe Riedel
// See Crow/LICENSE for licensing terms.

#pragma once
#include <QtGui/QMainWindow>
#include <QtCore/QRect>
#include <Engine/Packages/PackagesDef.h>
#include <Engine/Sound/SoundDef.h>
#include <Runtime/Time.h>

class QPixmap;
class QTimerEvent;

class CrowLauncher : public QMainWindow {
	Q_OBJECT
public:

	CrowLauncher();

	bool Run();

protected:

	virtual void timerEvent(QTimerEvent*);

private slots:

	void Play();
	void GotoFacebook();
	void VideoSettings();

private:

	void Center(const QRect &rect);
	void PlayMusic();
	bool LoadPixmap(const char *filename, QPixmap &pixmap);

	xtime::SecondsTimer<> m_timer;
	SoundRef m_music;
	SoundContextRef m_soundContext;
};
