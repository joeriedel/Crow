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

#if defined(RAD_OPT_OSX)
#include <unistd.h>
#endif

#if defined(RAD_OPT_DEBUG)
#include <Runtime/File.h>
#endif

#if defined(RAD_OPT_OSX)
void __OSX_BundlePath(char *dst);
#endif

#undef qApp

namespace {

bool LoadPixmap(const char *filename, QPixmap &pixmap) {
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

}

int CrowApp::DoLauncher() {

	int _argc = argc;
	QApplication *qApp = new QApplication(_argc, (char**)argv.get());
#if defined(RAD_OPT_WIN)
	{
		QPixmap icon;
		if (LoadPixmap("icon.tga", icon))
			qApp->setWindowIcon(QIcon(icon));
	}
#endif

	COut(C_Info) << "Enter Launcher..." << std::endl;
	
	for (int i = 0; i < argc; ++i) {
		COut(C_Info) << argv[i] << " ";
	}
	
	COut(C_Info) << std::endl;
	
	App *app = App::Get();
	
	QCoreApplication::setOrganizationName(app->company.get());
	QCoreApplication::setOrganizationDomain(app->website.get());
	QCoreApplication::setApplicationName(app->title.get());
	
	CrowLauncher *launcher = new CrowLauncher();
	launcher->Run();
	qApp->exec();
	
	int r = launcher->playClicked ? 1 : 0;
	
	delete launcher;
	
#if !defined(RAD_OPT_OSX)
	delete qApp; // leave the NSApp intact, we will use it in the OSX event loop.
#endif
	
	return r;
}

CrowLauncher::CrowLauncher() : m_playClicked(false) {
	setWindowFlags(
		Qt::Window|
		Qt::CustomizeWindowHint|
		Qt::WindowTitleHint|
		Qt::WindowSystemMenuHint|
		Qt::WindowCloseButtonHint|
		Qt::MSWindowsFixedSizeDialogHint
	);
	setAttribute(Qt::WA_QuitOnClose);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QWidget *w = new QWidget();
	QVBoxLayout *l = new QVBoxLayout(w);
	
	int bkg = SelectBackground();
	
	if (bkg == 0) {
		l->addSpacing(165); // push to bottom.
		QHBoxLayout *h = new QHBoxLayout();
		h->addSpacing(64);
		QVBoxLayout *buttons = new QVBoxLayout();
		CreateButtons(buttons);
		h->addLayout(buttons);
		h->addSpacing(64);
		l->addLayout(h);
	} else {
		QHBoxLayout *left = new QHBoxLayout();
		
		left->addSpacing(255);
		QVBoxLayout *buttons = new QVBoxLayout();
		CreateButtons(buttons);
		left->addLayout(buttons);
		
		l->addLayout(left);
		l->addSpacing(165);
	}

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

QSize CrowLauncher::sizeHint() const {
	return QSize(534, 360);
}

int CrowLauncher::SelectBackground() {
	const char *s_images[] = {
		"launcher1.png",
		"launcher2.png"
	};
	
	int idx = (xtime::ReadMilliseconds() / 1000) & 1;
	
	QPixmap bg;
	if (LoadPixmap(s_images[idx], bg)) {
		QPalette p(palette());
		p.setBrush(QPalette::Background, bg);
		setPalette(p);
	}
	
	return idx;
}

void CrowLauncher::CreateButtons(QVBoxLayout *l) {

	m_play = new QPushButton("Play");
	l->addWidget(m_play);
	RAD_VERIFY(connect(m_play, SIGNAL(clicked()), SLOT(Play())));

	QPushButton *b = new QPushButton("Graphics Settings...");
	l->addWidget(b);
	RAD_VERIFY(connect(b, SIGNAL(clicked()), SLOT(GraphicsSettings())));

	b = new QPushButton("Visit Us On Facebook...");
	l->addWidget(b);
	RAD_VERIFY(connect(b, SIGNAL(clicked()), SLOT(GotoFacebook())));
	
	b = new QPushButton("Buy Soundtrack...");
	l->addWidget(b);
	RAD_VERIFY(connect(b, SIGNAL(clicked()), SLOT(BuySoundtrack())));

	b = new QPushButton("Quit");
	l->addWidget(b);
	RAD_VERIFY(connect(b, SIGNAL(clicked()), SLOT(close())));
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
	m_playClicked = true;
	close();
}

void CrowLauncher::GotoFacebook() {
	App::Get()->LaunchURL("http://www.facebook.com/sunsidegames");
}

void CrowLauncher::BuySoundtrack() {
	App::Get()->LaunchURL("http://itunes.apple.com/us/album/crow-original-soundtrack/id520981840");
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

		if (!m.SameAspect(*defMode))
			continue;
		
		if (defaultMode.SameSize(m))
			m_defaultResolution = m_resolutions->count();
		if (selectMode.SameSize(m))
			m_savedResolution = m_resolutions->count();
			
		QString s;
		s.sprintf("%dx%d", m.w, m.h);
		
		m_resolutions->addItem(s, MakeRes(m.w, m.h));
	}
	  
	for (r::VidModeVec::const_iterator it = dd->vidModes->begin(); it != dd->vidModes->end(); ++it) {
		const r::VidMode &m = *it;

		if (m.SameAspect(*defMode))
			continue;

		if (defaultMode.SameSize(m))
			m_defaultResolution = m_resolutions->count();
		if (selectMode.SameSize(m))
			m_savedResolution = m_resolutions->count();
			
		QString s;
		s.sprintf("%dx%d [Stretched]", m.w, m.h);
		
		m_resolutions->addItem(s, MakeRes(m.w, m.h));
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
