#include <QCoreApplication>
#include <QTextStream>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QString>
#include <QSerialPortInfo>
#include <QSettings>

QTextStream output(stdout);
QString defaultPortName;
int defaultBaudRate;
bool verbose;
QSettings *settings;

void listSerialPorts(void)
{
    output << "Serial ports:" << endl;
    int i=1;
    foreach(auto port, QSerialPortInfo::availablePorts()) {
        output << "  " << i++ << ": " << port.portName();
        if (port.portName().compare(defaultPortName)==0) {
            output << " (*)";
        }
        output << endl;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setOrganizationName("SwiepSoft");
    QCoreApplication::setOrganizationDomain("swiepsoft.com");
    QCoreApplication::setApplicationName("NodeMCUtil");

    settings = new QSettings();

    // setup command line for parsing
    QCommandLineParser parser;
    parser.setApplicationDescription("Serial tool for NodeMCU");
    parser.addHelpOption();

    // command line parameter options
    QCommandLineOption portOption(QStringList() << "p" << "port",
                                  "use <port>", "port");
    QCommandLineOption baudOption(QStringList() << "b" << "baud",
                                   "use <speed> baud (default 9600)", "baud");
    QCommandLineOption verboseOption(QStringList() << "v" << "verbose",
                                     "use verbose output");
    parser.addOption(portOption);
    parser.addOption(baudOption);
    parser.addOption(verboseOption);

    // command line commands
    QCommandLineOption listPortsCmd(QStringList() << "l" << "listports",
                                       "list available serial ports");
    parser.addOption(listPortsCmd);

    // process commandline
    parser.process(app);
    if (parser.isSet(verboseOption)) {
        verbose=true;
    }

    if (verbose) {
        output << "nodemcutil 0.1" << endl << "(c)2015 R.F. Lens" << endl;
    }

    // read settings for default values
    if (settings->contains("defaultPortName")) {
        defaultPortName = settings->value("defaultPortName").toString();
        if (verbose) {
            output << "read default portname '" << defaultPortName << "' from settings" << endl;
        }
    }
    if (settings->contains("defaultBaudRate")) {
        defaultBaudRate = settings->value("defaultBaudRate").toInt();
        if (verbose) {
            output << "read default baudrate " << defaultBaudRate << " baud" << endl;
        }
    }

    // process settings
    if (parser.isSet(portOption)) {
        defaultPortName = parser.value(portOption);
        settings->setValue("defaultPortName", defaultPortName);
        if (verbose) {
            output << "now using default portname: '" << defaultPortName << "." << endl;
        }
    }
    if (parser.isSet(baudOption)) {
        defaultBaudRate = parser.value(baudOption).toInt();
        settings->setValue("defaultBaudRate", defaultBaudRate);
        if (verbose) {
            output << "now using default baudrate " << defaultBaudRate << " baud" << endl;
        }
    }

    if (parser.isSet(listPortsCmd)) {
        listSerialPorts();
    } else {
        output << "no command supplied!" << endl << parser.helpText();
    }
    output << endl;
    return 0;
}

