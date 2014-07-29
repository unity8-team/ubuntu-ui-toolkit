#include <QtQuick>
#include <QElapsedTimer>
#include <cstdio>

int main(int argc, char* argv[]) {
  QGuiApplication app(argc, argv);
  QQuickView view;
  view.setTitle("Test");
  view.setResizeMode(QQuickView::SizeRootObjectToView);
  view.setColor(Qt::black);
  QElapsedTimer timer;
  timer.start();
  view.setSource(QUrl("qrc:///Test.qml"));
  printf("setSource() time: %lld\n", timer.nsecsElapsed());
  view.show();
  return app.exec();
}
