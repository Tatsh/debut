#include <QApplication>
#include "debutform.h"

int main(int argc, char *argv[]) {

  QApplication app(argc, argv);

  DebutForm debut;
  debut.show();

  return app.exec();
}
