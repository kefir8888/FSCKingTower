//Project FSCKingTower: started at 07.10.13

#include <QtGui/QApplication>
#include "FSCKingTower.h"

int main (int argc, char** argv)
	{
	QApplication app (argc, argv);
	FSCKingTower foo;
	foo.show();
	return app.exec();
	}
