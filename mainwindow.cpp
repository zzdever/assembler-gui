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
#include <QTableView>
#include <QHeaderView>
#include <QTableWidget>
#include <sstream>
#include <QStandardItem>
#include "modules/modules.h"
#include "modules/assembler.h"


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
    center->setReadOnly(true);
    center->setMinimumSize(200, 205);
    //setCentralWidget(center);

    QSplitter *splitter= new QSplitter;
    splitter->setHandleWidth(0);
    splitter->addWidget(left);
    splitter->addWidget(center);

    currentRunningLine=0;
    stop=false;


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

    connect(tb,&ToolBar::Step,this,&MainWindow::Step);
    connect(tb,&ToolBar::Stop,this,&MainWindow::Stop);

    return;
}

//void MainWindow::Exception(QString information)
//{
//    qDebug()<<"information from others: "<<information;
//}

void MainWindow::SetMemoryWindow()
{
    QStandardItemModel *model=new QStandardItemModel(MEMORYSIZE/4,2);
    QModelIndex index;
    QString string;

    quint32 memoryContent;
    model->setHeaderData(0,Qt::Horizontal,tr("address"));
    model->setHeaderData(1,Qt::Horizontal,tr("content"));
    for(int row=0;row<MEMORYSIZE/4;row++)
    {
        for(int col=0;col<2;col++)
        {

            index = model->index(row, col, QModelIndex());
            if(col==0)
            {
                string=QString("%1").arg(row*4,8,16,QChar('0'));
                model->setData(index,string);
            }
            if(col==1)
            {
                memoryContent=emulator.memArray[row*4]<<24 | emulator.memArray[row*4+1]<<16
                            | emulator.memArray[row*4+2]<<8 | emulator.memArray[row*4+3];
                string=QString("%1").arg(memoryContent,8,16,QChar('0'));
                model->setData(index,string);
            }

        }
    }
    memoryTable->setModel(model);
    memoryTable->show();
}


void MainWindow::SetRegisterWindow()
{
    QStandardItemModel *model=new QStandardItemModel(REGISTERSETSIZE,2);
    QModelIndex index;
    QString string;

    model->setHeaderData(0,Qt::Horizontal,tr("name"));
    model->setHeaderData(1,Qt::Horizontal,tr("content"));
    for(int row=0;row<REGISTERSETSIZE;row++)
    {
        for(int col=0;col<2;col++)
        {

            index = model->index(row, col, QModelIndex());
            if(col==0)
            {
                string="#"+QString("%1").arg(row)+":"+QString("$")+registerSet[row].name;
                model->setData(index,string);
            }
            if(col==1)
            {
                string=QString("%1").arg(emulator.registerArray[row],8,16,QChar('0'));
                model->setData(index,string);
            }

        }
    }
    registerTable->setModel(model);
    registerTable->show();
}


void MainWindow::Stop()
{
    currentRunningLine=0;
    emulator.Init();
    SetMemoryWindow();
    SetRegisterWindow();
}

void MainWindow::Step(int line)
{
    if(fileObjName==""){
        qDebug()<<"No obj file opened";
        return;
    }

    if(line==NEXT){
        emulator.Step(fileObjName,currentRunningLine,currentRunningLine+1);
        currentRunningLine=emulator.registerArray[PC]/4;
    }
    else if(line==RUN){
        while(emulator.Step(fileObjName,currentRunningLine,line)!=-1);
        currentRunningLine=emulator.registerArray[PC]/4;
    }
    else{
        for(int i=0;i<line-currentRunningLine;i++){
            emulator.Step(fileObjName,currentRunningLine,line);
            currentRunningLine=emulator.registerArray[PC]/4;
        }
    }

 qDebug()<<"return: "<<currentRunningLine;
    showObj();
    SetMemoryWindow();
    SetRegisterWindow();
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
    connect(action, SIGNAL(triggered()), this, SLOT(saveFile()));

//    action = menu->addAction(tr("Save as"));
//    connect(action, SIGNAL(triggered()), this, SLOT(saveFile()));
    menu->addSeparator();
    menu->addAction(tr("Quit"), this, SLOT(close()));

    //Build
    mainWindowMenu = menuBar()->addMenu(tr("Build"));

    action = mainWindowMenu->addAction(tr("Assemble"));
    connect(action, SIGNAL(triggered()), this, SLOT(assemble()));

    action = mainWindowMenu->addAction(tr("Disassemble"));
    connect(action, SIGNAL(triggered()), this, SLOT(Disassemble()));

    action = mainWindowMenu->addAction(tr("Gen coe"));
    connect(action, SIGNAL(triggered()), this, SLOT(GenCoe()));

    //About
    QMenu *aboutMenu = menuBar()->addMenu(tr("About"));

    action = aboutMenu->addAction("author");
    connect(action, SIGNAL(triggered()), this, SLOT(devInfo()));


    //QMenu *toolBarMenu = menuBar()->addMenu(tr("Tool bars"));
    //for (int i = 0; i < toolBars.count(); ++i)
    //  toolBarMenu->addMenu(toolBars.at(i)->menu);

    dockWidgetMenu = menuBar()->addMenu(tr(""));
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


    static const struct Set {
        const char * name;
        uint flags;
        Qt::DockWidgetArea area;
    } sets [] = {
        //{ "Memory", 0, Qt::LeftDockWidgetArea },
        //{ "Register", 0, Qt::RightDockWidgetArea },
        //{ "Red", 0, Qt::TopDockWidgetArea },
        //{ "Green", 0, Qt::TopDockWidgetArea },
        //{ "Blue", 0, Qt::BottomDockWidgetArea },
        { "Info", 0, Qt::BottomDockWidgetArea }
    };
    const int setCount = sizeof(sets) / sizeof(Set);

    setCorner(0);

    for (int i = 0; i < setCount; ++i)
    {
        ColorSwatch *swatch = new ColorSwatch(tr(sets[i].name), this, Qt::WindowFlags(sets[i].flags));

        QSize size(250,80);
        swatch->setCustomSizeHint(size);
        addDockWidget(sets[i].area, swatch);


//        dockWidgetMenu->addMenu(swatch->menu);
    }

    registerDock = new QDockWidget(tr("Register"), this);
    registerDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    registerTable = new QTableView(registerDock);
    QStandardItemModel *registerModel = new QStandardItemModel(32, 2);
    registerModel->setHeaderData(0,Qt::Horizontal,QObject::tr("name"));
    registerModel->setHeaderData(1,Qt::Horizontal,QObject::tr("content"));
    //QTableModel::Tab_Model *delegate = new Tab_Model;
    registerTable->setModel(registerModel);
    registerTable->verticalHeader()->hide();
    registerTable->horizontalHeader()->setStretchLastSection(true);
    registerDock->setMaximumWidth( 327 );
    registerDock->setWidget(registerTable);
    addDockWidget(Qt::RightDockWidgetArea, registerDock);


    memoryDock = new QDockWidget(tr("Memory"), this);
    memoryDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    memoryTable = new QTableView(memoryDock);
    QStandardItemModel *memoryModel = new QStandardItemModel(MEMORYSIZE/4,2);
    memoryModel->setHeaderData(0,Qt::Horizontal,QObject::tr("address"));
    memoryModel->setHeaderData(1,Qt::Horizontal,QObject::tr("content"));
    //memoryModel->
    //QTableModel::Tab_Model *delegate = new Tab_Model;
    memoryTable->setModel(memoryModel);
    memoryTable->verticalHeader()->hide();
    memoryTable->horizontalHeader()->setStretchLastSection(true);
    //memoryTable->resizeColumnToContents(0);
    memoryDock->setWidget(memoryTable);
    addDockWidget(Qt::LeftDockWidgetArea, memoryDock);

}

void MainWindow::   setCorner(int id)
{
    QMainWindow::setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    QMainWindow::setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

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


void MainWindow::openFile()
{
    QString nameStr = QFileDialog::getOpenFileName(this, tr("Open ASM File"));
    if (nameStr.isEmpty())
        return;

    left->setText("");
    center->setText("");
    if(nameStr.endsWith(".asm",Qt::CaseInsensitive))
    {
        //if(!fileAsmName.isEmpty()) fileAsm.close();
        fileAsmName=nameStr;
        //fileAsmSaved=true;
        //fileAsmEdited=false;
        filename=fileAsmName;
        filename.truncate(filename.length()-4);
        showAsm(-1);
    }
    else if(nameStr.endsWith(".obj",Qt::CaseInsensitive))
    {
        fileObjName=nameStr;
        //fileObjSaved=true;
        //fileObjEdited=false;
        filename=fileObjName;
        filename.truncate(filename.length()-4);

        assembler.DisAssem(filename);
        fileAsmName=filename+".asm";
        assemble();
        left->setText("");

        showObj();
    }
    else
    {
        QString msg = tr("Unsupported file type");
        QMessageBox::warning(this, tr("Error"), msg);
    }


    Stop();  //initialize memory and registers

}

void MainWindow::newFile()
{
    left->setText("");
    fileAsmName="";
}

void MainWindow::saveFile()
{
    QString nameStr;
    if(fileAsmName.isEmpty() )
    {
        nameStr= QFileDialog::getSaveFileName(this, tr("Save ASM file"));
        filename=nameStr;
    }
    else
        nameStr=fileAsmName;

    fileAsmName=filename+".asm";

    if (nameStr.isEmpty())
        return;

    QFile file(fileAsmName);
    if (!file.open(QFile::WriteOnly)) {
        QString msg = tr("Failed to open %1\n%2")
                        .arg(nameStr)
                        .arg(file.errorString());
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }
    QTextStream stream(&file);

    stream<<left->toPlainText();

    file.close();

//    QByteArray geo_data = saveGeometry();
//    QByteArray layout_data = saveState();

//    bool ok = file.putChar((uchar)geo_data.size());
//    if (ok)
//        ok = file.write(geo_data) == geo_data.size();
//    if (ok)
//        ok = file.write(layout_data) == layout_data.size();

//    if (!ok) {
//        QString msg = tr("Error writing to %1\n%2")
//                        .arg(fileName)
//                        .arg(file.errorString());
//        QMessageBox::warning(this, tr("Error"), msg);
//        return;
//    }
}


void MainWindow::assemble()
{
    if(filename.isEmpty())return;
    assembler.Assemble(filename);
    fileXmlName=filename+".xml";
    fileObjName=filename+".obj";
    fileObjHexName=filename+".objh";
    showObj();
}

void MainWindow::Disassemble()
{
    if(filename.isEmpty()) return;
    showAsm();
}

void MainWindow::GenCoe()
{
    if(fileObjName.isEmpty())
    {
        QString msg = tr("Assemble first!");
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }

    QString nameStr = QFileDialog::getSaveFileName(this, tr("Save COE file to"));
    if (nameStr.isEmpty())
        return;

    nameStr+=".coe";
    QFile file(nameStr);
    if (!file.open(QFile::WriteOnly)) {
        QString msg = tr("Failed to open %1\n%2")
                        .arg(nameStr)
                        .arg(file.errorString());
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }
    QTextStream stream(&file);

    QFile fileObjH(fileObjHexName);
    if (!fileObjH.open(QFile::ReadOnly)) {
        QString msg = tr("Failed to open %1\n%2")
                        .arg(fileObjHexName)
                        .arg(file.errorString());
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }
    QTextStream streamObjH(&fileObjH);

    stream<<"memory_initialization_radix=16;"<<endl
         <<"memory_initialization_vector="<<endl;

    while(!streamObjH.atEnd())
    {
        stream<<streamObjH.readLine();
        stream<<" ";
    }
    stream <<";";

    file.close();
    fileObjH.close();
}

void MainWindow::showAsm(int error)
{
    if(fileAsmName.isEmpty() && fileXmlName.isEmpty()) return;

    left->setText("");

    bool xml;
    QFile file;
    if(fileAsmName.isEmpty())
    {
        file.setFileName(fileXmlName);
        xml=true;
    }
    else
    {
        file.setFileName(fileAsmName);
        xml=false;
    }

    if (!file.open(QFile::ReadOnly)) {
        QString msg = tr("Failed to open %1\n%2")
                        .arg(fileAsmName)
                        .arg(file.errorString());
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }
    QTextStream stream(&file);

    QString line;
    int lineNumber=1;
    QColor textColor(0,0,0);
//    if(xml)
//    {
//        while(!stream.atEnd()){
//            stream>>line;

//            if(line.compare("<blankline/>")==0)
//                continue;
//            else if(line.compare("<linenumber>")==0)
//            {
//                stream.readLine();
//                continue;
//            }
//            else if(line.compare("<comment>")==0)
//            {
//                textColor.setRgb(120,120,120);  //comment
//                left->setTextColor(textColor);
//                line=stream.readLine();
//                line.truncate(line.length()-11);
//                left->append("#"+line);
//            }
//            else if(line.compare("<instruction>")==0)
//            {
//                line=stream.read();
//                textColor.setRgb(216,127,98);  //instruction
//                left->setTextColor(textColor);
//                left->insertPlainText(line);
//                stream.readLine();

//                while(1)
//                {
//                    line=stream.read();
//                    if(line.compare("<register>")==0)
//                    {
//                        textColor.setRgb(135,153,179);
//                        left->setTextColor(textColor);
//                        left->insertPlainText(" "+line);
//                        stream.readLine();
//                        continue;
//                    }

//                    if(line.compare("<register>")==0)
//                    {
//                        textColor.setRgb(135,153,179);
//                        left->setTextColor(textColor);
//                        left->insertPlainText(" "+line);
//                        stream.readLine();
//                        continue;
//                    }

//                }
//            }

//            stream.readLine();
//        }

//    }
//    else
        while(!stream.atEnd())
        {
            line=stream.readLine();
            textColor.setRgb(0,0,0);
            if(lineNumber==error)
            {
                textColor.setRgb(219,21,49);  //error color
                left->setFontUnderline(true);
            }
            left->setTextColor(textColor);
            left->append(line);
    //        textColor.setRed(216);
    //        textColor.setGreen(127);
    //        textColor.setBlue(98);

            left->setFontUnderline(false);

            lineNumber++;
        }


    file.close();
}

void MainWindow::showObj(int errorLine, int runningLine, bool hex)
{
    if(fileObjName.isEmpty()) return;

    center->setText("");

    QFile file(fileObjHexName);
    QTextStream textStream;
    QDataStream dataStream;

    file.setFileName(fileObjHexName);
    if (!file.open(QFile::ReadOnly)) {
        QString msg = tr("Failed to open %1\n%2")
                        .arg(fileObjHexName)
                        .arg(file.errorString());
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }
    textStream.setDevice(&file);


    QString line;
    //std::stringstream line;
    int lineNumber=1;
    int address=0;
    QColor textColor(120,120,120);

    while(!textStream.atEnd())
    {
        line=textStream.readLine();

        center->setTextColor(textColor);
        //left->insertPlainText(line+"  ");

        textColor.setRgb(0,0,0);
        if(errorLine==lineNumber){
            textColor.setRgb(219,21,49);  //error color
            center->setFontUnderline(true);
        }
        if(currentRunningLine==lineNumber) textColor.setRgb(255,127,0);  //running color
        center->setTextColor(textColor);
        center->append(line);


        lineNumber++;
        address+=4;
    }

    QTextCursor textCursor = center->textCursor();
    textCursor.movePosition(QTextCursor::Start);
    for (int i=0;i<currentRunningLine-1;i++)
        textCursor.movePosition(QTextCursor::Down);
    center->setTextCursor(textCursor);

}

void MainWindow::devInfo()
{
    QString msg = tr("3120101966 应哲敏\nzzdever@gmail.com");
    QMessageBox::about(this,tr("about"),msg);
    return;
}
