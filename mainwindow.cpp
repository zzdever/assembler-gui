/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "colorswatch.h"
#include "toolbar.h"

#include <QAction>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QFile>
#include <QDataStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QSignalMapper>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <qdebug.h>
#include <QSplitter>

//static const char * const message =
//    "<p><b>Qt Main Window Example</b></p>"

//    "<p>This is a demonstration of the QMainWindow, QToolBar and "
//    "QDockWidget classes.</p>"

//    "<p>The tool bar and dock widgets can be dragged around and rearranged "
//    "using the mouse or via the menu.</p>"

//    "<p>Each dock widget contains a colored frame and a context "
//    "(right-click) menu.</p>"

//#ifdef Q_OS_MAC
//    "<p>On Mac OS X, the \"Black\" dock widget has been created as a "
//    "<em>Drawer</em>, which is a special kind of QDockWidget.</p>"
//#endif
//    ;

Q_DECLARE_METATYPE(QDockWidget::DockWidgetFeatures)

//MainWindow::MainWindow(const QMap<QString, QSize> &customSizeHints,
//                        QWidget *parent, Qt::WindowFlags flags)
//    : QMainWindow(parent, flags)
MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    setObjectName("Assembler GUI");
    setWindowTitle("Assembler by zzdever");

    left = new QTextEdit(this);
    left->setReadOnly(false);
    left->setMinimumSize(200, 205);
    //setCentralWidget(left);

    center = new QTextEdit(this);
    center->setReadOnly(false);
    center->setMinimumSize(200, 205);
    //setCentralWidget(center);

    QSplitter *splitter= new QSplitter;
    splitter->setHandleWidth(0);
    splitter->addWidget(left);
    splitter->addWidget(center);
    this->setCentralWidget(splitter);

    setupToolBar();

    setupMenuBar();

    setupDockWidgets();

    statusBar()->showMessage(tr("Status Bar"));
}

void MainWindow::actionTriggered(QAction *action)
{
    qDebug("action '%s' triggered", action->text().toLocal8Bit().data());
}

void MainWindow::setupToolBar()
{
    ToolBar *tb = new ToolBar(QString::fromLatin1("Tool bar %1").arg(1), this);
    toolBars.append(tb);
    addToolBar(tb);

    return;
}

void MainWindow::setupMenuBar()
{
    //File
    QMenu *menu = menuBar()->addMenu(tr("File"));

    QAction *action = menu->addAction("New");
    connect(action, SIGNAL(triggered()), this, SLOT(newFile()));

    action = menu->addAction(tr("Open"));
    connect(action, SIGNAL(triggered()), this, SLOT(openFile()));

    action = menu->addAction(tr("Save"));
    connect(action, SIGNAL(triggered()), this, SLOT(saveLayout()));

    action = menu->addAction(tr("Save as"));
    connect(action, SIGNAL(triggered()), this, SLOT(saveLayout()));
    menu->addSeparator();
    menu->addAction(tr("Quit"), this, SLOT(close()));

    //Build
    mainWindowMenu = menuBar()->addMenu(tr("Build"));

    action = mainWindowMenu->addAction(tr("Assemble"));
    connect(action, SIGNAL(toggled(bool)), this, SLOT(Assemble()));

    action = mainWindowMenu->addAction(tr("Disassemble"));
    action->setCheckable(true);
    action->setChecked(dockOptions() & AllowNestedDocks);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(Disassemble()));

    action = mainWindowMenu->addAction(tr("Gen coe"));
    connect(action, SIGNAL(toggled(bool)), this, SLOT(GenCoe()));

    //About
    QMenu *aboutMenu = menuBar()->addMenu(tr("About"));

    action = aboutMenu->addAction("dev");
    connect(action, SIGNAL(triggered()), this, SLOT(newFile()));


    //QMenu *toolBarMenu = menuBar()->addMenu(tr("Tool bars"));
    //for (int i = 0; i < toolBars.count(); ++i)
    //  toolBarMenu->addMenu(toolBars.at(i)->menu);

    dockWidgetMenu = menuBar()->addMenu(tr("&Dock Widgets"));
}

void MainWindow::setDockOptions()
{
    DockOptions opts;
    QList<QAction*> actions = mainWindowMenu->actions();

    if (actions.at(0)->isChecked())
        opts |= AnimatedDocks;
    if (actions.at(1)->isChecked())
        opts |= AllowNestedDocks;
    if (actions.at(2)->isChecked())
        opts |= AllowTabbedDocks;
    if (actions.at(3)->isChecked())
        opts |= ForceTabbedDocks;
    if (actions.at(4)->isChecked())
        opts |= VerticalTabs;

    QMainWindow::setDockOptions(opts);
}

void MainWindow::newFile()
{
    ;
}

void MainWindow::saveLayout()
{
    QString fileName
        = QFileDialog::getSaveFileName(this, tr("Save layout"));
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QString msg = tr("Failed to open %1\n%2")
                        .arg(fileName)
                        .arg(file.errorString());
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }

    QByteArray geo_data = saveGeometry();
    QByteArray layout_data = saveState();

    bool ok = file.putChar((uchar)geo_data.size());
    if (ok)
        ok = file.write(geo_data) == geo_data.size();
    if (ok)
        ok = file.write(layout_data) == layout_data.size();

    if (!ok) {
        QString msg = tr("Error writing to %1\n%2")
                        .arg(fileName)
                        .arg(file.errorString());
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }
}

void MainWindow::openFile()
{
    filename = QFileDialog::getOpenFileName(this, tr("Open ASM File"));
    if (filename.isEmpty())
        return;

    showAsm();

//    uchar geo_size;
//    QByteArray geo_data;
//    QByteArray layout_data;


//    bool ok = file.getChar((char*)&geo_size);
//    if (ok) {
//        geo_data = file.read(geo_size);
//        ok = geo_data.size() == geo_size;
//    }
//    if (ok) {
//        layout_data = file.readAll();
//        ok = layout_data.size() > 0;
//    }

//    if (ok)
//        ok = restoreGeometry(geo_data);
//    if (ok)
//        ok = restoreState(layout_data);

//    if (!ok) {
//        QString msg = tr("Error reading %1")
//                        .arg(fileName);
//        QMessageBox::warning(this, tr("Error"), msg);
//        return;
//    }

}

void MainWindow::showAsm(int error)
{
    QFile fileAsm(filename);
    if (!fileAsm.open(QFile::ReadOnly)) {
        QString msg = tr("Failed to open %1\n%2")
                        .arg(filename)
                        .arg(fileAsm.errorString());
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }
    QTextStream streamAsm(&fileAsm);

    QString line;
    int lineNumber=0;
    while(!streamAsm.atEnd())
    {
        line=streamAsm.readLine();

        QColor textColor(160,160,160);
        left->setTextColor(textColor);
        //left->append(QString("lineNumber"));
        left->insertPlainText("sfsfd ");
        textColor.setRed(216);
        textColor.setGreen(127);
        textColor.setBlue(98);
        left->setTextColor(textColor);
        left->insertPlainText(line+"\n");

        lineNumber++;
    }

    fileAsm.close();
}

void MainWindow::Assemble()
{
    ;
}

void MainWindow::Disassemble()
{
    ;
}

void MainWindow::GenCoe()
{
    ;
}

QAction *addAction(QMenu *menu, const QString &text, QActionGroup *group, QSignalMapper *mapper,
                    int id)
{
    bool first = group->actions().isEmpty();
    QAction *result = menu->addAction(text);
    result->setCheckable(true);
    result->setChecked(first);
    group->addAction(result);
    QObject::connect(result, SIGNAL(triggered()), mapper, SLOT(map()));
    mapper->setMapping(result, id);
    return result;
}

//void MainWindow::setupDockWidgets(const QMap<QString, QSize> &customSizeHints)
void MainWindow::setupDockWidgets(void)
{
    qRegisterMetaType<QDockWidget::DockWidgetFeatures>();

    mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(setCorner(int)));

//    QMenu *corner_menu = dockWidgetMenu->addMenu(tr("Top left corner"));
//    QActionGroup *group = new QActionGroup(this);
//    group->setExclusive(true);
//    ::addAction(corner_menu, tr("Top dock area"), group, mapper, 0);
//    ::addAction(corner_menu, tr("Left dock area"), group, mapper, 1);

//    corner_menu = dockWidgetMenu->addMenu(tr("Top right corner"));
//    group = new QActionGroup(this);
//    group->setExclusive(true);
//    ::addAction(corner_menu, tr("Top dock area"), group, mapper, 2);
//    ::addAction(corner_menu, tr("Right dock area"), group, mapper, 3);

//    corner_menu = dockWidgetMenu->addMenu(tr("Bottom left corner"));
//    group = new QActionGroup(this);
//    group->setExclusive(true);
//    ::addAction(corner_menu, tr("Bottom dock area"), group, mapper, 4);
//    ::addAction(corner_menu, tr("Left dock area"), group, mapper, 5);

//    corner_menu = dockWidgetMenu->addMenu(tr("Bottom right corner"));
//    group = new QActionGroup(this);
//    group->setExclusive(true);
//    ::addAction(corner_menu, tr("Bottom dock area"), group, mapper, 6);
//    ::addAction(corner_menu, tr("Right dock area"), group, mapper, 7);

//    dockWidgetMenu->addSeparator();

    static const struct Set {
        const char * name;
        uint flags;
        Qt::DockWidgetArea area;
    } sets [] = {
        { "Black", 0, Qt::LeftDockWidgetArea },
        { "White", 0, Qt::RightDockWidgetArea },
        //{ "Red", 0, Qt::TopDockWidgetArea },
        //{ "Green", 0, Qt::TopDockWidgetArea },
        //{ "Blue", 0, Qt::BottomDockWidgetArea },
        { "Yellow", 0, Qt::BottomDockWidgetArea }
    };
    const int setCount = sizeof(sets) / sizeof(Set);

    setCorner(0);

    for (int i = 0; i < setCount; ++i)
    {
        ColorSwatch *swatch = new ColorSwatch(tr(sets[i].name), this, Qt::WindowFlags(sets[i].flags));
//        if (i%2)
//            swatch->setWindowIcon(QIcon(QPixmap(":/res/qt.png")));
//        if (qstrcmp(sets[i].name, "Blue") == 0) {
//            BlueTitleBar *titlebar = new BlueTitleBar(swatch);
//            swatch->setTitleBarWidget(titlebar);
//            connect(swatch, SIGNAL(topLevelChanged(bool)), titlebar, SLOT(updateMask()));
//            connect(swatch, SIGNAL(featuresChanged(QDockWidget::DockWidgetFeatures)),
//                    titlebar, SLOT(updateMask()), Qt::QueuedConnection);
//        }

//        QString name = QString::fromLatin1(sets[i].name);
//        if (customSizeHints.contains(name))
//            swatch->setCustomSizeHint(customSizeHints.value(name));

        QSize size(250,80);
        swatch->setCustomSizeHint(size);
        addDockWidget(sets[i].area, swatch);

//        dockWidgetMenu->addMenu(swatch->menu);
    }

//    createDockWidgetAction = new QAction(tr("Add dock widget..."), this);
//    connect(createDockWidgetAction, SIGNAL(triggered()), this, SLOT(createDockWidget()));
//    destroyDockWidgetMenu = new QMenu(tr("Destroy dock widget"), this);
//    destroyDockWidgetMenu->setEnabled(false);
//    connect(destroyDockWidgetMenu, SIGNAL(triggered(QAction*)), this, SLOT(destroyDockWidget(QAction*)));

//    dockWidgetMenu->addSeparator();
//    dockWidgetMenu->addAction(createDockWidgetAction);
//    dockWidgetMenu->addMenu(destroyDockWidgetMenu);

}

void MainWindow::setCorner(int id)
{
    QMainWindow::setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    QMainWindow::setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

//    switch (id) {
//        case 0:
//            QMainWindow::setCorner(Qt::TopLeftCorner, Qt::TopDockWidgetArea);
//            break;
//        case 1:
//            QMainWindow::setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
//            break;
//        case 2:
//            QMainWindow::setCorner(Qt::TopRightCorner, Qt::TopDockWidgetArea);
//            break;
//        case 3:
//            QMainWindow::setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
//            break;
//        case 4:
//            QMainWindow::setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
//            break;
//        case 5:
//            QMainWindow::setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
//            break;
//        case 6:
//            QMainWindow::setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);
//            break;
//        case 7:
//            QMainWindow::setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
//            break;
//    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
}

//void MainWindow::switchLayoutDirection()
//{
//    if (layoutDirection() == Qt::LeftToRight)
//        qApp->setLayoutDirection(Qt::RightToLeft);
//    else
//        qApp->setLayoutDirection(Qt::LeftToRight);
//}

class CreateDockWidgetDialog : public QDialog
{
public:
    CreateDockWidgetDialog(QWidget *parent = 0);

    QString objectName() const;
    Qt::DockWidgetArea location() const;

private:
    QLineEdit *m_objectName;
    QComboBox *m_location;
};

CreateDockWidgetDialog::CreateDockWidgetDialog(QWidget *parent)
    : QDialog(parent)
{
    QGridLayout *layout = new QGridLayout(this);

    layout->addWidget(new QLabel(tr("Object name:")), 0, 0);
    m_objectName = new QLineEdit;
    layout->addWidget(m_objectName, 0, 1);

    layout->addWidget(new QLabel(tr("Location:")), 1, 0);
    m_location = new QComboBox;
    m_location->setEditable(false);
    m_location->addItem(tr("Top"));
    m_location->addItem(tr("Left"));
    m_location->addItem(tr("Right"));
    m_location->addItem(tr("Bottom"));
    m_location->addItem(tr("Restore"));
    layout->addWidget(m_location, 1, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    layout->addLayout(buttonLayout, 2, 0, 1, 2);
    buttonLayout->addStretch();

    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    buttonLayout->addWidget(cancelButton);
    QPushButton *okButton = new QPushButton(tr("Ok"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    buttonLayout->addWidget(okButton);

    okButton->setDefault(true);
}

QString CreateDockWidgetDialog::objectName() const
{
    return m_objectName->text();
}

Qt::DockWidgetArea CreateDockWidgetDialog::location() const
{
    switch (m_location->currentIndex()) {
        case 0: return Qt::TopDockWidgetArea;
        case 1: return Qt::LeftDockWidgetArea;
        case 2: return Qt::RightDockWidgetArea;
        case 3: return Qt::BottomDockWidgetArea;
        default:
            break;
    }
    return Qt::NoDockWidgetArea;
}

//void MainWindow::createDockWidget()
//{
//    CreateDockWidgetDialog dialog(this);
//    int ret = dialog.exec();
//    if (ret == QDialog::Rejected)
//        return;

//    QDockWidget *dw = new QDockWidget;
//    dw->setObjectName(dialog.objectName());
//    dw->setWindowTitle(dialog.objectName());
//    dw->setWidget(new QTextEdit);

//    Qt::DockWidgetArea area = dialog.location();
//    switch (area) {
//        case Qt::LeftDockWidgetArea:
//        case Qt::RightDockWidgetArea:
//        case Qt::TopDockWidgetArea:
//        case Qt::BottomDockWidgetArea:
//            addDockWidget(area, dw);
//            break;
//        default:
//            if (!restoreDockWidget(dw)) {
//                QMessageBox::warning(this, QString(), tr("Failed to restore dock widget"));
//                delete dw;
//                return;
//            }
//            break;
//    }

//    extraDockWidgets.append(dw);
//    destroyDockWidgetMenu->setEnabled(true);
//    destroyDockWidgetMenu->addAction(new QAction(dialog.objectName(), this));
//}

//void MainWindow::destroyDockWidget(QAction *action)
//{
//    int index = destroyDockWidgetMenu->actions().indexOf(action);
//    delete extraDockWidgets.takeAt(index);
//    destroyDockWidgetMenu->removeAction(action);
//    action->deleteLater();

//    if (destroyDockWidgetMenu->isEmpty())
//        destroyDockWidgetMenu->setEnabled(false);
//}
