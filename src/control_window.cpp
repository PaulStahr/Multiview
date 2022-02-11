#include "control_window.h"
#include <iostream>
#include <charconv>
#include <system_error>
#include "lang.h"
#include <QtWidgets/QFileDialog>

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
    auto [ptr, ec]{std::from_chars(&*str.begin(), &*str.end(), value)};
    return ec == std::errc();
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


ControlWindow::ControlWindow(session_t & session_, Ui::ControlWindow & ui_, std::shared_ptr<destroy_functor> exit_handler_) : _exit_handler(exit_handler_), _session(session_), _ui(ui_)
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
        _data[index.row()]->_visible = (Qt::CheckState)value.toInt() == Qt::Checked;_cw->update_session(UPDATE_SCENE); return true;
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
        case 1:if (role == Qt::CheckStateRole){_data[index.row()]->_visible =   (Qt::CheckState)value.toInt() == Qt::Checked; _cw->update_session(UPDATE_SCENE);return true;}break;
        case 2:if (role == Qt::EditRole)      {safe_stoi(_data[index.row()]->_id, value.toString());                          _cw->update_session(UPDATE_SCENE);return true;}break;
        case 3:if (role == Qt::CheckStateRole){_data[index.row()]->_diffrot =   (Qt::CheckState)value.toInt() == Qt::Checked; _cw->update_session(UPDATE_SCENE);return true;}break;
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
void ControlWindow::fov(int fov)                          {_session._fov = fov;                             updateUiFromComponent_impl(_ui.generalFov);}
void ControlWindow::fov(QString const & fov)              {safe_stof(_session._fov, fov);                   updateUiFromComponent_impl(_ui.generalFovText);}
void ControlWindow::crop(bool valid)                      {_session._crop = valid;                          update_session(UPDATE_SESSION);}
void ControlWindow::showFlow(bool valid)                  {_session._show_flow = valid;                     update_session(UPDATE_SESSION);}
void ControlWindow::showRendered(bool valid)              {_session._show_raytraced = valid;                update_session(UPDATE_SESSION);}
void ControlWindow::showIndex(bool valid)                 {_session._show_index = valid;                    update_session(UPDATE_SESSION);}
void ControlWindow::showPosition(bool valid)              {_session._show_position = valid;                 update_session(UPDATE_SESSION);}
void ControlWindow::showDepth(bool valid)                 {_session._show_depth = valid;                    update_session(UPDATE_SESSION);}
void ControlWindow::positionShowCurser(bool valid)        {_session._show_curser = valid;                   update_session(UPDATE_SESSION);}
void ControlWindow::showArrows(bool valid)                {_session._show_arrows = valid;                   update_session(UPDATE_SESSION);}
void ControlWindow::showFramelists(bool valid)            {_session._show_framelists = valid;               update_session(UPDATE_SESSION);}
void ControlWindow::past(int frames)                      {_session._diffbackward = -frames;                updateUiFromComponent_impl(_ui.flowPast);}
void ControlWindow::past(QString const & frames)          {safe_stoi(_session._diffbackward, frames);       updateUiFromComponent_impl(_ui.flowPastText);}
void ControlWindow::future(int frames)                    {_session._diffforward = frames;                  updateUiFromComponent_impl(_ui.flowFuture);}
void ControlWindow::future(QString const & frames)        {safe_stoi(_session._diffforward, frames);        updateUiFromComponent_impl(_ui.flowFutureText);}
void ControlWindow::flowFallback(bool valid)              {_session._difffallback = valid;                  update_session(UPDATE_SESSION);}
void ControlWindow::flowNormalize(bool valid)             {_session._diffnormalize = valid;                 update_session(UPDATE_SESSION);}
void ControlWindow::smoothing(int frames)                 {_session._smoothing = frames;                    updateUiFromComponent_impl(_ui.generalSmoothing);}
void ControlWindow::smoothing(QString const & frames)     {safe_stoi(_session._smoothing        , frames);  updateUiFromComponent_impl(_ui.generalSmoothingText);}
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
void ControlWindow::depthMax(QString const & value)       {safe_stof(_session._depth_scale, value);         update_session(UPDATE_SESSION);}
void ControlWindow::renderedVisibility(bool valid)        {_session._show_rendered_visibility = valid;      update_session(UPDATE_SESSION);}
void ControlWindow::depthTesting(bool valid)              {_session._depth_testing = valid;                 update_session(UPDATE_SESSION);}
void ControlWindow::guiAutoUpdate(bool valid)             {_session._auto_update_gui = valid;               update_session(UPDATE_SESSION);updateUi();}

void ControlWindow::importMesh()
{
    QString fileName = QFileDialog::getOpenFileName(this,
    tr("Open Image"), "/home/", tr("Wavefront file (*.obj)"));
    std::string sFileName = fileName.toUtf8().constData();
    mesh_object_t m = mesh_object_t("OBJ", sFileName);
    {
        std::lock_guard<std::mutex> lck(_session._scene._mtx);
        _session._scene._objects.push_back(std::move(m));
    }
    _session.scene_update(UPDATE_SCENE);
}

void ControlWindow::importAnimation()
{
}

void ControlWindow::importFramelist()
{
    QString fileName = QFileDialog::getOpenFileName(this,
    tr("Open Image"), "/home/", tr("Wavefront file (*.obj)"));
    std::string sFileName = fileName.toUtf8().constData();
    std::ifstream framefile(sFileName);
    std::vector<size_t> framelist = IO_UTIL::parse_framelist(framefile);
    {
        std::lock_guard<std::mutex> lck(_session._scene._mtx);
        _session._scene._framelists.emplace_back(sFileName, framelist);
    }
    framefile.close();
    _session.scene_update(UPDATE_SCENE);
}

void ControlWindow::exit()
{
    _session._exit_program = true;
    _session.scene_update(UPDATE_REDRAW);
}

void ControlWindow::addCamera()
{
    _session._scene._cameras.push_back(camera_t("cam"));
    _session.scene_update(UPDATE_SCENE);
}

void ControlWindow::update_session(SessionUpdateType kind)
{
    if (this->updateUiFlag){return;}
    this->updateUiFlag = true;
    _session.scene_update(kind);
    this->updateUiFlag = false;
}

void ControlWindow::coordinateSystem(QString const & value)
{
    auto iter = std::find_if(coordinate_system_values, coordinate_system_values + 3, [&value](auto elem){return elem.second == value;});
    if (iter != coordinate_system_values + 3){_session._coordinate_system = iter->first;}
    update_session(UPDATE_SESSION);
}

void ControlWindow::culling(QString const & value)
{
    auto iter = std::find_if(culling_values, culling_values + 4, [&value](auto elem){return elem.second == value;});
    if (iter != culling_values + 4){_session._culling = iter->first;}
    update_session(UPDATE_SESSION);
}

void ControlWindow::animating(QString const & value)
{
    auto iter = std::find_if(redraw_scedule_values, redraw_scedule_values + 3, [&value](auto elem){return elem.second == value;});
    if (iter != redraw_scedule_values + 3){_session._animating = iter->first;}
    _session.scene_update(UPDATE_ANIMATING);
}

void ControlWindow::depthbuffer(QString const & value)
{
    auto iter = std::find_if(depthbuffer_values, depthbuffer_values + 3, [&value](auto elem){return elem.second == value;});
    if (iter != depthbuffer_values + 3){_session._depthbuffer_size = iter->first;}
    _session.scene_update(UPDATE_SESSION);
}

void ControlWindow::executeCommand()
{
    exec_env env(IO_UTIL::get_programpath());
    std::string command = _ui.executeText->text().toUtf8().constData();
    pending_task_t *pending = new pending_task_t(~PendingFlag(0), command);
    env._pending_tasks.emplace_back(pending);
    exec(command, std::vector<std::string>(), env, std::cout, _session, *pending);
}

void ControlWindow::saveScreenshot(){
    size_t width;
    safe_stoi(width, _ui.screenshotWidth->text());
    size_t height;
    safe_stoi(height, _ui.screenshotHeight->text());
    std::string view = _ui.screenshotView->currentText().toUtf8().constData();
    bool prerendering = _ui.screenshotPrerendering->isChecked();
    auto iter = std::find_if(viewtype_values, viewtype_values + 5, [&view](auto elem){return elem.second == view;});
    if (iter == viewtype_values + 5){throw std::runtime_error("Illegal Argument");}
    viewtype_t viewtype = iter->first;
    
    if (prerendering)
    {
        for (int i = 0; i < 6; ++i)
        {
            std::string filename = _ui.screenshotFilename->text().toUtf8().constData();
            auto pindex = filename.find_last_of(".");
            if (pindex == std::string::npos)
            {
                pindex = filename.size();
                filename += ".exr";
            }
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
        delete this;
    }
    else if (_session._auto_update_gui)
    {
        updateUi_impl(kind);
    }
}

void ControlWindow::updateUiFromComponent_impl(QWidget *widget)
{
    if (widget){update_session(UPDATE_SESSION);}
    if (this->updateUiFlag){return;}
    this->updateUiFlag = true;
    if (widget == _ui.generalSmoothing || widget == _ui.generalSmoothingText || !widget)
    {
        if (widget != _ui.generalSmoothing)     {_ui.generalSmoothing->setValue(_session._smoothing);}
        if (widget != _ui.generalSmoothingText) {_ui.generalSmoothingText->setText(QString::number(_session._smoothing));}
    }
    if (widget == _ui.generalFov || widget == _ui.generalFovText || !widget)
    {
        if (widget != _ui.generalFov)           {_ui.generalFov->setValue(_session._fov);}
        if (widget != _ui.generalFovText)       {_ui.generalFovText->setText(QString::number(_session._fov));}
    }
    if (widget == _ui.flowFuture || widget == _ui.flowFutureText || !widget)
    {
        if (widget != _ui.flowFuture)               {_ui.flowFuture->setValue(_session._diffforward);}
        if (widget != _ui.flowFutureText)           {_ui.flowFutureText->setText(QString::number(_session._diffforward));}
    }
    if (widget == _ui.flowPast || widget == _ui.flowPastText || !widget)
    {
        if (widget != _ui.flowPast)                 {_ui.flowPast->setValue(-_session._diffbackward);}
        if (widget != _ui.flowPastText)             {_ui.flowPastText->setText(QString::number(_session._diffbackward));}
    }
    this->updateUiFlag = false;
}

void ControlWindow::updateUi_impl(int kind)
{
    if (this->updateUiFlag){return;}
    if (kind & UPDATE_FRAME)
    {
        _ui.lineEditFrame->setText(QString::number(_session._m_frame));
    }
    if (kind & (UPDATE_SESSION | UPDATE_ANIMATING))
    {
        const char* text = lang::get_animating_string(_session._animating);
        if (!text){throw std::runtime_error("Invalid animation-strategy selection");}
        _ui.performanceAnimation->setCurrentText(text);
    }
    if (kind & UPDATE_SESSION)
    {
        _ui.checkBoxGuiAutoupdate->setChecked(_session._auto_update_gui);
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
        this->updateUiFlag = false;
        updateUiFromComponent_impl(nullptr);
        this->updateUiFlag = true;
        _ui.checkBoxDebug->setChecked(_session._debug);
        _ui.depthScaleText->setText(QString::number(_session._depth_scale));
        _ui.checkBoxDepthTesting->setChecked(_session._depth_testing);
        _ui.renderedVisibility->setChecked(_session._show_rendered_visibility);
        _ui.checkBoxCrop->setChecked(_session._crop);
        _ui.flowFallback->setChecked(_session._difffallback);
        _ui.flowNormalize->setChecked(_session._diffnormalize);
        _ui.lineEditFrame->setText(QString::number(_session._m_frame));
        {
            auto iter = std::find_if(culling_values, culling_values + 4, [this](auto elem){return elem.first == _session._culling;});
            if (iter != culling_values + 4){_ui.performanceCulling->setCurrentText(iter->second);}
            else{throw std::runtime_error("Invalid culling selection");}
        }
        {
            auto iter = std::find_if(coordinate_system_values, coordinate_system_values + 3, [this](auto elem){return elem.first == _session._coordinate_system;});
            if (iter != coordinate_system_values + 3){_ui.coordinateSystem->setCurrentText(iter->second);}
            else{throw std::runtime_error("Invalid coordinate-system selection");}
        }
        {
            auto iter = std::find_if(depthbuffer_values, depthbuffer_values + 3, [this](auto elem){return elem.first == _session._depthbuffer_size;});
            if (iter != depthbuffer_values + 3){_ui.performanceDepthbuffer->setCurrentIndex(std::distance(depthbuffer_values, iter));}
            else{throw std::runtime_error("Invalid depthbuffer selection");}
        }
        _ui.performancePreresolution->setCurrentText(QString::number(_session._preresolution));
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
