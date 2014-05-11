﻿#include <QString>
#include <QRegExp>
#include <QDebug>
#include <iostream>
#include "assembler.h"

MatchTable::MatchTable(void)
{
    registerPatternName.setPattern("^\\$([a-zA-Z]+\\d*)$");
    registerPatternName.setCaseSensitivity(Qt::CaseInsensitive);
    registerPatternNumber.setPattern("^\\$(\\d+)$");
}

int MatchTable::MatchInstruction(QString instruct)
{
    for(int i=0;i<INSTRUCTIONSETSIZE;i++)
    {
        if(0==instruct.compare(coreInstructionSet[i].mnemonic)) return i;
    }

    return -1;
}

int MatchTable::MatchRegister(QString registerName)
{
    registerName=registerName.toLower();

    if(registerPatternNumber.indexIn(registerName)>=0)
        return registerPatternNumber.capturedTexts()[1].toInt();
    else if(registerPatternName.indexIn(registerName)>=0)
    {
        registerName=registerPatternName.capturedTexts()[1];
        for(int i=0;i<REGISTERSETSIZE;i++)
        {
            if(0==registerName.compare(registerSet[i].name)) return i;
        }
    }

    return -1;
}

int MatchTable::DisassemMatchInstruction(unsigned int opcode, unsigned int funct)
{
    for(int i=0;i<INSTRUCTIONSETSIZE;i++)
    {
        if(i==0 || i==3 || i==4 || i==10
                || i==16 || i==17 || i==19
                || i==22 || i==23 || i==24
                || i==29 || i==30 )
        {
            if(opcode==coreInstructionSet[i].opcode && funct ==coreInstructionSet[i].funct)
                        return i;
        }
        else
        {
            if(opcode==coreInstructionSet[i].opcode)
                        return i;
        }
    }
    return -1;
}

int MatchTable::instructionEncode(QTextStream &streamXml, QString type, LookUpTable labelTable)
{
    int matchId;

    streamXml>>line;
    if(line!=type)
    {
        streamXml>>line;
        qDebug()<<line<<"Incompatible operand. A "<<type<<" is expected";
        streamXml.readLine();
        return -1;
    }

    if(line.compare("<register>")==0)
    {
        streamXml>>line;
        matchId=MatchRegister(line);

        if(matchId<0)
            qDebug()<<line<<"Unknown register number/name";
        else
        {
            streamXml.readLine();
            return registerSet[matchId].number;
        }
    }
    else if(line.compare("<parameter>")==0)
    {
        streamXml>>line;
        if((line[0]=='0'&&line[1]=='x') || (line[0]=='0'&&line[1]=='X'))
        {
            streamXml.readLine();
            int num=hexTextToInt(line);
//qDebug()<<"parameter match:"<<num;
            return num;
        }
        else
        {
//qDebug()<<"parameter match:"<<line.toInt();
            streamXml.readLine();
            return line.toInt();
        }
    }
    else if(line.compare("<reference>")==0)
    {
        streamXml>>line;
        unsigned int address=0;
        address=labelTable.LookUp(line);
        if(address<0xffffffff)  //address < 0xffffffff
        {
            streamXml.readLine();
            return address;
        }
        else
        {
            qDebug()<<"Unrecognized label reference "<<line;
        }
    }

    streamXml.readLine();
    return -1;
}

int MatchTable::hexTextToInt(QString line)
{
    int num=0;
    for(int i=2;i<line.length();i++)
    {
        if(line.toStdString()[i]>='a' && line.toStdString()[i]<='f')
            num = num * 16 + line.toStdString()[i]-'a'+10;
        else if(line.toStdString()[i]>='A' && line.toStdString()[i]<='F')
            num = num * 16 + line.toStdString()[i]-'A'+10;
        else
            num = num * 16 + line.toStdString()[i]-'0';
    }

    return num;
}
