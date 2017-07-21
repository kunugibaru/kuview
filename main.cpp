#include "stdafx.h"
#include "kuview.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	kuview w;
	w.show();
	return a.exec();
}
