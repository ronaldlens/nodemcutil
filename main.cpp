#include <QCoreApplication>
#include <QTextStream>
#include <QCommandLineParser>
#include <QStringList>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTextStream output(stdout);
    output << "nodemcutil 0.1 " << endl;

    QCommandLineParser parser;

    parser.setApplicationDescription("Serial tool for NodeMCU");
    parser.addHelpOption();
    parser.addPositionalArgument("command", "command");
    parser.process(app);
    QStringList args = parser.positionalArguments();
    QString command = args.at(0);

    output << "Command: " << command << endl;

    return 0;
}

