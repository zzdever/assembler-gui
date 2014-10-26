#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QFile>
#include <QTableView>
#include <QTableWidget>
#include "toolbar.h"
#include "modules/modules.h"

#define NEXT -1
#define RUN -2

class ToolBar;
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QSignalMapper)


class MainWindow : public QMainWindow
{
    Q_OBJECT

    QTextEdit *left, *center;
    QTableView *memoryTable, *registerTable;
    QDockWidget *memoryDock, *registerDock;

    QList<ToolBar*> toolBars;
    QMenu *dockWidgetMenu;
    QMenu *mainWindowMenu;
    QSignalMapper *mapper;
    QList<QDockWidget*> extraDockWidgets;
    QAction *createDockWidgetAction;
    QMenu *destroyDockWidgetMenu;

public:
//    MainWindow(const QMap<QString, QSize> &customSizeHints,
//                QWidget *parent = 0, Qt::WindowFlags flags = 0);
    bool fileAsmSaved;
    bool fileAsmEdited;
    bool fileObjSaved;
    bool fileObjEdited;
    Assembler assembler;
    Emulator emulator;

    MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);


protected:
    void showEvent(QShowEvent *event);

public slots:
    void actionTriggered(QAction *action);
    void newFile();
    void saveFile();
    void openFile();
    void assemble();
    void Disassemble();
    void devInfo();
    void GenCoe();
    void Step(int line);
    void Stop(void);
    void setCorner(int id);
    //void switchLayoutDirection();
    void setDockOptions();
    void showAsm(int error=-1);
    void showObj(int errorLine=-1, int runningLine=-1, bool hex=true);

    //void Exception(QString information);

    //void createDockWidget();
    //void destroyDockWidget(QAction *action);

private:
    QString filename;
    QString fileAsmName;
    QString fileXmlName;
    QString fileObjName;
    QString fileObjHexName;
    QString fileCoeName;
//    QFile fileAsm;
//    QFile fileXml;
//    QFile fileObj;
//    QFile fileCoe;

    int currentRunningLine;
    bool stop;

    void setupToolBar();
    void setupMenuBar();
    void setupDockWidgets(void);
    void SetMemoryWindow(void);
    void SetRegisterWindow(void);
    //void setupDockWidgets(const QMap<QString, QSize> &customSizeHints);
};


#endif

