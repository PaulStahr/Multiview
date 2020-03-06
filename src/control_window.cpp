#include "control_window.h"
#include <iostream>
//#include "control.h"

ControlWindow::ControlWindow(session_t & session_, Ui::ControlWindow & ui_) : _session(session_), _ui(ui_)
{}

void ControlWindow::playForward()
{
    _session._play = 1;
}

void ControlWindow::playBackward(){
    _session._play = -1;
}
    
void ControlWindow::playStop()
{
    _session._play = 0;
}
    
void ControlWindow::next(){
    _session._m_frame += _session._frames_per_step;
}

void ControlWindow::prev(){
    _session._m_frame -= _session._frames_per_step;
}

void ControlWindow::fov(int fov){
    _session._fov = fov;
}

void ControlWindow::showFlow(bool valid){
    _session._show_flow = valid;
}

void ControlWindow::showRendered(bool valid){
    _session._show_raytraced = valid;
}

void ControlWindow::showIndex(bool valid){
    _session._show_index = valid;
}

void ControlWindow::showPosition(bool valid){
    _session._show_position = valid;
}

void ControlWindow::showDepth(bool valid){
    _session._show_depth = valid;
}

void ControlWindow::showArrows(bool valid){_session._show_arrows = valid;}

void ControlWindow::past(int frames){_session._diffbackward = frames;}

void ControlWindow::future(int frames){_session._diffforward = frames;}

void ControlWindow::smoothing(int frames){_session._smoothing = frames;}

void ControlWindow::framesPerSecond(QString const & value){
    _session._frames_per_second = std::stoi(value.toUtf8().constData());
}
void ControlWindow::framesPerStep(QString const & value){
    _session._frames_per_step = std::stoi(value.toUtf8().constData());
}

void ControlWindow::preresolution(QString const & value){
    _session._preresolution = std::stoi(value.toUtf8().constData());
}

void ControlWindow::flowRotation(bool valid){_session._diffrot = valid;}
void ControlWindow::flowTranslation(bool valid){_session._difftrans = valid;}
void ControlWindow::flowObjects(bool valid){_session._diffobjects = valid;}
void ControlWindow::frame(QString const & frame){_session._m_frame = std::stoi(frame.toUtf8().constData());}

void ControlWindow::realtime(bool valid){_session._realtime = valid;}
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
