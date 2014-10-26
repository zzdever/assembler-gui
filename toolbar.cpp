#include "toolbar.h"

#include <QMainWindow>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QSpinBox>
#include <QLabel>
#include <QToolTip>
#include <QDebug>
#include <QMessageBox>
#include "mainwindow.h"
#include "modules/modules.h"

#include <stdlib.h>

static QPixmap genIcon(const QSize &iconSize, const QString & imageName, const QColor &color)
{
    int w = iconSize.width();
    int h = iconSize.height();

    QImage image(w, h, QImage::Format_ARGB32_Premultiplied);
    //image.fill(0);
    image.load("sources/"+imageName+".png");
    QPainter p(&image);

    //extern void render_qt_text(QPainter *, int, int, const QColor &);
    //render_qt_text(&p, w, h, color);

    return QPixmap::fromImage(image, Qt::DiffuseDither | Qt::DiffuseAlphaDither);
}


static QPixmap genIcon(const QSize &iconSize, int number, const QColor &color)
{ return genIcon(iconSize, QString::number(number), color); }

ToolBar::ToolBar(const QString &title, QWidget *parent)
    : QToolBar(parent), spinbox(0), spinboxAction(0)
{
    tip = 0;
    setWindowTitle(title);
    setObjectName(title);

    setIconSize(QSize(24, 24));

    //QColor bg(palette().background().color());

//    menu = new QMenu("Run", this);
//    menu->setIcon(genIcon(iconSize(), "run", Qt::black));

//    action=menu->addAction("Run to");
//    addAction(menu->menuAction());            



    QAction * action=addAction(genIcon(iconSize(), "run", Qt::blue), "Run");
    connect(action, SIGNAL(triggered()), this, SLOT(Run()));

    action=addAction(genIcon(iconSize(), "runto", Qt::blue), "Runto");
    connect(action, SIGNAL(triggered()), this, SLOT(Runto()));

    action=addAction(genIcon(iconSize(), "stop", Qt::blue), "Stop");
    connect(action, SIGNAL(triggered()), this, SLOT(SetStop()));

    action=addAction(genIcon(iconSize(), "next", Qt::yellow), "Next");
    connect(action, SIGNAL(triggered()), this, SLOT(Next()));

}



void ToolBar::msg()
{
    QMessageBox::information(this,"O","yeah");
}

void ToolBar::updateMenu()
{
    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    Q_ASSERT(mainWindow != 0);

    const Qt::ToolBarArea area = mainWindow->toolBarArea(this);
    const Qt::ToolBarAreas areas = allowedAreas();

    movableAction->setChecked(isMovable());

    allowLeftAction->setChecked(isAreaAllowed(Qt::LeftToolBarArea));
    allowRightAction->setChecked(isAreaAllowed(Qt::RightToolBarArea));
    allowTopAction->setChecked(isAreaAllowed(Qt::TopToolBarArea));
    allowBottomAction->setChecked(isAreaAllowed(Qt::BottomToolBarArea));

    if (allowedAreasActions->isEnabled()) {
        allowLeftAction->setEnabled(area != Qt::LeftToolBarArea);
        allowRightAction->setEnabled(area != Qt::RightToolBarArea);
        allowTopAction->setEnabled(area != Qt::TopToolBarArea);
        allowBottomAction->setEnabled(area != Qt::BottomToolBarArea);
    }

    leftAction->setChecked(area == Qt::LeftToolBarArea);
    rightAction->setChecked(area == Qt::RightToolBarArea);
    topAction->setChecked(area == Qt::TopToolBarArea);
    bottomAction->setChecked(area == Qt::BottomToolBarArea);

    if (areaActions->isEnabled()) {
        leftAction->setEnabled(areas & Qt::LeftToolBarArea);
        rightAction->setEnabled(areas & Qt::RightToolBarArea);
        topAction->setEnabled(areas & Qt::TopToolBarArea);
        bottomAction->setEnabled(areas & Qt::BottomToolBarArea);
    }
}

void ToolBar::order()
{
    QList<QAction *> ordered, actions1 = actions(),
                              actions2 = findChildren<QAction *>();
    while (!actions2.isEmpty()) {
        QAction *action = actions2.takeFirst();
        if (!actions1.contains(action))
            continue;
        actions1.removeAll(action);
        ordered.append(action);
    }

    clear();
    addActions(ordered);

    orderAction->setEnabled(false);
}

void ToolBar::randomize()
{
    QList<QAction *> randomized, actions = this->actions();
    while (!actions.isEmpty()) {
        QAction *action = actions.takeAt(rand() % actions.size());
        randomized.append(action);
    }
    clear();
    addActions(randomized);

    orderAction->setEnabled(true);
}

void ToolBar::addSpinBox()
{
    if (!spinbox) {
        spinbox = new QSpinBox(this);
    }
    if (!spinboxAction)
        spinboxAction = addWidget(spinbox);
    else
        addAction(spinboxAction);

    addSpinBoxAction->setEnabled(false);
    removeSpinBoxAction->setEnabled(true);
}

void ToolBar::removeSpinBox()
{
    if (spinboxAction)
        removeAction(spinboxAction);

    addSpinBoxAction->setEnabled(true);
    removeSpinBoxAction->setEnabled(false);
}

void ToolBar::allow(Qt::ToolBarArea area, bool a)
{
    Qt::ToolBarAreas areas = allowedAreas();
    areas = a ? areas | area : areas & ~area;
    setAllowedAreas(areas);

    if (areaActions->isEnabled()) {
        leftAction->setEnabled(areas & Qt::LeftToolBarArea);
        rightAction->setEnabled(areas & Qt::RightToolBarArea);
        topAction->setEnabled(areas & Qt::TopToolBarArea);
        bottomAction->setEnabled(areas & Qt::BottomToolBarArea);
    }
}

void ToolBar::place(Qt::ToolBarArea area, bool p)
{
    if (!p)
        return;

    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    Q_ASSERT(mainWindow != 0);

    mainWindow->addToolBar(area, this);

    if (allowedAreasActions->isEnabled()) {
        allowLeftAction->setEnabled(area != Qt::LeftToolBarArea);
        allowRightAction->setEnabled(area != Qt::RightToolBarArea);
        allowTopAction->setEnabled(area != Qt::TopToolBarArea);
        allowBottomAction->setEnabled(area != Qt::BottomToolBarArea);
    }
}

void ToolBar::changeMovable(bool movable)
{ setMovable(movable); }

void ToolBar::allowLeft(bool a)
{ allow(Qt::LeftToolBarArea, a); }

void ToolBar::allowRight(bool a)
{ allow(Qt::RightToolBarArea, a); }

void ToolBar::allowTop(bool a)
{ allow(Qt::TopToolBarArea, a); }

void ToolBar::allowBottom(bool a)
{ allow(Qt::BottomToolBarArea, a); }

void ToolBar::placeLeft(bool p)
{ place(Qt::LeftToolBarArea, p); }

void ToolBar::placeRight(bool p)
{ place(Qt::RightToolBarArea, p); }

void ToolBar::placeTop(bool p)
{ place(Qt::TopToolBarArea, p); }

void ToolBar::placeBottom(bool p)
{ place(Qt::BottomToolBarArea, p); }

void ToolBar::insertToolBarBreak()
{
    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    Q_ASSERT(mainWindow != 0);

    mainWindow->insertToolBarBreak(this);
}

void ToolBar::enterEvent(QEvent*)
{
/*
    These labels on top of toolbars look darn ugly

    if (tip == 0) {
        tip = new QLabel(windowTitle(), this);
        QPalette pal = tip->palette();
        QColor c = Qt::black;
        c.setAlpha(100);
        pal.setColor(QPalette::Window, c);
        pal.setColor(QPalette::Foreground, Qt::white);
        tip->setPalette(pal);
        tip->setAutoFillBackground(true);
        tip->setMargin(3);
        tip->setText(windowTitle());
    }
    QPoint c = rect().center();
    QSize hint = tip->sizeHint();
    tip->setGeometry(c.x() - hint.width()/2, c.y() - hint.height()/2,
                        hint.width(), hint.height());

    tip->show();
*/
}

void ToolBar::leaveEvent(QEvent*)
{
    if (tip != 0)
        tip->hide();
}

