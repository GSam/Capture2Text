/*
Copyright (C) 2010-2017 Christopher Brochtrup

This file is part of Capture2Text.

Capture2Text is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Capture2Text is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Capture2Text.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "CommandLine.h"

#ifndef CLI_BUILD
    #include <QApplication>
    #include <RunGuard.h>
    #include "MainWindow.h"
#else
    #include <QGuiApplication>
#endif

int main(int argc, char *argv[])
{
    /* From http://www.qtcentre.org/threads/60203-QApplication-QGuiApplication-and-QCoreApplication:
       "QCoreApplication is the base class, QGuiApplication extends the base class with functionality
       related to handling windows and GUI stuff (non-widget related, e.g. OpenGL or QtQuick),
       QApplication extends QGuiApplication with functionality related to handling widgets." */
#ifndef CLI_BUILD
    QApplication app(argc, argv);
    bool portable = false;
#else
    QGuiApplication  app(argc, argv);
#endif
    QCoreApplication::setOrganizationName("Capture2Text");
    QCoreApplication::setOrganizationDomain("Capture2Text.com");
    QCoreApplication::setApplicationName("Capture2Text");
    QCoreApplication::setApplicationVersion("4.5.1");

#ifndef CLI_BUILD
    if(argc > 1)
#endif
    {
        CommandLine cmdLine;
        bool success = cmdLine.process(app);
#ifndef CLI_BUILD
        if(success && cmdLine.getPortable())
        {
            portable = true;
        }
        else
        {
            return (int)(!success);
        }
#else
        return (int)(!success);
#endif
    }

#ifndef CLI_BUILD
    // Force single instance
    RunGuard guard("Capture2Text-42e81c93-4f7c-4d68-9688-0b7f75a427cc");

    if(!guard.tryToRun())
    {
        return 0;
    }

    // Ensure that closing a message box does not exit the program
    app.setQuitOnLastWindowClosed(false);
    MainWindow w(portable); // The constructor will setup everything

    return app.exec();
#endif
}
