// CrowLauncher.h
// Copyright (c) 2010 Sunside Inc., All Rights Reserved
// Author: Joe Riedel
// See Crow/LICENSE for licensing terms.

#pragma once
#include <QtGui/QMainWindow>
#include <QtGui/QDialog>
#include <QtCore/QRect>
#include <Engine/Packages/PackagesDef.h>
#include <Engine/Sound/SoundDef.h>
#include <Engine/Persistence.h>
#include <Runtime/Time.h>

class QPixmap;
class QTimerEvent;
class QComboBox;
class QCheckBox;
class QPushButton;

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
	void GraphicsSettings();

private:

	void LoadSettings();
	void Center(const QRect &rect);
	void PlayMusic();
	bool LoadPixmap(const char *filename, QPixmap &pixmap);

	xtime::SecondsTimer<> m_timer;
	SoundRef m_music;
	SoundContextRef m_soundContext;
	Persistence::Ref m_settings;
	Persistence::Ref m_defaults;
};

///////////////////////////////////////////////////////////////////////////////

class CrowGraphicsSettings : public QDialog {
	Q_OBJECT
public:
	CrowGraphicsSettings(
		const Persistence::Ref &settings,
		const Persistence::Ref &defaults,
		QWidget *parent = 0
	);

	virtual void done(int r);

private slots:

	void ResetToDefaults();
	void UpdateEnabledState();

private:

	int m_defaultResolution;
	bool m_defaultFullscreen;

	int m_savedResolution;
	bool m_savedFullscreen;

	Persistence::Ref m_settings;
	QComboBox *m_resolutions;
	QCheckBox *m_fullscreen;
	QPushButton *m_ok;
	QPushButton *m_reset;
};
