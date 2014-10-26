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

#include <QApplication>
#include <QPainterPath>
#include <QPainter>
#include <QMap>
#include <qdebug.h>
#include "modules/modules.h"

void usage()
{
    qWarning() << "Usage: mainwindow [-SizeHint<color> <width>x<height>] ...";
    exit(1);
}

//QMap<QString, QSize> parseCustomSizeHints(int argc, char **argv)
//{
//    QMap<QString, QSize> result;

//    for (int i = 1; i < argc; ++i) {
//        QString arg = QString::fromLocal8Bit(argv[i]);

//        if (arg.startsWith(QLatin1String("-SizeHint"))) {
//            QString name = arg.mid(9);
//            if (name.isEmpty())
//                usage();
//            if (++i == argc)
//                usage();
//            QString sizeStr = QString::fromLocal8Bit(argv[i]);
//            int idx = sizeStr.indexOf(QLatin1Char('x'));
//            if (idx == -1)
//                usage();
//            bool ok;
//            int w = sizeStr.left(idx).toInt(&ok);
//            if (!ok)
//                usage();
//            int h = sizeStr.mid(idx + 1).toInt(&ok);
//            if (!ok)
//                usage();
//            result[name] = QSize(w, h);
//        }
//    }

//    return result;
//}


int main(int argc, char **argv)
{
    QApplication app(argc, argv);
//    QMap<QString, QSize> customSizeHints = parseCustomSizeHints(argc, argv);
    MainWindow mainWin;
    mainWin.resize(1024, 576);
    mainWin.show();
    return app.exec();
}


