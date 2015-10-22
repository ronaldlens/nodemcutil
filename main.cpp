#include <QCoreApplication>
#include <QTextStream>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QString>
#include <QSerialPortInfo>
#include <QSettings>

QSettings *settings;
QTextStream output(stdout);

QString defaultPortName;
int defaultBaudRate;
bool verbose;

void setPortName(QString portName)
{
    // check whether a number was provided
    bool ok;
    int index = portName.toInt(&ok);

    // if it's a number, check whether index fits and set default
    if (ok) {
        if (index > QSerialPortInfo::availablePorts().length()) {
            output << "error: index " << index << " out of range to set portname" << endl;
            exit(1);
        }
        defaultPortName = QSerialPortInfo::availablePorts().at(index-1).portName();
    } else {
        // otherwise check whether name exists
        foreach(auto port, QSerialPortInfo::availablePorts()) {
            if (port.portName().compare(portName)==0) {
                defaultPortName = portName;
                break;
            }
        }
        output << "error: portname '" << portName << "' not found to set portname" << endl;
        exit(1);
    }
    settings->setValue("defaultPortName", defaultPortName);
    if (verbose) {
        output << "now using default portname: '" << defaultPortName << "." << endl;
    }
}

void setBaudRate(QString baudRate)
{
    bool ok;
    int baud = baudRate.toInt(&ok);
    if (ok) {
        defaultBaudRate = baud;
        settings->setValue("defaultBaudRate", defaultBaudRate);

        if (verbose) {
            output << "now using default baudrate " << defaultBaudRate << " baud" << endl;
        }
    } else {
        output << "error, baudrate '" << baudRate << "' not a legal value" << endl;
        exit(1);
    }
}

void readDefaultSettings(void)
{
    // read settings for default values
    if (settings->contains("defaultPortName")) {
        defaultPortName = settings->value("defaultPortName").toString();
        bool found = false;
        foreach(auto port, QSerialPortInfo::availablePorts()) {
            if (defaultPortName.compare(port.portName()) == 0) {
                found = true;
            }
        }
        if (!found) {
            if (verbose) {
                output << "default portname '" << defaultPortName << "' not available." << endl;
            }
            defaultPortName = "";
        } else if (verbose) {
            output << "read default portname '" << defaultPortName << "' from settings" << endl;
        }
    }
    if (settings->contains("defaultBaudRate")) {
        defaultBaudRate = settings->value("defaultBaudRate").toInt();
        if (verbose) {
            output << "read default baudrate " << defaultBaudRate << " baud" << endl;
        }
    } else {
        setBaudRate("9600");
    }

}

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
    verbose = parser.isSet(verboseOption);

    if (verbose) {
        output << "nodemcutil 0.1" << endl << "(c)2015 R.F. Lens" << endl;
    }

    readDefaultSettings();

    // process settings
    if (parser.isSet(portOption)) {
        setPortName(parser.value(portOption));
    }
    if (parser.isSet(baudOption)) {
        setBaudRate(parser.value(baudOption));
    }

    // check commands
    if (parser.isSet(listPortsCmd)) {
        listSerialPorts();
    }
    output << endl;
    return 0;
}

