#ifndef FSCKingTower_H
#define FSCKingTower_H

#include <QtGui/QMainWindow>

class IGenerator
	{
	public:
	virtual QPointF next (double xstep = 0.1) = 0;
	};

class Plotter;
class FSCKingTower : public QMainWindow
	{
	Q_OBJECT
	public:
	FSCKingTower();
	virtual ~FSCKingTower();

	private:
	Plotter* plotter_;
	IGenerator* sine_generator_;

	private slots:
	void generate_sine();
	};

#endif // FSCKingTower_H
