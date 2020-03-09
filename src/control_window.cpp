#include "control_window.h"
#include <iostream>
//#include "control.h"

ControlWindow::ControlWindow(session_t & session_, Ui::ControlWindow & ui_) : _session(session_), _ui(ui_)
{}

void ControlWindow::playForward()           {_session._play = 1;}
void ControlWindow::playBackward()          {_session._play = -1;}
void ControlWindow::playStop()              {_session._play = 0;}
void ControlWindow::next()                  {_session._m_frame += _session._frames_per_step;}
void ControlWindow::prev()                  {_session._m_frame -= _session._frames_per_step;}
void ControlWindow::fov(int fov)            {_session._fov = fov;}
void ControlWindow::showFlow(bool valid)    {_session._show_flow = valid;}
void ControlWindow::showRendered(bool valid){_session._show_raytraced = valid;}
void ControlWindow::showIndex(bool valid)   {_session._show_index = valid;}
void ControlWindow::showPosition(bool valid){_session._show_position = valid;}
void ControlWindow::showDepth(bool valid)   {_session._show_depth = valid;}
void ControlWindow::positionShowCurser(bool valid){_session._show_curser = valid;}
void ControlWindow::showArrows(bool valid)  {_session._show_arrows = valid;}
void ControlWindow::past(int frames)        {_session._diffbackward = -frames;}
void ControlWindow::future(int frames)      {_session._diffforward = frames;}
void ControlWindow::smoothing(int frames)   {_session._smoothing = frames;}
void ControlWindow::framesPerSecond(QString const & value){_session._frames_per_second = std::stoi(value.toUtf8().constData());}
void ControlWindow::framesPerStep(QString const & value){_session._frames_per_step = std::stoi(value.toUtf8().constData());}
void ControlWindow::preresolution(QString const & value){_session._preresolution = std::stoi(value.toUtf8().constData());}
void ControlWindow::flowRotation(bool valid){_session._diffrot = valid;}
void ControlWindow::flowTranslation(bool valid){_session._difftrans = valid;}
void ControlWindow::flowObjects(bool valid) {_session._diffobjects = valid;}
void ControlWindow::frame(QString const & frame){_session._m_frame = std::stoi(frame.toUtf8().constData());}
void ControlWindow::updateShader()          {_session._reload_shader = true;}
void ControlWindow::realtime(bool valid)    {_session._realtime = valid;}
void ControlWindow::executeCommand()
{
    exec_env env(IO_UTIL::get_programpath());
    pending_task_t *pending = new pending_task_t(~PendingFlag(0));
    env._pending_tasks.emplace_back(pending);
    exec(_ui.executeText->text().toUtf8().constData(), env, std::cout, _session, *pending);
}

void ControlWindow::saveScreenshot(){
    size_t width = std::stoi(_ui.screenshotWidth->text().toUtf8().constData());
    size_t height = std::stoi(_ui.screenshotHeight->text().toUtf8().constData());
    std::string view = _ui.screenshotView->currentText().toUtf8().constData();
    viewtype_t viewtype;
    if (view == "Rendered")
    {
        viewtype = VIEWTYPE_RENDERED;
    }
    else if (view == "Flow")
    {
        viewtype = VIEWTYPE_FLOW;
    }
    else if (view == "Position")
    {
        viewtype = VIEWTYPE_POSITION;
    }
    else if (view == "Index")
    {
        viewtype = VIEWTYPE_INDEX;
    }
    else if (view == "Depth")
    {
        viewtype = VIEWTYPE_DEPTH;
    }
    else
    {
        throw std::runtime_error("Illegal Argument");
    }
    
    auto f = std::async(std::launch::async, take_save_lazy_screenshot, _ui.screenshotFilename->text().toUtf8().constData(), width, height, _ui.screenshotCamera->currentText().toUtf8().constData(), viewtype, true, std::ref(_session._scene));

    _mtx.lock();
    _pending_futures.push_back(std::move(f));
    _mtx.unlock();
}

void ControlWindow::updateUi(){
    _ui.flowShow->setChecked(_session._show_flow);
    _ui.depthShow->setChecked(_session._show_flow);
    _ui.flowArrowsShow->setChecked(_session._show_arrows);
    _ui.positionShow->setChecked(_session._show_position);
    _ui.indexShow->setChecked(_session._show_index);
    _ui.positionShowCurser->setChecked(_session._show_curser);
    _ui.generalSmoothing->setValue(_session._smoothing);
    _ui.generalFov->setValue(_session._fov);
    _ui.flowPast->setValue(-_session._diffbackward);
    _ui.flowFuture->setValue(_session._diffforward);
    _ui.lineEditFrame->setText(QString::number(_session._m_frame));
    _ui.performancePreresolution->setCurrentText(QString::number(_session._preresolution));
    _ui.screenshotCamera->clear();
    for (camera_t & cam : _session._scene._cameras)
    {
        _ui.screenshotCamera->addItem(QString(cam._name.c_str()));
    }
}
/*void test()
{
    QWidget* TextFinder::loadUiFile()
{
    QUiLoader loader;

    QFile file(":/forms/textfinder.ui");
    file.open(QFile::ReadOnly);

    QWidget *formWidget = loader.load(&file, this);
    file.close();

    return formWidget;
        ui_findButton = findChild<QPushButton*>("findButton");
    ui_textEdit = findChild<QTextEdit*>("textEdit");
    ui_lineEdit = findChild<QLineEdit*>("lineEdit");
}
}*/
