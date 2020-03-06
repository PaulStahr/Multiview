#ifndef CONTROL_WINDOW_H
#define CONTROL_WINDOW_H


#include <QtGui/QGuiApplication>
#include <QtWidgets/QMainWindow>
#include <QtGui/QScreen>
//#include <QtQml/QQmlEngine>
//#include <QtQml/QQmlComponent>
//#include <QtQuick/QQuickWindow>
#include <QtCore/QUrl>
#include <QDebug>
#include "session.h"
#include "control_ui.h"

class ControlWindow : public QMainWindow
{
    Q_OBJECT
public:
    session_t & _session;
    ControlWindow(session_t &, Ui::ControlWindow &);
    Ui::ControlWindow & _ui;
    public slots:
void playForward();
void playBackward();
void playStop();
void next();
void prev();
void fov(int);
void showFlow(bool);
void showRendered(bool);
void showIndex(bool);
void showPosition(bool);
void showDepth(bool);
void showArrows(bool);
void past(int);
void future(int);
void smoothing(int);
void preresolution(QString const &);
void flowRotation(bool);
void flowTranslation(bool);
void flowObjects(bool);
void frame(QString const &);
void framesPerStep(QString const &);
void framesPerSecond(QString const &);
void realtime(bool);
};

/*int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    const auto screens = QGuiApplication::screens();
    for (QScreen *screen : screens)
        screen->setOrientationUpdateMask(Qt::LandscapeOrientation | Qt::PortraitOrientation |
                                         Qt::InvertedLandscapeOrientation | Qt::InvertedPortraitOrientation);
    QQmlEngine engine;
    QQmlComponent component(&engine);
    QQuickWindow::setDefaultAlphaBuffer(true);
    component.loadUrl(QUrl("qrc:///window/window.qml"));
    if ( component.isReady() )
        component.create();
    else
        qWarning() << component.errorString();
    return app.exec();
}*/

#endif
