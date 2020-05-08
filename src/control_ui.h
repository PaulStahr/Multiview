/********************************************************************************
** Form generated from reading UI file 'control_ui.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef CONTROL_UI_H
#define CONTROL_UI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlWindow
{
public:
    QAction *actionTest;
    QWidget *centralwidget;
    QFrame *frame;
    QCheckBox *flowShow;
    QCheckBox *flowArrowsShow;
    QLabel *label;
    QSlider *flowFuture;
    QSlider *flowPast;
    QLabel *label_2;
    QLabel *label_3;
    QCheckBox *flowTranslation;
    QCheckBox *flowRotation;
    QCheckBox *flowObjects;
    QLineEdit *flowFutureText;
    QLineEdit *flowPastText;
    QFrame *frame_2;
    QLabel *label_4;
    QCheckBox *depthShow;
    QLineEdit *depthScaleText;
    QLabel *label_25;
    QFrame *frame_3;
    QLabel *label_5;
    QCheckBox *renderedShow;
    QCheckBox *renderedVisibility;
    QFrame *frame_4;
    QLabel *label_6;
    QCheckBox *indexShow;
    QFrame *frame_5;
    QLabel *label_7;
    QCheckBox *positionShow;
    QCheckBox *positionShowCurser;
    QFrame *frame_6;
    QLabel *label_8;
    QSlider *generalFov;
    QLabel *label_9;
    QLabel *label_11;
    QSlider *generalSmoothing;
    QLineEdit *generalSmoothingText;
    QLineEdit *generalFovText;
    QCheckBox *checkBoxDepthTesting;
    QFrame *frame_7;
    QLabel *label_10;
    QPushButton *buttonNext;
    QPushButton *buttonPrev;
    QPushButton *buttonForward;
    QPushButton *buttonBackward;
    QPushButton *buttonStop;
    QLineEdit *lineEditFrame;
    QLabel *label_15;
    QLineEdit *lineEditFramesPerStep;
    QCheckBox *checkBoxRealtime;
    QLineEdit *lineEditFramesPerSecond;
    QLabel *label_16;
    QFrame *frame_8;
    QLabel *label_12;
    QLabel *label_17;
    QLabel *label_18;
    QLineEdit *screenshotWidth;
    QLineEdit *screenshotHeight;
    QLabel *label_19;
    QLabel *label_20;
    QLabel *label_21;
    QLineEdit *screenshotFilename;
    QComboBox *screenshotView;
    QPushButton *buttonSaveScreenshot;
    QComboBox *screenshotCamera;
    QCheckBox *screenshotPrerendering;
    QLabel *label_23;
    QFrame *frame_9;
    QLabel *label_13;
    QComboBox *performancePreresolution;
    QLabel *label_14;
    QLabel *label_22;
    QComboBox *performanceAnimation;
    QLabel *label_24;
    QComboBox *performanceDepthbuffer;
    QFrame *frame_10;
    QPushButton *buttonUpdateUi;
    QPushButton *buttonUpdateShader;
    QLineEdit *executeText;
    QPushButton *executeButton;
    QPushButton *buttonRedraw;
    QOpenGLWidget *openGLWidget;
    QCheckBox *checkBoxDebug;
    QCheckBox *checkBoxApproximated;
    QFrame *frame_11;
    QLabel *label_26;
    QCheckBox *visibilityShow;
    QTableView *cameraTableView;
    QTableView *meshTableView;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ControlWindow)
    {
        if (ControlWindow->objectName().isEmpty())
            ControlWindow->setObjectName(QStringLiteral("ControlWindow"));
        ControlWindow->resize(463, 709);
        actionTest = new QAction(ControlWindow);
        actionTest->setObjectName(QStringLiteral("actionTest"));
        centralwidget = new QWidget(ControlWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        frame = new QFrame(centralwidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(10, 10, 331, 81));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        flowShow = new QCheckBox(frame);
        flowShow->setObjectName(QStringLiteral("flowShow"));
        flowShow->setGeometry(QRect(10, 30, 71, 23));
        flowArrowsShow = new QCheckBox(frame);
        flowArrowsShow->setObjectName(QStringLiteral("flowArrowsShow"));
        flowArrowsShow->setGeometry(QRect(10, 50, 71, 23));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 10, 67, 17));
        flowFuture = new QSlider(frame);
        flowFuture->setObjectName(QStringLiteral("flowFuture"));
        flowFuture->setGeometry(QRect(80, 60, 71, 16));
        flowFuture->setMinimum(0);
        flowFuture->setMaximum(10);
        flowFuture->setValue(1);
        flowFuture->setOrientation(Qt::Horizontal);
        flowFuture->setInvertedAppearance(false);
        flowFuture->setInvertedControls(false);
        flowPast = new QSlider(frame);
        flowPast->setObjectName(QStringLiteral("flowPast"));
        flowPast->setGeometry(QRect(80, 30, 71, 16));
        flowPast->setMaximum(10);
        flowPast->setValue(1);
        flowPast->setOrientation(Qt::Horizontal);
        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(90, 10, 67, 17));
        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(80, 40, 67, 17));
        flowTranslation = new QCheckBox(frame);
        flowTranslation->setObjectName(QStringLiteral("flowTranslation"));
        flowTranslation->setGeometry(QRect(220, 10, 91, 23));
        flowRotation = new QCheckBox(frame);
        flowRotation->setObjectName(QStringLiteral("flowRotation"));
        flowRotation->setGeometry(QRect(220, 30, 81, 23));
        flowObjects = new QCheckBox(frame);
        flowObjects->setObjectName(QStringLiteral("flowObjects"));
        flowObjects->setGeometry(QRect(220, 50, 91, 23));
        flowFutureText = new QLineEdit(frame);
        flowFutureText->setObjectName(QStringLiteral("flowFutureText"));
        flowFutureText->setGeometry(QRect(160, 50, 41, 21));
        flowPastText = new QLineEdit(frame);
        flowPastText->setObjectName(QStringLiteral("flowPastText"));
        flowPastText->setGeometry(QRect(160, 30, 41, 21));
        frame_2 = new QFrame(centralwidget);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setGeometry(QRect(10, 100, 121, 81));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        label_4 = new QLabel(frame_2);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 10, 67, 17));
        depthShow = new QCheckBox(frame_2);
        depthShow->setObjectName(QStringLiteral("depthShow"));
        depthShow->setGeometry(QRect(10, 30, 71, 23));
        depthScaleText = new QLineEdit(frame_2);
        depthScaleText->setObjectName(QStringLiteral("depthScaleText"));
        depthScaleText->setGeometry(QRect(10, 50, 41, 16));
        label_25 = new QLabel(frame_2);
        label_25->setObjectName(QStringLiteral("label_25"));
        label_25->setGeometry(QRect(60, 50, 51, 17));
        frame_3 = new QFrame(centralwidget);
        frame_3->setObjectName(QStringLiteral("frame_3"));
        frame_3->setGeometry(QRect(350, 10, 101, 81));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        label_5 = new QLabel(frame_3);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 10, 67, 17));
        renderedShow = new QCheckBox(frame_3);
        renderedShow->setObjectName(QStringLiteral("renderedShow"));
        renderedShow->setGeometry(QRect(10, 30, 61, 23));
        renderedVisibility = new QCheckBox(frame_3);
        renderedVisibility->setObjectName(QStringLiteral("renderedVisibility"));
        renderedVisibility->setGeometry(QRect(10, 50, 91, 23));
        frame_4 = new QFrame(centralwidget);
        frame_4->setObjectName(QStringLiteral("frame_4"));
        frame_4->setGeometry(QRect(270, 100, 81, 81));
        frame_4->setFrameShape(QFrame::StyledPanel);
        frame_4->setFrameShadow(QFrame::Raised);
        label_6 = new QLabel(frame_4);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(10, 10, 51, 17));
        indexShow = new QCheckBox(frame_4);
        indexShow->setObjectName(QStringLiteral("indexShow"));
        indexShow->setGeometry(QRect(10, 30, 61, 23));
        frame_5 = new QFrame(centralwidget);
        frame_5->setObjectName(QStringLiteral("frame_5"));
        frame_5->setGeometry(QRect(140, 100, 121, 81));
        frame_5->setFrameShape(QFrame::StyledPanel);
        frame_5->setFrameShadow(QFrame::Raised);
        label_7 = new QLabel(frame_5);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(10, 10, 67, 17));
        positionShow = new QCheckBox(frame_5);
        positionShow->setObjectName(QStringLiteral("positionShow"));
        positionShow->setGeometry(QRect(10, 30, 71, 23));
        positionShowCurser = new QCheckBox(frame_5);
        positionShowCurser->setObjectName(QStringLiteral("positionShowCurser"));
        positionShowCurser->setGeometry(QRect(10, 50, 71, 23));
        frame_6 = new QFrame(centralwidget);
        frame_6->setObjectName(QStringLiteral("frame_6"));
        frame_6->setGeometry(QRect(230, 490, 201, 91));
        frame_6->setFrameShape(QFrame::StyledPanel);
        frame_6->setFrameShadow(QFrame::Raised);
        label_8 = new QLabel(frame_6);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(10, 10, 67, 17));
        generalFov = new QSlider(frame_6);
        generalFov->setObjectName(QStringLiteral("generalFov"));
        generalFov->setGeometry(QRect(80, 30, 61, 16));
        generalFov->setMaximum(180);
        generalFov->setValue(90);
        generalFov->setOrientation(Qt::Horizontal);
        label_9 = new QLabel(frame_6);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(10, 30, 41, 17));
        label_11 = new QLabel(frame_6);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(10, 50, 71, 17));
        generalSmoothing = new QSlider(frame_6);
        generalSmoothing->setObjectName(QStringLiteral("generalSmoothing"));
        generalSmoothing->setGeometry(QRect(80, 50, 61, 16));
        generalSmoothing->setMaximum(180);
        generalSmoothing->setValue(90);
        generalSmoothing->setOrientation(Qt::Horizontal);
        generalSmoothingText = new QLineEdit(frame_6);
        generalSmoothingText->setObjectName(QStringLiteral("generalSmoothingText"));
        generalSmoothingText->setGeometry(QRect(150, 50, 41, 21));
        generalFovText = new QLineEdit(frame_6);
        generalFovText->setObjectName(QStringLiteral("generalFovText"));
        generalFovText->setGeometry(QRect(150, 30, 41, 21));
        checkBoxDepthTesting = new QCheckBox(frame_6);
        checkBoxDepthTesting->setObjectName(QStringLiteral("checkBoxDepthTesting"));
        checkBoxDepthTesting->setGeometry(QRect(20, 70, 121, 23));
        frame_7 = new QFrame(centralwidget);
        frame_7->setObjectName(QStringLiteral("frame_7"));
        frame_7->setGeometry(QRect(10, 300, 161, 181));
        frame_7->setFrameShape(QFrame::StyledPanel);
        frame_7->setFrameShadow(QFrame::Raised);
        label_10 = new QLabel(frame_7);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(10, 10, 51, 17));
        buttonNext = new QPushButton(frame_7);
        buttonNext->setObjectName(QStringLiteral("buttonNext"));
        buttonNext->setGeometry(QRect(100, 40, 21, 21));
        buttonPrev = new QPushButton(frame_7);
        buttonPrev->setObjectName(QStringLiteral("buttonPrev"));
        buttonPrev->setGeometry(QRect(40, 40, 21, 21));
        buttonForward = new QPushButton(frame_7);
        buttonForward->setObjectName(QStringLiteral("buttonForward"));
        buttonForward->setGeometry(QRect(130, 40, 21, 21));
        buttonBackward = new QPushButton(frame_7);
        buttonBackward->setObjectName(QStringLiteral("buttonBackward"));
        buttonBackward->setGeometry(QRect(10, 40, 21, 21));
        buttonStop = new QPushButton(frame_7);
        buttonStop->setObjectName(QStringLiteral("buttonStop"));
        buttonStop->setGeometry(QRect(68, 40, 21, 21));
        lineEditFrame = new QLineEdit(frame_7);
        lineEditFrame->setObjectName(QStringLiteral("lineEditFrame"));
        lineEditFrame->setGeometry(QRect(60, 10, 91, 21));
        label_15 = new QLabel(frame_7);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(10, 100, 51, 17));
        lineEditFramesPerStep = new QLineEdit(frame_7);
        lineEditFramesPerStep->setObjectName(QStringLiteral("lineEditFramesPerStep"));
        lineEditFramesPerStep->setGeometry(QRect(90, 100, 61, 21));
        checkBoxRealtime = new QCheckBox(frame_7);
        checkBoxRealtime->setObjectName(QStringLiteral("checkBoxRealtime"));
        checkBoxRealtime->setGeometry(QRect(10, 70, 91, 23));
        lineEditFramesPerSecond = new QLineEdit(frame_7);
        lineEditFramesPerSecond->setObjectName(QStringLiteral("lineEditFramesPerSecond"));
        lineEditFramesPerSecond->setGeometry(QRect(90, 130, 61, 21));
        label_16 = new QLabel(frame_7);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setGeometry(QRect(10, 130, 71, 17));
        frame_8 = new QFrame(centralwidget);
        frame_8->setObjectName(QStringLiteral("frame_8"));
        frame_8->setGeometry(QRect(180, 300, 161, 181));
        frame_8->setFrameShape(QFrame::StyledPanel);
        frame_8->setFrameShadow(QFrame::Raised);
        label_12 = new QLabel(frame_8);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(10, 10, 81, 17));
        label_17 = new QLabel(frame_8);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setGeometry(QRect(10, 30, 51, 17));
        label_18 = new QLabel(frame_8);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setGeometry(QRect(10, 50, 51, 17));
        screenshotWidth = new QLineEdit(frame_8);
        screenshotWidth->setObjectName(QStringLiteral("screenshotWidth"));
        screenshotWidth->setGeometry(QRect(80, 30, 71, 21));
        screenshotHeight = new QLineEdit(frame_8);
        screenshotHeight->setObjectName(QStringLiteral("screenshotHeight"));
        screenshotHeight->setGeometry(QRect(80, 50, 71, 21));
        label_19 = new QLabel(frame_8);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setGeometry(QRect(10, 70, 51, 17));
        label_20 = new QLabel(frame_8);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setGeometry(QRect(10, 90, 51, 17));
        label_21 = new QLabel(frame_8);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setGeometry(QRect(10, 110, 71, 17));
        screenshotFilename = new QLineEdit(frame_8);
        screenshotFilename->setObjectName(QStringLiteral("screenshotFilename"));
        screenshotFilename->setGeometry(QRect(80, 110, 71, 21));
        screenshotView = new QComboBox(frame_8);
        screenshotView->setObjectName(QStringLiteral("screenshotView"));
        screenshotView->setGeometry(QRect(80, 90, 71, 21));
        buttonSaveScreenshot = new QPushButton(frame_8);
        buttonSaveScreenshot->setObjectName(QStringLiteral("buttonSaveScreenshot"));
        buttonSaveScreenshot->setGeometry(QRect(10, 150, 141, 21));
        screenshotCamera = new QComboBox(frame_8);
        screenshotCamera->setObjectName(QStringLiteral("screenshotCamera"));
        screenshotCamera->setGeometry(QRect(80, 70, 71, 21));
        screenshotPrerendering = new QCheckBox(frame_8);
        screenshotPrerendering->setObjectName(QStringLiteral("screenshotPrerendering"));
        screenshotPrerendering->setGeometry(QRect(100, 130, 21, 23));
        label_23 = new QLabel(frame_8);
        label_23->setObjectName(QStringLiteral("label_23"));
        label_23->setGeometry(QRect(10, 130, 71, 17));
        frame_9 = new QFrame(centralwidget);
        frame_9->setObjectName(QStringLiteral("frame_9"));
        frame_9->setGeometry(QRect(10, 490, 211, 91));
        frame_9->setFrameShape(QFrame::StyledPanel);
        frame_9->setFrameShadow(QFrame::Raised);
        label_13 = new QLabel(frame_9);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(10, 10, 101, 17));
        performancePreresolution = new QComboBox(frame_9);
        performancePreresolution->setObjectName(QStringLiteral("performancePreresolution"));
        performancePreresolution->setGeometry(QRect(120, 30, 86, 21));
        label_14 = new QLabel(frame_9);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(10, 30, 101, 17));
        label_22 = new QLabel(frame_9);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setGeometry(QRect(10, 50, 61, 17));
        performanceAnimation = new QComboBox(frame_9);
        performanceAnimation->setObjectName(QStringLiteral("performanceAnimation"));
        performanceAnimation->setGeometry(QRect(120, 50, 86, 21));
        label_24 = new QLabel(frame_9);
        label_24->setObjectName(QStringLiteral("label_24"));
        label_24->setGeometry(QRect(10, 70, 91, 17));
        performanceDepthbuffer = new QComboBox(frame_9);
        performanceDepthbuffer->setObjectName(QStringLiteral("performanceDepthbuffer"));
        performanceDepthbuffer->setGeometry(QRect(120, 70, 86, 21));
        frame_10 = new QFrame(centralwidget);
        frame_10->setObjectName(QStringLiteral("frame_10"));
        frame_10->setGeometry(QRect(10, 590, 331, 71));
        frame_10->setFrameShape(QFrame::StyledPanel);
        frame_10->setFrameShadow(QFrame::Raised);
        buttonUpdateUi = new QPushButton(frame_10);
        buttonUpdateUi->setObjectName(QStringLiteral("buttonUpdateUi"));
        buttonUpdateUi->setGeometry(QRect(10, 10, 81, 21));
        buttonUpdateShader = new QPushButton(frame_10);
        buttonUpdateShader->setObjectName(QStringLiteral("buttonUpdateShader"));
        buttonUpdateShader->setGeometry(QRect(100, 10, 111, 21));
        executeText = new QLineEdit(frame_10);
        executeText->setObjectName(QStringLiteral("executeText"));
        executeText->setGeometry(QRect(10, 40, 231, 21));
        executeButton = new QPushButton(frame_10);
        executeButton->setObjectName(QStringLiteral("executeButton"));
        executeButton->setGeometry(QRect(250, 40, 71, 21));
        buttonRedraw = new QPushButton(frame_10);
        buttonRedraw->setObjectName(QStringLiteral("buttonRedraw"));
        buttonRedraw->setGeometry(QRect(220, 10, 101, 20));
        openGLWidget = new QOpenGLWidget(centralwidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(360, 410, 61, 31));
        checkBoxDebug = new QCheckBox(centralwidget);
        checkBoxDebug->setObjectName(QStringLiteral("checkBoxDebug"));
        checkBoxDebug->setGeometry(QRect(350, 300, 91, 23));
        checkBoxApproximated = new QCheckBox(centralwidget);
        checkBoxApproximated->setObjectName(QStringLiteral("checkBoxApproximated"));
        checkBoxApproximated->setGeometry(QRect(350, 320, 121, 23));
        frame_11 = new QFrame(centralwidget);
        frame_11->setObjectName(QStringLiteral("frame_11"));
        frame_11->setGeometry(QRect(360, 100, 91, 81));
        frame_11->setFrameShape(QFrame::StyledPanel);
        frame_11->setFrameShadow(QFrame::Raised);
        label_26 = new QLabel(frame_11);
        label_26->setObjectName(QStringLiteral("label_26"));
        label_26->setGeometry(QRect(10, 10, 61, 17));
        visibilityShow = new QCheckBox(frame_11);
        visibilityShow->setObjectName(QStringLiteral("visibilityShow"));
        visibilityShow->setGeometry(QRect(10, 30, 61, 23));
        cameraTableView = new QTableView(centralwidget);
        cameraTableView->setObjectName(QStringLiteral("cameraTableView"));
        cameraTableView->setGeometry(QRect(220, 190, 231, 101));
        meshTableView = new QTableView(centralwidget);
        meshTableView->setObjectName(QStringLiteral("meshTableView"));
        meshTableView->setGeometry(QRect(10, 190, 201, 101));
        ControlWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ControlWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 463, 22));
        ControlWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(ControlWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        ControlWindow->setStatusBar(statusbar);
        QWidget::setTabOrder(buttonBackward, buttonStop);
        QWidget::setTabOrder(buttonStop, buttonForward);

        retranslateUi(ControlWindow);
        QObject::connect(buttonForward, SIGNAL(clicked()), ControlWindow, SLOT(playForward()));
        QObject::connect(buttonBackward, SIGNAL(clicked()), ControlWindow, SLOT(playBackward()));
        QObject::connect(buttonStop, SIGNAL(clicked()), ControlWindow, SLOT(playStop()));
        QObject::connect(depthShow, SIGNAL(toggled(bool)), ControlWindow, SLOT(showDepth(bool)));
        QObject::connect(indexShow, SIGNAL(toggled(bool)), ControlWindow, SLOT(showIndex(bool)));
        QObject::connect(positionShow, SIGNAL(toggled(bool)), ControlWindow, SLOT(showPosition(bool)));
        QObject::connect(renderedShow, SIGNAL(toggled(bool)), ControlWindow, SLOT(showRendered(bool)));
        QObject::connect(buttonPrev, SIGNAL(clicked()), ControlWindow, SLOT(prev()));
        QObject::connect(buttonNext, SIGNAL(clicked()), ControlWindow, SLOT(next()));
        QObject::connect(generalFov, SIGNAL(valueChanged(int)), ControlWindow, SLOT(fov(int)));
        QObject::connect(flowShow, SIGNAL(toggled(bool)), ControlWindow, SLOT(showFlow(bool)));
        QObject::connect(performancePreresolution, SIGNAL(currentIndexChanged(QString)), ControlWindow, SLOT(preresolution(QString)));
        QObject::connect(generalSmoothing, SIGNAL(valueChanged(int)), ControlWindow, SLOT(smoothing(int)));
        QObject::connect(flowPast, SIGNAL(valueChanged(int)), ControlWindow, SLOT(past(int)));
        QObject::connect(flowFuture, SIGNAL(valueChanged(int)), ControlWindow, SLOT(future(int)));
        QObject::connect(flowArrowsShow, SIGNAL(toggled(bool)), ControlWindow, SLOT(showArrows(bool)));
        QObject::connect(flowObjects, SIGNAL(toggled(bool)), ControlWindow, SLOT(flowObjects(bool)));
        QObject::connect(flowRotation, SIGNAL(toggled(bool)), ControlWindow, SLOT(flowRotation(bool)));
        QObject::connect(flowTranslation, SIGNAL(toggled(bool)), ControlWindow, SLOT(flowTranslation(bool)));
        QObject::connect(lineEditFrame, SIGNAL(textEdited(QString)), ControlWindow, SLOT(frame(QString)));
        QObject::connect(checkBoxRealtime, SIGNAL(toggled(bool)), ControlWindow, SLOT(realtime(bool)));
        QObject::connect(lineEditFramesPerSecond, SIGNAL(textEdited(QString)), ControlWindow, SLOT(framesPerSecond(QString)));
        QObject::connect(lineEditFramesPerStep, SIGNAL(textEdited(QString)), ControlWindow, SLOT(framesPerStep(QString)));
        QObject::connect(buttonSaveScreenshot, SIGNAL(clicked()), ControlWindow, SLOT(saveScreenshot()));
        QObject::connect(buttonUpdateUi, SIGNAL(clicked()), ControlWindow, SLOT(updateUi()));
        QObject::connect(buttonUpdateShader, SIGNAL(clicked()), ControlWindow, SLOT(updateShader()));
        QObject::connect(positionShowCurser, SIGNAL(toggled(bool)), ControlWindow, SLOT(positionShowCurser(bool)));
        QObject::connect(executeButton, SIGNAL(clicked()), ControlWindow, SLOT(executeCommand()));
        QObject::connect(executeText, SIGNAL(returnPressed()), ControlWindow, SLOT(executeCommand()));
        QObject::connect(ControlWindow, SIGNAL(updateUiSignal(int)), ControlWindow, SLOT(updateUi(int)));
        QObject::connect(buttonRedraw, SIGNAL(clicked()), ControlWindow, SLOT(redraw()));
        QObject::connect(performanceAnimation, SIGNAL(currentTextChanged(QString)), ControlWindow, SLOT(animating(QString)));
        QObject::connect(generalFovText, SIGNAL(textChanged(QString)), ControlWindow, SLOT(fov(QString)));
        QObject::connect(checkBoxDebug, SIGNAL(toggled(bool)), ControlWindow, SLOT(debug(bool)));
        QObject::connect(performanceDepthbuffer, SIGNAL(currentIndexChanged(QString)), ControlWindow, SLOT(depthbuffer(QString)));
        QObject::connect(checkBoxApproximated, SIGNAL(toggled(bool)), ControlWindow, SLOT(approximated(bool)));
        QObject::connect(depthScaleText, SIGNAL(textEdited(QString)), ControlWindow, SLOT(depthMax(QString)));
        QObject::connect(renderedVisibility, SIGNAL(toggled(bool)), ControlWindow, SLOT(renderedVisibility(bool)));
        QObject::connect(checkBoxDepthTesting, SIGNAL(toggled(bool)), ControlWindow, SLOT(depthTesting(bool)));

        QMetaObject::connectSlotsByName(ControlWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ControlWindow)
    {
        ControlWindow->setWindowTitle(QApplication::translate("ControlWindow", "MainWindow", Q_NULLPTR));
        actionTest->setText(QApplication::translate("ControlWindow", "Test", Q_NULLPTR));
        flowShow->setText(QApplication::translate("ControlWindow", "Show", Q_NULLPTR));
        flowArrowsShow->setText(QApplication::translate("ControlWindow", "Arrows", Q_NULLPTR));
        label->setText(QApplication::translate("ControlWindow", "Flow", Q_NULLPTR));
        label_2->setText(QApplication::translate("ControlWindow", "Past", Q_NULLPTR));
        label_3->setText(QApplication::translate("ControlWindow", "Future", Q_NULLPTR));
        flowTranslation->setText(QApplication::translate("ControlWindow", "Translation", Q_NULLPTR));
        flowRotation->setText(QApplication::translate("ControlWindow", "Rotation", Q_NULLPTR));
        flowObjects->setText(QApplication::translate("ControlWindow", "Objects", Q_NULLPTR));
        label_4->setText(QApplication::translate("ControlWindow", "Depth", Q_NULLPTR));
        depthShow->setText(QApplication::translate("ControlWindow", "Show", Q_NULLPTR));
        depthScaleText->setText(QApplication::translate("ControlWindow", "1", Q_NULLPTR));
        label_25->setText(QApplication::translate("ControlWindow", "Dist", Q_NULLPTR));
        label_5->setText(QApplication::translate("ControlWindow", "Rendered", Q_NULLPTR));
        renderedShow->setText(QApplication::translate("ControlWindow", "Show", Q_NULLPTR));
        renderedVisibility->setText(QApplication::translate("ControlWindow", "Visibility", Q_NULLPTR));
        label_6->setText(QApplication::translate("ControlWindow", "Index", Q_NULLPTR));
        indexShow->setText(QApplication::translate("ControlWindow", "Show", Q_NULLPTR));
        label_7->setText(QApplication::translate("ControlWindow", "Position", Q_NULLPTR));
        positionShow->setText(QApplication::translate("ControlWindow", "Show", Q_NULLPTR));
        positionShowCurser->setText(QApplication::translate("ControlWindow", "Curser", Q_NULLPTR));
        label_8->setText(QApplication::translate("ControlWindow", "General", Q_NULLPTR));
        label_9->setText(QApplication::translate("ControlWindow", "Fov", Q_NULLPTR));
        label_11->setText(QApplication::translate("ControlWindow", "Smooting", Q_NULLPTR));
        checkBoxDepthTesting->setText(QApplication::translate("ControlWindow", "DepthTesting", Q_NULLPTR));
        label_10->setText(QApplication::translate("ControlWindow", "Frame", Q_NULLPTR));
        buttonNext->setText(QApplication::translate("ControlWindow", ">|", Q_NULLPTR));
        buttonPrev->setText(QApplication::translate("ControlWindow", "|<", Q_NULLPTR));
        buttonForward->setText(QApplication::translate("ControlWindow", ">", Q_NULLPTR));
        buttonBackward->setText(QApplication::translate("ControlWindow", "<", Q_NULLPTR));
        buttonStop->setText(QApplication::translate("ControlWindow", "H", Q_NULLPTR));
        label_15->setText(QApplication::translate("ControlWindow", "F/Step", Q_NULLPTR));
        lineEditFramesPerStep->setText(QApplication::translate("ControlWindow", "1", Q_NULLPTR));
        checkBoxRealtime->setText(QApplication::translate("ControlWindow", "Realtime", Q_NULLPTR));
        lineEditFramesPerSecond->setText(QApplication::translate("ControlWindow", "1", Q_NULLPTR));
        label_16->setText(QApplication::translate("ControlWindow", "F/Second", Q_NULLPTR));
        label_12->setText(QApplication::translate("ControlWindow", "Screenshot", Q_NULLPTR));
        label_17->setText(QApplication::translate("ControlWindow", "Width", Q_NULLPTR));
        label_18->setText(QApplication::translate("ControlWindow", "Height", Q_NULLPTR));
        screenshotWidth->setText(QApplication::translate("ControlWindow", "1024", Q_NULLPTR));
        screenshotHeight->setText(QApplication::translate("ControlWindow", "1024", Q_NULLPTR));
        label_19->setText(QApplication::translate("ControlWindow", "Camera", Q_NULLPTR));
        label_20->setText(QApplication::translate("ControlWindow", "View", Q_NULLPTR));
        label_21->setText(QApplication::translate("ControlWindow", "Filename", Q_NULLPTR));
        screenshotView->clear();
        screenshotView->insertItems(0, QStringList()
         << QApplication::translate("ControlWindow", "Rendered", Q_NULLPTR)
         << QApplication::translate("ControlWindow", "Position", Q_NULLPTR)
         << QApplication::translate("ControlWindow", "Flow", Q_NULLPTR)
         << QApplication::translate("ControlWindow", "Index", Q_NULLPTR)
         << QApplication::translate("ControlWindow", "Depth", Q_NULLPTR)
        );
        buttonSaveScreenshot->setText(QApplication::translate("ControlWindow", "Save", Q_NULLPTR));
        screenshotPrerendering->setText(QString());
        label_23->setText(QApplication::translate("ControlWindow", "Pre", Q_NULLPTR));
        label_13->setText(QApplication::translate("ControlWindow", "Performance", Q_NULLPTR));
        performancePreresolution->clear();
        performancePreresolution->insertItems(0, QStringList()
         << QApplication::translate("ControlWindow", "256", Q_NULLPTR)
         << QApplication::translate("ControlWindow", "512", Q_NULLPTR)
         << QApplication::translate("ControlWindow", "1024", Q_NULLPTR)
         << QApplication::translate("ControlWindow", "2048", Q_NULLPTR)
         << QApplication::translate("ControlWindow", "4096", Q_NULLPTR)
        );
        label_14->setText(QApplication::translate("ControlWindow", "Preresolution", Q_NULLPTR));
        label_22->setText(QApplication::translate("ControlWindow", "Redraw", Q_NULLPTR));
        performanceAnimation->clear();
        performanceAnimation->insertItems(0, QStringList()
         << QApplication::translate("ControlWindow", "Always", Q_NULLPTR)
         << QApplication::translate("ControlWindow", "Automatic", Q_NULLPTR)
         << QApplication::translate("ControlWindow", "Manual", Q_NULLPTR)
        );
        label_24->setText(QApplication::translate("ControlWindow", "Depthbuffer", Q_NULLPTR));
        performanceDepthbuffer->clear();
        performanceDepthbuffer->insertItems(0, QStringList()
         << QApplication::translate("ControlWindow", "16 bit", Q_NULLPTR)
         << QApplication::translate("ControlWindow", "24 bit", Q_NULLPTR)
         << QApplication::translate("ControlWindow", "32 bit", Q_NULLPTR)
        );
        buttonUpdateUi->setText(QApplication::translate("ControlWindow", "UpdateUi", Q_NULLPTR));
        buttonUpdateShader->setText(QApplication::translate("ControlWindow", "UpdateShader", Q_NULLPTR));
        executeButton->setText(QApplication::translate("ControlWindow", "Execute", Q_NULLPTR));
        buttonRedraw->setText(QApplication::translate("ControlWindow", "Redraw", Q_NULLPTR));
        checkBoxDebug->setText(QApplication::translate("ControlWindow", "Debug", Q_NULLPTR));
        checkBoxApproximated->setText(QApplication::translate("ControlWindow", "Approximated", Q_NULLPTR));
        label_26->setText(QApplication::translate("ControlWindow", "Visibility", Q_NULLPTR));
        visibilityShow->setText(QApplication::translate("ControlWindow", "Show", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ControlWindow: public Ui_ControlWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // CONTROL_UI_H
