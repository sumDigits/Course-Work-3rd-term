#include <QApplication>
#include "arcana.h"

int main(int argc, char *argv[]) {

  QApplication app(argc, argv);

  Arcana window;

  window.resize(300, 400);
  window.setWindowTitle("Project");
  window.show();

  return app.exec();
}
