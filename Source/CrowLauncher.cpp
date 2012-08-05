// CrowLauncher.cpp
// Copyright (c) 2010 Sunside Inc., All Rights Reserved
// Author: Joe Riedel
// See Crow/LICENSE for licensing terms.

#include "CrowApp.h"
#include "CrowLauncher.h"
#include <QtGui/QApplication>
#include <QtGui/QBoxLayout>
#include <QtGui/QSpacerItem>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QDesktopWidget>
#include <QtCore/QProcess>
#include <Runtime/Time.h>
#include <Engine/Packages/Packages.h>
#include <Engine/Sound/Sound.h>
#include <cstdlib>

#undef qApp

int CrowApp::DoLauncher() {
	int argc = App::Get()->argc;
	const char **argv = App::Get()->argv;
	QApplication qApp(argc, (char**)argv);
	QCoreApplication::setOrganizationName(App::Get()->company.get());
	QCoreApplication::setOrganizationDomain(App::Get()->website.get());
	QCoreApplication::setApplicationName(App::Get()->title.get());

	CrowLauncher launcher;
	launcher.Run();
	qApp.exec();
	return 0;
}

CrowLauncher::CrowLauncher() {
	setWindowFlags(
		Qt::Window|
		Qt::CustomizeWindowHint|
		Qt::WindowTitleHint|
		Qt::WindowSystemMenuHint|
		Qt::WindowCloseButtonHint|
		Qt::MSWindowsFixedSizeDialogHint
	);
	setAttribute(Qt::WA_QuitOnClose);

	QWidget *w = new QWidget();
	QVBoxLayout *l = new QVBoxLayout(w);
	QLabel *banner = new QLabel();

	{
		QPixmap p;
		if (LoadPixmap("launcher.png", p))
			banner->setPixmap(p);
	}

	l->addWidget(banner);
	l->addSpacing(32);

	QPushButton *b = new QPushButton("Play");
	l->addWidget(b);
	RAD_VERIFY(connect(b, SIGNAL(clicked()), SLOT(Play())));

	b = new QPushButton("Video Settings...");
	l->addWidget(b);
	RAD_VERIFY(connect(b, SIGNAL(clicked()), SLOT(VideoSettings())));

	b = new QPushButton("Visit Us On Facebook...");
	l->addWidget(b);
	RAD_VERIFY(connect(b, SIGNAL(clicked()), SLOT(GotoFacebook())));

	b = new QPushButton("Quit");
	l->addWidget(b);
	RAD_VERIFY(connect(b, SIGNAL(clicked()), SLOT(close())));

	setCentralWidget(w);

	QDesktopWidget *desktop = QApplication::desktop();
	if (desktop)
		Center(desktop->screenGeometry());

	setWindowTitle((CStr(App::Get()->title) + " Launcher").c_str.get());
}

bool CrowLauncher::Run() {
	m_soundContext = SoundContext::New(App::Get()->engine->sys->alDriver);
	PlayMusic();
	show();
	startTimer(1);
	return true;
}

void CrowLauncher::timerEvent(QTimerEvent*) {
	float elapsed = 0.1f;
	if (m_timer.IsTiming()) {
		elapsed = m_timer.Elapsed();
		m_timer.Stop();
		m_timer.Start();
	} else {
		m_timer.Start();
	}

	if (m_soundContext)
		m_soundContext->Tick(elapsed, true);
}

void CrowLauncher::Play() {
#if defined(RAD_OPT_SHIP)
	const char *exe = "Crow.exe";
#elif defined(RAD_OPT_MACHINE_SIZE_32)
#if defined(RAD_OPT_DEBUG)
	const char *exe = "Bin/Golden - Debug - Win32/Crow.exe";
#else
	const char *exe = "Bin/Golden - Release - Win32/Crow.exe";
#endif
#else
#if defined(RAD_OPT_DEBUG)
	const char *exe = "Bin/Golden - Debug - x64/Crow.exe";
#else
	const char *exe = "Bin/Golden - Release - x64/Crow.exe";
#endif
#endif
	if (QProcess::startDetached(exe, QStringList() << "-go"))
		close();
}

void CrowLauncher::GotoFacebook() {
	App::Get()->LaunchURL("http://www.facebook.com/sunsidegames");
}

void CrowLauncher::VideoSettings() {
}

void CrowLauncher::Center(const QRect &rect) {
	move(rect.x()+((rect.width()-width())/2), rect.y()+((rect.height()-height())/2));
}

void CrowLauncher::PlayMusic() {
	enum { kNumSongs = 5 };
	static char *s_songs[kNumSongs] = {
		"Audio/EntertheFight",
		"Audio/ch2_audio",
		"Audio/demon_background",
		"Audio/rain_wind_thunder01",
		"Audio/cave_drips1"
	};

	srand((unsigned int)xtime::ReadMilliseconds());
	int idx = rand() % kNumSongs;

	pkg::Asset::Ref asset = App::Get()->engine->sys->packages->Resolve(s_songs[idx], pkg::Z_Engine);
	if (!asset)
		return;

	int r = asset->Process(
		xtime::TimeSlice::Infinite,
		pkg::P_Load
	);

	if (r != pkg::SR_Success)
		return;

	asset->Process(
		xtime::TimeSlice::Infinite,
		pkg::P_Trim
	);

	m_music = m_soundContext->NewSound(asset);
	if (m_music)
		m_music->Play(SC_Music, 0);
}

bool CrowLauncher::LoadPixmap(const char *filename, QPixmap &pixmap) {
	int media = file::HDD;
	file::HBufferedAsyncIO buf;

	int enabledMedia = App::Get()->engine->sys->files->enabledMedia;
	App::Get()->engine->sys->files->enabledMedia = file::Paks | file::HDD;

	int r = App::Get()->engine->sys->files->LoadFile(
		filename,
		media,
		buf,
		file::HIONotify()
	);

	App::Get()->engine->sys->files->enabledMedia = enabledMedia;

	if (r == file::Pending) {
		buf->WaitForCompletion();
		r = buf->result;
	}

	if (r != file::Success) 
		return false;

	return pixmap.loadFromData(
		(const uchar*)buf->data->ptr.get(),
		(uint)buf->data->size.get()
	);
}

#include "moc_CrowLauncher.cc"
