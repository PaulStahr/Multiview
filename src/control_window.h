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

class ControlWindow;

struct control_window_update_handler_t : session_updater_t
{
    ControlWindow *_cw;

    control_window_update_handler_t(ControlWindow *cw_) : _cw(cw_){}
    
    bool operator()(SessionUpdateType sut);
};

class CameraObjectModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CameraObjectModel(QObject *parent = 0);
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QList<camera_t*> _data;
    ControlWindow *_cw;
};

class MeshObjectModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    MeshObjectModel(QObject *parent = 0);
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QList<mesh_object_t*> _data;
    ControlWindow *_cw;
};

class ControlWindow : public QMainWindow
{
    Q_OBJECT
private:
    std::shared_ptr<destroy_functor> _exit_handler;
public:
    session_t & _session;
    ControlWindow(session_t &, Ui::ControlWindow &, std::shared_ptr<destroy_functor> exit_handler);
    Ui::ControlWindow & _ui;
    std::mutex _mtx;
    std::vector<std::future<int> > _pending_futures;
    volatile bool updateUiFlag = false;
    void update_session(SessionUpdateType kind);
    ~ControlWindow();
signals:
    void updateUiSignal(int kind);
    
private:
    CameraObjectModel *_cameraModel;
    MeshObjectModel *_meshModel;
    std::shared_ptr<session_updater_t> _update_listener;

public slots:
void playForward();
void playBackward();
void playStop();
void next();
void prev();
void fov(int);
void fov(QString const &);
void crop(bool);
void showFlow(bool);
void showRendered(bool);
void showIndex(bool);
void showPosition(bool);
void showDepth(bool);
void showVisibility(bool);
void showArrows(bool);
void showFramelists(bool);
void positionShowCurser(bool);
void past(int);
void past(QString const &);
void future(int);
void future(QString const &);
void smoothing(int);
void smoothing(QString const &);
void preresolution(QString const &);
void flowFallback(bool);
void flowNormalize(bool);
void flowRotation(bool);
void flowTranslation(bool);
void flowObjects(bool);
void frame(QString const &);
void framesPerStep(QString const &);
void framesPerSecond(QString const &);
void subframes(QString const &);
void indirectRendering(bool);
void motionBlur(QString const &);
void motionBlur(int);
void animating(QString const &);
void culling(QString const &);
void realtime(bool);
void saveScreenshot();
void updateUiFromComponent_impl(QWidget *widget);
void updateUi(int kind);
void updateUi_impl(int kind);
void updateUi();
void updateShader();
void executeCommand();
void redraw();
void debug(bool);
void coordinateSystem(QString const &);
void depthbuffer(QString const &);
void renderedVisibility(bool);
void depthMax(QString const &);
void depthTesting(bool);
void guiAutoUpdate(bool);
void importMesh();
void importAnimation();
void importFramelist();
void exit();
void addCamera();
};
#endif
