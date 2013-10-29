#include "FSCKingTower.h"
#include "widgets/plotter.h"

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLayout>
#include <QtCore/QTimer>
#include <cmath>

class SineGenerator : public IGenerator
{
	double nextx;
public:
	SineGenerator(): nextx (0)
		{ }

	virtual QPointF next (double xstep = 0.1)
		{
		QPointF result (nextx, std::sin (nextx));
		nextx += xstep;
		return result;
		}
};

FSCKingTower::FSCKingTower() :
	plotter_ (new Plotter (this)),
	sine_generator_ (new SineGenerator),
	generator_timer_ (new QTimer (this))
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

	a = new QAction (this);
	a->setText ("Generate Sine");
	a->setCheckable (true);
	connect (a, SIGNAL(toggled(bool)), SLOT(control_timer(bool)));
	menuBar()->addMenu ("Generate")->addAction (a);
	
	generator_timer_->setInterval (100);
	connect (generator_timer_, SIGNAL(timeout()), SLOT(generate_sine()));
	}

FSCKingTower::~FSCKingTower() { }

void FSCKingTower::generate_sine()
	{
	plotter_->addCurveData (0, sine_generator_->next (0.01 * 2 * M_PI));
	}

void FSCKingTower::control_timer(bool enabled)
{
	if (enabled)
		generator_timer_->start();
	
	else
		generator_timer_->stop();
}

#include "FSCKingTower.moc"
