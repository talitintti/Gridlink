#include "datahandler.h"
#include <QStandardPaths>
#include <QFile>


DataHandler::DataHandler() {}
DataHandler::~DataHandler() {
}

QString DataHandler::Initialize() {
    QString error_msg = QString();
    // create config file
    // read config file
    // provide members with necessary values from config file

    ReadConfigFile();
    if (!error_msg.isEmpty()) {
        return error_msg;
    }


    return error_msg;
}



Config DataHandler::ReadConfigFile() {
    QString error_msg = QString();

    QString configFolderPath = QStandardPaths::writableLocation(
        QStandardPaths::ConfigLocation
        );

    if (configFolderPath.isEmpty()) {
    }

    QString configFilePath = configFolderPath + "/Gridlink/config.yaml";
    QFile configFile(configFilePath);
    if (!configFile.exists()) {
        CreateConfigFile(configFile);
    }
    else {
        ReadConfigFile();
    }

    return

}



QString DataHandler::CreateConfigFile(QFile configFile) {

}
