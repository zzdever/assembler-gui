#ifndef MODULES_H
#define MODULES_H

#include <QString>
//#include <QObject>
#include "assembler.h"

#define MEMORYSIZE (512)  //1KB memory
#define PC (REGISTERSETSIZE)

class Assembler{
public:
    int Assemble(QString filename);
    int DisAssem(QString filename);

};

class Emulator{

private:
    uint programSize;
    //QString errorInformation;

public:
    Emulator(void);
    int registerArray[REGISTERSETSIZE+1];
    unsigned char memArray[MEMORYSIZE];
    //quint32 PC;
    bool isProgramLoaded;

    int Step(QString filename, int beginLine=0, int line=0);
    void Init(void);
    void LoadProgram(QString filename);

};


#endif // MODULES_H

