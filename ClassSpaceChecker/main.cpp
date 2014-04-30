#include "stdafx.h"
#include "classspacechecker.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.installEventFilter(&gGlobalEvent);

	ClassSpaceChecker w;
	w.show();
	return a.exec();
}
