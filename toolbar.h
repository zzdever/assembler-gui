#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>
#include <QInputDialog>
#include <QDebug>
#include "modules/modules.h"
#include "mainwindow.h"

#define NEXT -1
#define RUN -2

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QActionGroup)
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QSpinBox)
QT_FORWARD_DECLARE_CLASS(QLabel)

class ToolBar : public QToolBar
{
    Q_OBJECT

    QSpinBox *spinbox;
    QAction *spinboxAction;

    QAction *orderAction;
    QAction *randomizeAction;
    QAction *addSpinBoxAction;
    QAction *removeSpinBoxAction;

    QAction *movableAction;

    QActionGroup *allowedAreasActions;
    QAction *allowLeftAction;
    QAction *allowRightAction;
    QAction *allowTopAction;
    QAction *allowBottomAction;

    QActionGroup *areaActions;
    QAction *leftAction;
    QAction *rightAction;
    QAction *topAction;
    QAction *bottomAction;

    QAction *toolBarBreakAction;

public:
    ToolBar(const QString &title, QWidget *parent);

    QMenu *menu;
    int runMode;
    Emulator emulator;



protected:
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    void allow(Qt::ToolBarArea area, bool allow);
    void place(Qt::ToolBarArea area, bool place);
    QLabel *tip;
    int runLine;

signals:
    void Step(int line);
    void Stop(void);

private slots:
    void order();
    void randomize();
    void addSpinBox();
    void removeSpinBox();

    void changeMovable(bool movable);

    void allowLeft(bool a);
    void allowRight(bool a);
    void allowTop(bool a);
    void allowBottom(bool a);

    void placeLeft(bool p);
    void placeRight(bool p);
    void placeTop(bool p);
    void placeBottom(bool p);

    void updateMenu();
    void insertToolBarBreak();

    void msg();
    void Run()
    {
        emit Step(RUN);
    }
    void Runto()
    {
        runLine=QInputDialog::getInt(this,tr("Input line number"),tr("Please input line number"));
        if(runLine>0) {emit Step(runLine);}
    }
    void SetStop()
    {
        emit Stop();
    }
    void Next()
    {
        emit Step(NEXT);
    }


};

#endif
