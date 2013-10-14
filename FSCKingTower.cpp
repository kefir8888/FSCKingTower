#include "FSCKingTower.h"
#include "widgets/plotter.h"

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLayout>

FSCKingTower::FSCKingTower() :
	plotter_ (new Plotter (this))
	{
	setCentralWidget (new QWidget);
	centralWidget()->setLayout (new QVBoxLayout);

	QLayout* layout = centralWidget()->layout();

	QLabel* l = new QLabel (this);
	l->setText ("Hello World!");
	layout->addWidget (l);

	layout->addWidget (plotter_);

	QAction* a = new QAction (this);
	a->setText ("Quit");
	connect (a, SIGNAL (triggered()), SLOT (close()));
	menuBar()->addMenu ("File")->addAction (a);
	}

FSCKingTower::~FSCKingTower() { }

#include "FSCKingTower.moc"
