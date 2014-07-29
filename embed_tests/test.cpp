#include <QtQuick>
#include <QElapsedTimer>
#include <cstdio>

int main(int argc, char* argv[]) {
  QElapsedTimer timer;
  timer.start();
  QGuiApplication app(argc, argv);
  QQuickView view;
  view.setTitle("Test");
  view.setResizeMode(QQuickView::SizeRootObjectToView);
  view.setColor(Qt::black);
  view.setSource(QUrl("qrc:///Test.qml"));
  view.show();
  qint64 sourceTime = timer.nsecsElapsed();
  printf("setSource() time: %lld\n", sourceTime);
  return app.exec();
}
