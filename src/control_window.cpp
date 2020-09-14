#include "control_window.h"
#include <iostream>
//#include "control.h"

template <typename T>
bool safe_stof(T & value, const std::string& str)
{
    try {
        value = std::stof(str);
        return true;
    }
    catch (const std::invalid_argument& ia) {return false;}
    catch (const std::out_of_range& oor)    {return false;}
    catch (const std::exception& e)         {return false;}
}

template <typename T>
bool safe_stoi(T & value, const std::string& str)
{
    try {
        value = std::stoi(str);
        return true;
    }
    catch (const std::invalid_argument& ia) {return false;}
    catch (const std::out_of_range& oor)    {return false;}
    catch (const std::exception& e)         {return false;}
}

template <typename T>
bool safe_stof(T & value, const QString& str)
{
    return safe_stof(value, std::string(str.toUtf8().constData()));
}

template <typename T>
bool safe_stoi(T & value, const QString& str)
{
    return safe_stoi(value, std::string(str.toUtf8().constData()));
}


ControlWindow::ControlWindow(session_t & session_, Ui::ControlWindow & ui_) : _session(session_), _ui(ui_)
{
    _update_listener = [this](SessionUpdateType sut){
        if (sut == UPDATE_SESSION || sut == UPDATE_FRAME || sut == UPDATE_ANIMATING || sut == UPDATE_SCENE || sut == UPDATE_REDRAW)
        {
            this->updateUiSignal(static_cast<int>(sut));
        }
    };
    session_._updateListener.emplace_back(&_update_listener);
    _ui.setupUi(this);
    
    _cameraModel = new CameraObjectModel(this);
    _ui.cameraTableView->setModel(_cameraModel);
    _ui.cameraTableView->horizontalHeader()->setVisible(true);
    _ui.cameraTableView->show();
    
    _meshModel = new MeshObjectModel(this);
    _ui.meshTableView->setModel(_meshModel);
    _ui.meshTableView->horizontalHeader()->setVisible(true);
    _ui.meshTableView->show();
}

bool CameraObjectModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    if (role == Qt::CheckStateRole)
    {
        _data[index.row()]->_visible = (Qt::CheckState)value.toInt() == Qt::Checked;
        return true;
    }
    return false;
}

CameraObjectModel::CameraObjectModel(QObject *parent) : QAbstractTableModel(parent)
{
    _cw = dynamic_cast<ControlWindow*>(parent);
}

int CameraObjectModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _data.length();
}

int CameraObjectModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

Qt::ItemFlags CameraObjectModel::flags(const QModelIndex & index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    switch (index.column())
    {
        case 0: return flags;
        case 1: return flags | Qt::ItemIsUserCheckable;
        default: throw std::runtime_error("Unknown column");
    }
    return flags;
}

QVariant CameraObjectModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    switch (index.column()) {
        case 0:if (role == Qt::DisplayRole){return QString(_data[index.row()]->_name.c_str());}break;
        case 1:if (role ==Qt::CheckStateRole){return _data[index.row()]->_visible ? Qt::Checked : Qt::Unchecked;}break;
        default: throw std::runtime_error("Unknown column");
    }
    return QVariant();
}

QVariant CameraObjectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section)
        {
            case 0: return QString("Name");
            case 1: return QString("Visible");
            default: throw std::runtime_error("Unknown column");
        }
    }
    return QVariant();
}

bool MeshObjectModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    switch (index.column())
    {
        case 0:break;
        case 1:if (role == Qt::CheckStateRole){_data[index.row()]->_visible = (Qt::CheckState)value.toInt() == Qt::Checked;_cw->update_session(UPDATE_SCENE);return true;}break;
        case 2:if (role == Qt::EditRole){safe_stoi(_data[index.row()]->_id, value.toString()); _cw->update_session(UPDATE_SCENE);return true;}break;
        case 3:if (role == Qt::CheckStateRole){_data[index.row()]->_diffrot = (Qt::CheckState)value.toInt() == Qt::Checked; _cw->update_session(UPDATE_SCENE);return true;}break;
        case 4:if (role == Qt::CheckStateRole){_data[index.row()]->_difftrans = (Qt::CheckState)value.toInt() == Qt::Checked; _cw->update_session(UPDATE_SCENE);return true;}break;
        default: throw std::runtime_error("Unknown column");            
    }
    return false;
}

MeshObjectModel::MeshObjectModel(QObject *parent) : QAbstractTableModel(parent)
{
    _cw = dynamic_cast<ControlWindow*>(parent);
}

int MeshObjectModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _data.length();
}

int MeshObjectModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

Qt::ItemFlags MeshObjectModel::flags(const QModelIndex & index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    switch (index.column())
    {
        case 0: return flags;
        case 1: return flags | Qt::ItemIsUserCheckable;
        case 2: return flags | Qt::ItemIsEditable;
        case 3: return flags | Qt::ItemIsUserCheckable;
        case 4: return flags | Qt::ItemIsUserCheckable;
        default: throw std::runtime_error("Unknown column");
    }
}

QVariant MeshObjectModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    switch (index.column()) {
        case 0:if (role == Qt::DisplayRole){return QString(_data[index.row()]->_name.c_str());}break;
        case 1:if (role ==Qt::CheckStateRole){return _data[index.row()]->_visible ? Qt::Checked : Qt::Unchecked;}break;
        case 2:if (role == Qt::DisplayRole){return QString::number(_data[index.row()]->_id);}break;
        case 3:if (role ==Qt::CheckStateRole){return _data[index.row()]->_diffrot ? Qt::Checked : Qt::Unchecked;}break;
        case 4:if (role ==Qt::CheckStateRole){return _data[index.row()]->_difftrans ? Qt::Checked : Qt::Unchecked;}break;
        default: throw std::runtime_error("Unknown column");
    }
    return QVariant();
}

QVariant MeshObjectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section)
        {
            case 0: return QString("Name");
            case 1: return QString("Visible");
            case 2: return QString("Id");
            case 3: return QString("DiffR");
            case 4: return QString("DiffT");
            default: throw std::runtime_error("Unknown column");
        }
    }
    return QVariant();
}

void ControlWindow::playForward()                         {_session._play = 1; update_session(UPDATE_SESSION);}
void ControlWindow::playBackward()                        {_session._play = -1;update_session(UPDATE_SESSION);}
void ControlWindow::playStop()                            {_session._play = 0; update_session(UPDATE_SESSION);}
void ControlWindow::next()                                {_session._m_frame += _session._frames_per_step; _ui.lineEditFrame->setText(QString::number(_session._m_frame));update_session(UPDATE_FRAME);}
void ControlWindow::prev()                                {_session._m_frame -= _session._frames_per_step; _ui.lineEditFrame->setText(QString::number(_session._m_frame));update_session(UPDATE_FRAME);}
void ControlWindow::fov(int fov)                          {_session._fov = fov;                             update_session(UPDATE_SESSION);}
void ControlWindow::fov(QString const & fov)              {safe_stoi(_session._fov, fov);                   update_session(UPDATE_SESSION);}
void ControlWindow::showFlow(bool valid)                  {_session._show_flow = valid;                     update_session(UPDATE_SESSION);}
void ControlWindow::showRendered(bool valid)              {_session._show_raytraced = valid;                update_session(UPDATE_SESSION);}
void ControlWindow::showIndex(bool valid)                 {_session._show_index = valid;                    update_session(UPDATE_SESSION);}
void ControlWindow::showPosition(bool valid)              {_session._show_position = valid;                 update_session(UPDATE_SESSION);}
void ControlWindow::showDepth(bool valid)                 {_session._show_depth = valid;                    update_session(UPDATE_SESSION);}
void ControlWindow::positionShowCurser(bool valid)        {_session._show_curser = valid;                   update_session(UPDATE_SESSION);}
void ControlWindow::showArrows(bool valid)                {_session._show_arrows = valid;                   update_session(UPDATE_SESSION);}
void ControlWindow::showFramelists(bool valid)            {_session._show_framelists = valid;               update_session(UPDATE_SESSION);}
void ControlWindow::past(int frames)                      {_session._diffbackward = -frames;                update_session(UPDATE_SESSION);}
void ControlWindow::past(QString const & frames)          {safe_stoi(_session._diffbackward, frames);       update_session(UPDATE_SESSION);}
void ControlWindow::future(int frames)                    {_session._diffforward = frames;                  update_session(UPDATE_SESSION);}
void ControlWindow::future(QString const & frames)        {safe_stoi(_session._diffforward, frames);        update_session(UPDATE_SESSION);}
void ControlWindow::flowFallback(bool valid)              {_session._difffallback = valid;                  update_session(UPDATE_SESSION);}
void ControlWindow::flowNormalize(bool valid)             {_session._diffnormalize = valid;                 update_session(UPDATE_SESSION);}
void ControlWindow::smoothing(int frames)                 {_session._smoothing = frames;                    update_session(UPDATE_SESSION);}
void ControlWindow::smoothing(QString const & frames)     {safe_stoi(_session._smoothing        , frames);  update_session(UPDATE_SESSION);}
void ControlWindow::framesPerSecond(QString const & value){safe_stoi(_session._frames_per_second,  value);  update_session(UPDATE_SESSION);}
void ControlWindow::framesPerStep(QString const & value)  {safe_stoi(_session._frames_per_step  ,  value);  update_session(UPDATE_SESSION);}
void ControlWindow::preresolution(QString const & value)  {safe_stoi(_session._preresolution    ,  value);  update_session(UPDATE_SESSION);}
void ControlWindow::flowRotation(bool valid)              {_session._diffrot = valid;                       update_session(UPDATE_SESSION);}
void ControlWindow::flowTranslation(bool valid)           {_session._difftrans = valid;                     update_session(UPDATE_SESSION);}
void ControlWindow::flowObjects(bool valid)               {_session._diffobjects = valid;                   update_session(UPDATE_SESSION);}
void ControlWindow::frame(QString const & frame)          {safe_stoi(_session._m_frame,frame);              update_session(UPDATE_FRAME);}
void ControlWindow::updateShader()                        {_session._reload_shader = true;                  update_session(UPDATE_SESSION);}
void ControlWindow::realtime(bool valid)                  {_session._realtime = valid;                      update_session(UPDATE_SESSION);}
void ControlWindow::debug(bool valid)                     {_session._debug = valid;                         update_session(UPDATE_SESSION);}
void ControlWindow::approximated(bool valid)              {_session._approximated = valid;                  update_session(UPDATE_SESSION);}
void ControlWindow::depthMax(QString const & value)       {safe_stof(_session._depth_scale, value);         update_session(UPDATE_SESSION);}
void ControlWindow::renderedVisibility(bool valid)        {_session._show_rendered_visibility = valid;      update_session(UPDATE_SESSION);}
void ControlWindow::depthTesting(bool valid)              {_session._depth_testing = valid;                 update_session(UPDATE_SESSION);}
void ControlWindow::guiAutoUpdate(bool valid)             {_session._auto_update_gui = valid;               update_session(UPDATE_SESSION);updateUi();}

void ControlWindow::update_session(SessionUpdateType kind)
{
    if (this->updateUiFlag)
    {
        return;
    }
    this->updateUiFlag = true;
    _session.scene_update(kind);
    this->updateUiFlag = false;
}

void ControlWindow::culling(QString const & value)
{
    if      (value == "None")           {_session._culling = 0;}
    else if (value == "Front")          {_session._culling = 1;}
    else if (value == "Back")           {_session._culling = 2;}
    else if (value == "Front and Back") {_session._culling = 3;}
    update_session(UPDATE_SESSION);
}

void ControlWindow::animating(QString const & value)
{
    if      (value == "Always")     {_session._animating = REDRAW_ALWAYS;}
    else if (value == "Automatic")  {_session._animating = REDRAW_AUTOMATIC;}
    else if (value == "Manual")     {_session._animating = REDRAW_MANUAL;}
    else                            {throw std::runtime_error("Unknown Key " + std::string(value.toUtf8().constData()));}
    _session.scene_update(UPDATE_ANIMATING);
}
void ControlWindow::executeCommand()
{
    exec_env env(IO_UTIL::get_programpath());
    pending_task_t *pending = new pending_task_t(~PendingFlag(0));
    env._pending_tasks.emplace_back(pending);
    exec(_ui.executeText->text().toUtf8().constData(), std::vector<std::string>(), env, std::cout, _session, *pending);
}

void ControlWindow::depthbuffer(QString const & depthstr)
{
    depthbuffer_size_t depth;
    if      (depthstr == "16 bit")  {depth = DEPTHBUFFER_16_BIT;}
    else if (depthstr == "24 bit")  {depth = DEPTHBUFFER_24_BIT;}
    else if (depthstr == "32 bit")  {depth = DEPTHBUFFER_32_BIT;}
    else                            {throw std::runtime_error("Illegal Argument");}
    _session._depthbuffer_size = depth;
    _session.scene_update(UPDATE_SESSION);
}

void ControlWindow::saveScreenshot(){
    size_t width;
    safe_stoi(width, _ui.screenshotWidth->text());
    size_t height;
    safe_stoi(height, _ui.screenshotHeight->text());
    std::string view = _ui.screenshotView->currentText().toUtf8().constData();
    bool prerendering = _ui.screenshotPrerendering->isChecked();
    viewtype_t viewtype;
    if      (view == "Rendered") {viewtype = VIEWTYPE_RENDERED;}
    else if (view == "Flow")     {viewtype = VIEWTYPE_FLOW;}
    else if (view == "Position") {viewtype = VIEWTYPE_POSITION;}
    else if (view == "Index")    {viewtype = VIEWTYPE_INDEX;}
    else if (view == "Depth")    {viewtype = VIEWTYPE_DEPTH;}
    else                         {throw std::runtime_error("Illegal Argument");}
    
    if (prerendering)
    {
        for (int i = 0; i < 6; ++i)
        {
            std::string filename = _ui.screenshotFilename->text().toUtf8().constData();
            auto pindex = filename.find_last_of(".");
            filename = filename.substr(0, pindex) + std::to_string(i) + filename.substr(pindex);
            auto f = std::async(std::launch::async, take_save_lazy_screenshot, filename, width, height, _ui.screenshotCamera->currentText().toUtf8().constData(), viewtype, true, i, std::vector<std::string>(), std::ref(_session._scene));
            
            std::lock_guard<std::mutex> lck(_session._scene._mtx);
            _pending_futures.push_back(std::move(f));
        }
    }
    else
    {
        auto f = std::async(std::launch::async, take_save_lazy_screenshot, _ui.screenshotFilename->text().toUtf8().constData(), width, height, _ui.screenshotCamera->currentText().toUtf8().constData(), viewtype, true, std::numeric_limits<size_t>::max(), std::vector<std::string>(), std::ref(_session._scene));
        
        std::lock_guard<std::mutex> lck(_session._scene._mtx);
        _pending_futures.push_back(std::move(f));
    }
}

void ControlWindow::updateUi(){updateUi_impl(UPDATE_SESSION | UPDATE_FRAME | UPDATE_SCENE);}
void ControlWindow::updateUi(int kind){
    if (_session._exit_program)
    {
        if (_session._loglevel > 2)
        {
            std::cout <<"close control"<< std::endl;
        }
        close();
    }
    if (_session._auto_update_gui)
    {
        updateUi_impl(kind);
    }
}

void ControlWindow::updateUi_impl(int kind)
{
    if (this->updateUiFlag)
    {
        return;
    }
    this->updateUiFlag = true;
    if (kind & UPDATE_FRAME)
    {
        _ui.lineEditFrame->setText(QString::number(_session._m_frame));
    }
    if (kind & UPDATE_SESSION)
    {   _ui.checkBoxGuiAutoupdate->setChecked(_session._auto_update_gui);
        _ui.renderedShow->setChecked(_session._show_raytraced);
        _ui.flowShow->setChecked(_session._show_flow);
        _ui.depthShow->setChecked(_session._show_depth);
        _ui.flowArrowsShow->setChecked(_session._show_arrows);
        _ui.framelistsShow->setChecked(_session._show_framelists);
        _ui.flowRotation->setChecked(_session._diffrot);
        _ui.flowTranslation->setChecked(_session._difftrans);
        _ui.flowObjects->setChecked(_session._diffobjects);
        _ui.positionShow->setChecked(_session._show_position);
        _ui.indexShow->setChecked(_session._show_index);
        _ui.positionShowCurser->setChecked(_session._show_curser);
        _ui.generalSmoothing->setValue(_session._smoothing);
        _ui.checkBoxDebug->setChecked(_session._debug);
        _ui.checkBoxApproximated->setChecked(_session._approximated);
        _ui.depthScaleText->setText(QString::number(_session._depth_scale));
        _ui.checkBoxDepthTesting->setChecked(_session._depth_testing);
        _ui.renderedVisibility->setChecked(_session._show_rendered_visibility);
        _ui.generalSmoothingText->setText(QString::number(_session._smoothing));
        _ui.generalFov->setValue(_session._fov);
        _ui.generalFovText->setText(QString::number(_session._fov));
        _ui.flowPast->setValue(-_session._diffbackward);
        _ui.flowPastText->setText(QString::number(_session._diffbackward));
        _ui.flowFuture->setValue(_session._diffforward);
        _ui.flowFallback->setChecked(_session._difffallback);
        _ui.flowNormalize->setChecked(_session._diffnormalize);
        _ui.flowFutureText->setText(QString::number(_session._diffforward));
        _ui.lineEditFrame->setText(QString::number(_session._m_frame));
        switch(_session._culling)
        {
            case 0:_ui.performanceCulling->setCurrentText("None");          break;
            case 1:_ui.performanceCulling->setCurrentText("Front");         break;
            case 2:_ui.performanceCulling->setCurrentText("Back");          break;
            case 3:_ui.performanceCulling->setCurrentText("Front and Back");break;
            default: throw std::runtime_error("Illegal culling selection");
        }   
        
        _ui.performancePreresolution->setCurrentText(QString::number(_session._preresolution));
        {
            uint8_t index = 255;
            switch(_session._depthbuffer_size)
            {
                case DEPTHBUFFER_16_BIT: index = 0; break;
                case DEPTHBUFFER_24_BIT: index = 1; break;
                case DEPTHBUFFER_32_BIT: index = 2; break;
            }
            _ui.performanceDepthbuffer->setCurrentIndex(index);
        }
    }
    if (kind & UPDATE_SCENE)
    {
        _ui.screenshotCamera->clear();
        for (camera_t & cam : _session._scene._cameras)
        {
            _ui.screenshotCamera->addItem(QString(cam._name.c_str()));
        }
        {
            std::lock_guard<std::mutex> lck(_session._scene._mtx);
            QList<camera_t*> & data = _cameraModel->_data;
            data.clear();
            for (camera_t & cam : _session._scene._cameras)
            {
                data.append(&cam);
            }
            QModelIndex topLeft = _cameraModel->index(0, 0);
            QModelIndex bottomRight = _cameraModel->index(_cameraModel->rowCount() - 1, _cameraModel->columnCount() - 1);
            emit _cameraModel->dataChanged(topLeft, bottomRight);
            emit _cameraModel->layoutChanged();
        }
        {
            std::lock_guard<std::mutex> lck(_session._scene._mtx);
            QList<mesh_object_t*> & data = _meshModel->_data;
            data.clear();
            for (mesh_object_t & cam : _session._scene._objects)
            {
                data.append(&cam);
            }
            QModelIndex topLeft = _meshModel->index(0, 0);
            QModelIndex bottomRight = _meshModel->index(_meshModel->rowCount() - 1, _meshModel->columnCount() - 1);
            emit _meshModel->dataChanged(topLeft, bottomRight);
            emit _meshModel->layoutChanged();
        }
    }
    this->updateUiFlag = false;
}

void ControlWindow::redraw()
{
    _session.scene_update(UPDATE_REDRAW);
}
