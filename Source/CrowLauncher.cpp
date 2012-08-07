// CrowLauncher.cpp
// Copyright (c) 2010 Sunside Inc., All Rights Reserved
// Author: Joe Riedel
// See Crow/LICENSE for licensing terms.

#include "CrowApp.h"
#include "CrowLauncher.h"
#include <Runtime/Runtime.h>
#include <QtGui/QApplication>
#include <QtGui/QBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QSpacerItem>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QDesktopWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QComboBox>
#include <QtGui/QCheckBox>
#include <QtGui/QMessageBox>
#include <QtCore/QProcess>
#include <QtCore/QTimer>
#include <Runtime/Time.h>
#include <Engine/Packages/Packages.h>
#include <Engine/Sound/Sound.h>
#include <cstdlib>
#include <unistd.h>

#if defined(RAD_OPT_DEBUG)
#include <Runtime/File.h>
#endif

#if defined(RAD_OPT_OSX)
void __OSX_BundlePath(char *dst);
void __OSX_SpawnSandboxed(const char *application);
#endif

#undef qApp

int App::DoLauncher(int argc, const char **argv) {
	rt::Initialize();
	RAD_DEBUG_ONLY(file::EnforcePortablePaths(false));
	
	QApplication qApp(argc, (char**)argv);
	
	COut(C_Info) << "LauncherMain..." << std::endl;
	COut(C_Info) << "echo command line: ";
	
	for (int i = 0; i < argc; ++i) {
		COut(C_Info) << argv[i] << " ";
	}
	
	COut(C_Info) << std::endl;
	
	App *app = App::Get(argc, (const char **)argv);
	
	if (!app->PreInit()) {
		QMessageBox::critical(0, "Error", "Initialization failed! See log.txt for details.");
		return 1;
	}
	
	QCoreApplication::setOrganizationName(App::Get()->company.get());
	QCoreApplication::setOrganizationDomain(App::Get()->website.get());
	QCoreApplication::setApplicationName(App::Get()->title.get());
	
	CrowLauncher *launcher = new CrowLauncher();
	launcher->Run();
	qApp.exec();
	
	delete launcher;
	
	app->Finalize();
	App::DestroyInstance();
	rt::Finalize();
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

	m_play = new QPushButton("Play");
	l->addWidget(m_play);
	RAD_VERIFY(connect(m_play, SIGNAL(clicked()), SLOT(Play())));

	QPushButton *b = new QPushButton("Graphics Settings...");
	l->addWidget(b);
	RAD_VERIFY(connect(b, SIGNAL(clicked()), SLOT(GraphicsSettings())));

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

CrowLauncher::~CrowLauncher() {
	if (m_music)
		m_music->Stop();
	m_music.reset();
	m_soundContext.reset();
}

bool CrowLauncher::Run() {
	LoadSettings();
	m_soundContext = SoundContext::New(App::Get()->engine->sys->alDriver);
	PlayMusic();
	show();
	startTimer(1);
	return true;
}

void CrowLauncher::timerEvent(QTimerEvent*) {
	float elapsed = 0.1f;
	if (m_timer.IsTiming()) {
		elapsed = (float)m_timer.Elapsed();
		m_timer.Stop();
		m_timer.Start();
	} else {
		m_timer.Start();
	}

	if (m_soundContext)
		m_soundContext->Tick(elapsed, true);
}

void CrowLauncher::Play() {
#if defined(RAD_OPT_WIN)
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
	if (QProcess::startDetached(exe, QStringList() << "-nolauncher"))
		close();
#elif defined(RAD_OPT_OSX)
#if defined(RAD_OPT_SHIP)
	__OSX_SpawnSandboxed("com.sunsidegames.crowmac");
#else
	__OSX_BundlePath(bundle);
	
	String cmd;
	cmd.Printf("open -n -a %s --args -nolauncher\"", bundle);
	//cmd = CStr("/bin/sh -c \"sleep 1;open -a \\\"textedit\\\"\"");

	COut(C_Debug) << "Spawning '" << cmd.c_str.get() << "'" << std::endl;
	
	QProcess::execute(cmd.c_str.get());
	close();
#endif
#else
#error RAD_ERROR_UNSUP_PLAT
#endif
}

void CrowLauncher::GotoFacebook() {
	App::Get()->LaunchURL("http://www.facebook.com/sunsidegames");
}

void CrowLauncher::GraphicsSettings() {
	CrowGraphicsSettings d(m_settings, m_defaults, this);
	d.exec();
}

void CrowLauncher::LoadSettings() {
	m_defaults = Persistence::Load(0);
	m_defaults->keys->pairs[String("fullscreen")] = String("true");
	
	DisplayDevice::Ref dd = App::Get()->primaryDisplay;
	const r::VidMode *defMode = dd->defVidMode;

	String s;
	s.Printf("%dx%d", defMode->w, defMode->h);

	m_defaults->keys->pairs[String("vidMode")] = s;

	m_settings = Persistence::Load("settings.prefs");
	if (m_settings->keys->pairs.empty()) {
		m_settings = m_defaults->Clone();
		m_settings->Save("settings.prefs");
	}
}

void CrowLauncher::Center(const QRect &rect) {
	move(rect.x()+((rect.width()-width())/2), rect.y()+((rect.height()-height())/2));
}

void CrowLauncher::PlayMusic() {
	enum { kNumSongs = 5 };
	static const char *s_songs[kNumSongs] = {
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

///////////////////////////////////////////////////////////////////////////////

inline int MakeRes(int x, int y) {
	return x | (y<<16);
}

inline void GetRes(int z, int &x, int &y) {
	x = z & 0xffff;
	y = (z>>16) & 0xffff;
}

CrowGraphicsSettings::CrowGraphicsSettings(
	const Persistence::Ref &settings, 
	const Persistence::Ref &defaults, 
	QWidget *parent)
: QDialog(parent), 
  m_settings(settings), 
  m_defaultResolution(0), 
  m_defaultFullscreen(false),
  m_savedResolution(0),
  m_savedFullscreen(false) {

	setWindowTitle("Graphics Settings");

	setWindowFlags(
		Qt::Window|
		Qt::CustomizeWindowHint|
		Qt::WindowTitleHint|
		Qt::WindowCloseButtonHint|
		Qt::MSWindowsFixedSizeDialogHint
	);

	QGridLayout *grid = new QGridLayout(this);
	QHBoxLayout *l = new QHBoxLayout();

	QGroupBox *g = new QGroupBox("Resolution");
	m_resolutions = new QComboBox();

	DisplayDevice::Ref dd = App::Get()->primaryDisplay;
	RAD_ASSERT(dd);

	const r::VidMode *defMode = dd->defVidMode;

	const char *vidModeString = settings->keys->StringForKey("vidMode");
	r::VidMode selectMode;
	selectMode.bpp = 32;

	sscanf(vidModeString, "%dx%d", &selectMode.w, &selectMode.h);

	vidModeString = defaults->keys->StringForKey("vidMode");
	r::VidMode defaultMode;
	defaultMode.bpp = 32;

	sscanf(vidModeString, "%dx%d", &defaultMode.w, &defaultMode.h);

	for (r::VidModeVec::const_iterator it = dd->vidModes->begin(); it != dd->vidModes->end(); ++it) {
		const r::VidMode &m = *it;

		QString s;
		s.sprintf("%dx%d", m.w, m.h);

		if (!m.SameAspect(*defMode))
			s += " [Stretched]";

		m_resolutions->addItem(s, MakeRes(m.w, m.h));

		if (defaultMode.SameSize(m))
			m_defaultResolution = (int)(it-dd->vidModes->begin());
		if (selectMode.SameSize(m))
			m_savedResolution = (int)(it-dd->vidModes->begin());
	}

	if (m_savedResolution != -1)
		m_resolutions->setCurrentIndex(m_savedResolution);

	RAD_VERIFY(connect(m_resolutions, SIGNAL(currentIndexChanged(int)), SLOT(UpdateEnabledState())));

	QHBoxLayout *il = new QHBoxLayout();
	il->addWidget(m_resolutions);
	g->setLayout(il);
	l->addWidget(g);

	m_defaultFullscreen = m_settings->keys->BoolForKey("fullscreen");
	m_savedFullscreen = m_settings->keys->BoolForKey("fullscreen");
	m_fullscreen = new QCheckBox("Fullscreen");
	m_fullscreen->setChecked(m_savedFullscreen);
	RAD_VERIFY(connect(m_fullscreen, SIGNAL(stateChanged(int)), SLOT(UpdateEnabledState())));

	l->addWidget(m_fullscreen);
	grid->addLayout(l, 0, 0);

	l = new QHBoxLayout();

	m_ok = new QPushButton("OK");
	RAD_VERIFY(connect(m_ok, SIGNAL(clicked()), SLOT(accept())));
	l->addWidget(m_ok);

	QPushButton *b = new QPushButton("Cancel");
	RAD_VERIFY(connect(b, SIGNAL(clicked()), SLOT(reject())));
	b->setDefault(true);
	l->addWidget(b);

	m_reset = new QPushButton("Reset To Defaults");
	RAD_VERIFY(connect(m_reset, SIGNAL(clicked()), SLOT(ResetToDefaults())));
	l->addWidget(m_reset);
	grid->addLayout(l, 1, 0);

	UpdateEnabledState();
}

void CrowGraphicsSettings::done(int r) {
	if (r == QDialog::Accepted) {
		int idx = m_resolutions->currentIndex();
		int w, h;
		GetRes(m_resolutions->itemData(idx).toInt(), w, h);

		String s;
		s.Printf("%dx%d", w, h);
		m_settings->keys->pairs[String("vidMode")] = s;

		if (m_fullscreen->checkState() == Qt::Checked) {
			s = String("true");
		} else {
			s = String("false");
		}

		m_settings->keys->pairs[String("fullscreen")] = s;
		m_settings->Save();
	}
	QDialog::done(r);
}

void CrowGraphicsSettings::ResetToDefaults() {
	m_resolutions->setCurrentIndex(m_defaultResolution);
	m_fullscreen->setChecked(m_defaultFullscreen);
	UpdateEnabledState();
}

void CrowGraphicsSettings::UpdateEnabledState() {
	int idx = m_resolutions->currentIndex();
	bool fullscreen = m_fullscreen->checkState() == Qt::Checked;
	m_ok->setEnabled((idx != m_savedResolution) || (fullscreen != m_savedFullscreen));
	m_reset->setEnabled((idx != m_defaultResolution) || (fullscreen != m_defaultFullscreen));
}

#include "moc_CrowLauncher.cc"
