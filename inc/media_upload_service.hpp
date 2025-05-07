#ifndef MEDIA_UPLOAD_SERVICE_HPP
#define MEDIA_UPLOAD_SERVICE_HPP

#include "ServiceHandler.hpp"
#include <string>
// #include "transactionLogs.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include "../Logger/logger/logger_api.hpp"
// #include "logger_api.hpp"

// #include <opencv2/opencv.hpp>
using namespace SF;
using namespace LOGGER;

class MediaUploadService
{
private:
    ServiceHandler *serHandler; // Private variable
    // tLogs::transactionLogs *tlogs;
    // Logger *module_plogger;
    // to check that the message is received on config topic or not
    int flag = 0;

public:
    // Initialization function
    void init(ServiceHandler *Handler, const std::string &configDirPath);
    // Encoding image functions
    int encodeimage(const nlohmann::json &data, const nlohmann::json &media);

    // Encoding video functions
    int encodevideo(const nlohmann::json &data, const nlohmann::json &media);

    //  Encoding snapshot functions
    int encodesnapshot(const nlohmann::json &data);

    // Member function to post media
    eErrorCode processMedia(const std::string inputDataName, uint8_t *inputData, size_t *size);

    // getFlag function
    int getFlag();

    // updateFlag function
    int updateFlag();

    // Read config file
    nlohmann::json readConfigFile(const std::string &configFilePath);

    // Write in json file
    int writeInJsonFile(std::string &jsonData);
};

#endif // MEDIA_UPLOAD_SERVICE_HPP
