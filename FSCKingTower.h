#ifndef FSCKingTower_H
#define FSCKingTower_H

#include <QtGui/QMainWindow>

class Plotter;
class FSCKingTower : public QMainWindow
	{
	Q_OBJECT
	public:
	FSCKingTower();
	virtual ~FSCKingTower();

	private:
	Plotter* plotter_;
	};

#endif // FSCKingTower_H
