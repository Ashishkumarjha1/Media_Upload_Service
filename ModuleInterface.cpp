/**********************************************************************************************************
 *  The copyright of this software is owned by AllGoVision Technologies Pvt Ltd. All rights are reserved.  *
 * (C) ALLGOVISION TECHNOLOGIES PVT. LTD.                                                                  *
 *                                                                                                         *
 ***********************************************************************************************************
 *
 *   \filename ModuleInterface.cpp
 *
 *   CHANGE HISTORY
 *   --------------
 *
 *   dd/mm/yy      Code Ver      Description                       Author
 *   --------      -------       -----------                      -------
 *   13/10/2023      1.0.3       Initial version                    Ashish
 *   31/10/2023      1.0.4       Modified                           Ashish
 *   08/11/2023      1.0.5       ServiceDiscovery inegration        Ashish
 *******************************************************************************/
#include "ModuleInterface.hpp"
#include "media_upload_service.hpp"
// #include "../Logger/logger/logger_api.hpp"
#include "logger_api.hpp"

using namespace SF;
using namespace LOGGER;
//  Constuctor
ModuleInterface::ModuleInterface()
{
    serHandler = nullptr;
    moduleInf = nullptr;
    moduleInternalClass = nullptr;

    // serFactory=nullptr;
}

// For now its dummy function
dependencyList ModuleInterface::getDependencyServicesList()
{
    dependencyList dependencyList;
    return dependencyList;
}

std::vector<std::string> ModuleInterface::getInputDataNames()
{
    std::vector<std::string> name;
    name.push_back("testType");
    return name;
}

std::vector<std::string> ModuleInterface::getOutputDataNames()
{
    std::vector<std::string> name;
    name.push_back("testType");
    return name;
}

eErrorCode ModuleInterface::initialize(ServiceHandler *Handler, const std::map<std::string, std::string> &initializationParameters)
{
    serHandler = Handler;
    const std::string &configDirPath = initializationParameters.at("configDirPath");
    // std::string logFilePath = configDirPath + "log/log.txt";
    std::string logFilePath = "../configFiles/log/log.txt";
    // std::cout << logFilePath << std::endl;
    // Logger *pLogger = LoggerCreator::get_logger(LOGGER_TYPE_FILE, ALogLevel::DEBUG, logFilePath.c_str());
    ALOG(pLogger, DEBUG, "inside module_interfaceinit");
    // std::string logfilepath = configDirPath + "log/log.txt";
    // Logger *plogger = LoggerCreator::get_logger(LOGGER_TYPE_FILE, DEBUG, logfilepath.c_str());

    // Create an instance of MediaUploadService
    MediaUploadService *mediaService = new MediaUploadService();

    // Call the init method of MediaUploadService
    mediaService->init(serHandler, configDirPath);

    // Set the moduleInternalClass to the instance of MediaUploadService
    moduleInternalClass = static_cast<void *>(mediaService);
    return eErrorCode::SUCCESS;
}

// For now its dummy function
std::vector<std::string> ModuleInterface::getResourceRequirements()
{
    std::vector<std::string> resrcReq;
    return resrcReq;
}

eErrorCode ModuleInterface::deinitialize()
{
    return eErrorCode::SUCCESS;
}

uint8_t *ModuleInterface::handleInputData(const std::string inputDataName, uint8_t *inputData, size_t *size, const int channelID)
{

    // Ensure that size is not NULL and that it points to a valid size value.
    if (size == nullptr || *size == 0)
    {
        std::cerr << "Invalid size argument." << std::endl;
        return nullptr;
    }
    std::cout << std::flush;

    MediaUploadService *retrievedMediaService = static_cast<MediaUploadService *>(moduleInternalClass);

    if (inputDataName == "media-config")
    { // Convert inputData (uint8_t*) to std::string

        std::string jsonString(reinterpret_cast<char *>(inputData));
        int i = retrievedMediaService->writeInJsonFile(jsonString);
        std::cout << "Data written in json file" << std::endl;
        std::string configFilePath = "../configFiles/config.json";
        // Read the config file
        nlohmann::json configData = retrievedMediaService->readConfigFile(configFilePath);

        // Update the flag
        retrievedMediaService->updateFlag();

        // Get the url, username and password from the config file
        std::string url = configData.value("cloudURL", "");
        std::string username = configData.value("username", "");
        std::string password = configData.value("password", "");
        eBusType bus = MQTT_BUS;
        std::string outputDataType = "media-data";

        // call the setOutputMessageBus function
        eErrorCode code1 = serHandler->setOutputMessageBus(bus, outputDataType, username, password, url);
    }
    else
    { // Check if the flag is set
        if (retrievedMediaService->getFlag() == 1)
        { // Process the media
            std::cout << "Processing media" << std::flush << std::endl;
            retrievedMediaService->processMedia(inputDataName, inputData, size);
        }
        else
        {
            std::cout << "No connection";
        }
    }

    return NULL;
}

void ModuleInterface::setModule(void *module)
{
    ModuleInterface *moduleInf = static_cast<ModuleInterface *>(module);
    this->moduleInf = moduleInf;
}

// getControl endpoints which returns handler and service name
controlEndpointVector ModuleInterface::getControlEndpoints()
{

    controlEndpointVector endpoints;

    // requestHandler function to handler requests
    auto handler = [this](eRequestType reqType, const std::string &path, responseData resData, eDataType dataType, std::string userInfo) -> requestResponse
    {
        // Convert responseData to a string
        std::string responseString(resData.begin(), resData.end());

        // Deserialize the JSON string back to a C++ struct
        nlohmann::json response_json = nlohmann::json::parse(responseString);
        std::cout << response_json << std::endl;

        std::string ServiceName = response_json["serviceName"].get<std::string>();
        std::string ServiceType = response_json["serviceType"].get<std::string>();
        int channelId = response_json["channelId"].get<int>();

        std::string inpData = "testType";
        size_t size = resData.size();
        requestResponse response;
        std::string bodyString;
        responseData data;
        uint8_t *unsignedData = resData.data();
        eErrorCode code;
        sServiceData serviceData;
        serviceData.serviceName = ServiceName;
        serviceData.serviceType = ServiceType;
        if (reqType == SFTypes::POST)
        {
            if (path == "/addChannel")
            {

                code = serHandler->sendReadInputCommand(serviceData, inpData, channelId, size, NULL, eCommand::START); // start recieving data
                if (code != eErrorCode::SUCCESS && code == eErrorCode::SERVICE_NOT_FOUND)
                    printf("Error: Service is not Up\n");

                code = serHandler->writeOutputData(inpData, channelId, unsignedData, size);
                if (code != eErrorCode::SUCCESS)
                    printf("Error\n");

                // Construct the response as a requestResponse object
                std::get<0>(response) = OK;
                bodyString = "Success";
                data.assign(bodyString.begin(), bodyString.end());
                std::get<1>(response) = data;
                std::get<2>(response) = TEXT;
            }
        }
        return response;
    };

    // Create the ControlEndpointPair and add it to the vector
    endpoints.emplace_back("mediaUploadService", handler);

    return endpoints;
}

eErrorCode ModuleInterface::start()
{
    std::vector<std::string> services = serHandler->getServices("firstService", 5);
    for (std::string service : services)
    {
        std::cout << "Service discovered in Module: " << service << std::endl;
        break;
    }
    eErrorCode code;
    std::string ServiceName = "";
    std::string ServiceType = "";

    int channelId = NULL;
    std::string inpData = "site-media";
    size_t size = 5000;
    sServiceData serviceData;
    serviceData.serviceName = ServiceName;
    serviceData.serviceType = ServiceType;
    eBusType bus = MQTT_BUS;
    std::string outputDataType = "media-data";
    // for the media-config topic
    code = serHandler->sendReadInputCommand(serviceData, "media-config", channelId, size, NULL, eCommand::START); // start recieving data
    if (code != eErrorCode::SUCCESS && code == eErrorCode::SERVICE_NOT_FOUND)
        printf("Error: Service is not Up\n");

    // for the site-media topic
    code = serHandler->sendReadInputCommand(serviceData, inpData, channelId, size, NULL, eCommand::START); // start recieving data
    if (code != eErrorCode::SUCCESS && code == eErrorCode::SERVICE_NOT_FOUND)
        printf("Error: Service is not Up\n");

    // To check if the config file is empty or not
    MediaUploadService *retrievedMediaService = static_cast<MediaUploadService *>(moduleInternalClass);

    std::string configFilePath = "../configFiles/config.json";

    nlohmann::json configData = retrievedMediaService->readConfigFile(configFilePath);

    if (!configData.empty())
    {
        // std::cout << "the file is not empty" << std::endl;
        //  Extract the necessary fields
        std::string url = configData.value("cloudURL", "");
        std::string username = configData.value("username", "");
        std::string password = configData.value("password", "");
        int flag = retrievedMediaService->updateFlag();

        eErrorCode code1 = serHandler->setOutputMessageBus(bus, outputDataType, username, password, url);
    }

    // eErrorCode code1 = serHandler->setOutputMessageBus(bus, outputDataType);

    // Make a reequest from here

    return eErrorCode::SUCCESS;
}
