#include "media_upload_service.hpp"
#include <iostream>
#include <bitset>
#include <vector>
#include <string>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
// #include "../../../Logger/logger/logger_api.hpp"

// #include "transactionLogs.hpp"
using json = nlohmann::json;
// using namespace LOGGER;

// using namespace tLogs;

void MediaUploadService::init(ServiceHandler *Handler, const std::string &configDirPath)
{
    serHandler = Handler;

    std::string logfilepath = configDirPath + "log/log.txt";

    // std::cout << "logfilepath " << logfilepath << std::endl;
    //  Create a logger object
    // module_plogger = LoggerCreator::get_logger(LOGGER_TYPE_FILE, DEBUG, logfilepath.c_str());
    //   tlogs=new transactionLogs();
    //   tlogs->createTransactionLog(10);

    ALOG(pLogger, DEBUG, "inside init");
}

int MediaUploadService::getFlag()
{
    return flag;
}

// update flag to 1
int MediaUploadService::updateFlag()
{
    flag = 1;
    return flag;
}

// Read config file
nlohmann::json MediaUploadService::readConfigFile(const std::string &configFilePath)
{
    std::ifstream file(configFilePath);

    // Check if the file is open
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open the file " << configFilePath << std::endl;
        return nlohmann::json();
    }

    // Check if the file is empty
    if (file.peek() == std::ifstream::traits_type::eof())
    {
        // Return an empty JSON object
        return nlohmann::json();
    }

    // Read and parse the JSON content
    try
    {
        nlohmann::json configData;
        file >> configData;
        // Return the parsed JSON content
        return configData;
    }
    catch (nlohmann::json::parse_error &e)
    {
        std::cerr << "Error: Failed to parse the JSON file " << configFilePath << ": " << e.what() << std::endl;
        // Return an empty JSON object
        return nlohmann::json();
    }
}

// function to encode image
int MediaUploadService::encodeimage(const nlohmann::json &data, const nlohmann::json &media)
{

    ALOG(pLogger, DEBUG, "inside encodeimage function");

    nlohmann::json metadata;
    // get the siteID,id,messageType,name,type
    metadata["siteId"] = data["siteId"];
    metadata["id"] = data["id"];
    metadata["messageType"] = data["messageType"];
    std::string mediaPath = media["path"];
    metadata["name"] = media["name"];
    metadata["type"] = media["type"];
    if (mediaPath != "")
    {
        cv::Mat image = cv::imread(mediaPath, cv::IMREAD_COLOR);

        if (image.empty())
        {
            std::cerr << "Error: Could not read the image at path: " << mediaPath << std::endl;
            return 0;
        }

        std::vector<uchar> jpeg_data;

        cv::imencode(".jpg", image, jpeg_data);
        size_t size = jpeg_data.size();

        // Assign the encoded data to the 'imageData' field in your JSON object
        metadata["imageData"] = jpeg_data;

        std::string metadataString = metadata.dump();

        // Calculate total size
        size_t totalSize = metadataString.size();

        // Create a buffer to hold the combined payload (metadata + image data)
        uint8_t *combinedBuffer = new uint8_t[totalSize];

        // Copy metadata string to the combined buffer
        std::copy_n(metadataString.begin(), metadataString.size(), combinedBuffer);

        eErrorCode s = serHandler->writeOutputData("media-data", NULL, combinedBuffer, totalSize);
        delete[] combinedBuffer;

        std::flush(std::cout);
        // store in database if it is not published
        //  if (s!=eErrorCode::SUCCESS)
        //  {
        //      uint8_t* p = reinterpret_cast< uint8_t*>(mediaPath.data());
        //      eErrorCode w=tlogs->writeTransactionLog("media-data","ab",p,mediaPath.length());
        //  }
    }

    return 1;
}

// function to encode video
int MediaUploadService::encodevideo(const nlohmann::json &data, const nlohmann::json &media)
{

    // ALOG(module_plogger, DEBUG, "inside encodevideo function");
    nlohmann::json metadata;
    // get the siteID,id,messageType,name,type
    metadata["siteId"] = data["siteId"];
    metadata["id"] = data["id"];
    metadata["messageType"] = data["messageType"];
    std::string mediaPath = media["path"];
    metadata["name"] = media["name"];
    metadata["type"] = media["type"];
    bool fileExists = std::filesystem::exists(mediaPath);

    usleep(80000);
    fileExists = std::filesystem::exists(mediaPath);

    if (mediaPath != "")
    { // OpenCV video capture
        cv::VideoCapture cap(mediaPath);
        if (!cap.isOpened())
        {
            std::cerr << "Error opening video file." << std::endl;
            return 0;
        }

        // Read all frames into a vector
        std::vector<uchar> encoded_frames;
        cv::Mat frame;
        while (cap.read(frame))
        {
            // Encode the frame
            std::vector<uchar> buffer;
            cv::imencode(".jpg", frame, buffer);
            // Append encoded frame to the vector
            encoded_frames.insert(encoded_frames.end(), buffer.begin(), buffer.end());
        }
        // Assign the encoded data to the 'videoData' field in your JSON object
        metadata["videoData"] = encoded_frames;

        std::string metadataString = metadata.dump();

        // Calculate total size
        size_t totalSize = metadataString.size();

        // Create a buffer to hold the combined payload (metadata + image data)
        uint8_t *combinedBuffer = new uint8_t[totalSize];

        // Copy metadata string to the combined buffer
        std::copy_n(metadataString.begin(), metadataString.size(), combinedBuffer);
        eErrorCode s = serHandler->writeOutputData("media-data", NULL, combinedBuffer, totalSize);
    }
    return 1;
}

int MediaUploadService::encodesnapshot(const nlohmann::json &data)
{
    // ALOG(pLogger, DEBUG, "inside encodesnapsort function");

    nlohmann::json metadata = data;
    std::cout << "received";

    std::string mediaPath = metadata["snapURL"];

    // Remove the key "snapURL" from metadata if it exists
    metadata.erase("snapURL");

    if (mediaPath != "")
    {
        cv::Mat image = cv::imread(mediaPath, cv::IMREAD_COLOR);

        if (image.empty())
        {
            std::cerr << "Error: Could not read the image at path: " << mediaPath << std::endl;
            return 0;
        }

        std::vector<uchar> jpeg_data;

        cv::imencode(".jpg", image, jpeg_data);
        size_t size = jpeg_data.size();

        // Assign the encoded data to the 'imageData' field in your JSON object
        metadata["imageData"] = jpeg_data;

        std::string metadataString = metadata.dump();

        // Calculate total size
        size_t totalSize = metadataString.size();

        // Create a buffer to hold the combined payload (metadata + image data)
        uint8_t *combinedBuffer = new uint8_t[totalSize];

        // Copy metadata string to the combined buffer
        std::copy_n(metadataString.begin(), metadataString.size(), combinedBuffer);

        eErrorCode s = serHandler->writeOutputData("media-data", NULL, combinedBuffer, totalSize);
        delete[] combinedBuffer;

        std::flush(std::cout);
        // store in database if it is not published
        //  if (s!=eErrorCode::SUCCESS)
        //  {
        //      uint8_t* p = reinterpret_cast< uint8_t*>(mediaPath.data());
        //      eErrorCode w=tlogs->writeTransactionLog("media-data","ab",p,mediaPath.length());
        //  }
    }

    return 1;
}

eErrorCode MediaUploadService::processMedia(const std::string inputDataName, uint8_t *inputData, size_t *size)
{

    try
    {
        // std::cout << "Inside process" << std::endl;
        ALOG(pLogger, DEBUG, "inside processMedia function");
        std::cout << std::flush;

        std::string result;
        result.reserve(*size);

        for (size_t i = 0; i < *size; ++i)
        {
            result.push_back(static_cast<char>(inputData[i]));
        }

        // Parse the JSON string
        json data = json::parse(result);
        std::cout << std::flush;

        if (data.contains("msgName"))
        {
            std::cout << std::flush;
            std::cout << "Received snapshot" << std::flush << std::endl;
            int status = encodesnapshot(data);
            std::cout << std::flush;
            return eErrorCode::SUCCESS;
        }

        // Extract paths and call the function
        for (const auto &media : data["mediaData"])
        {

            if (media["type"] == "video")
            {
                // int status=encodevideo(data,media);
                // if(status==0)
                //     return eErrorCode::FAILURE;
                continue;
            }

            else
            {
                int status = encodeimage(data, media);
                if (status == 0)
                    return eErrorCode::FAILURE;
            }
        }
    }
    catch (const std::exception &e)
    {

        std::cerr << "Exception caught: " << e.what() << std::endl;

        ALOG(pLogger, ERROR, "Error encoding data");
    }
    return eErrorCode::SUCCESS;
}

// Write in json file
int MediaUploadService::writeInJsonFile(std::string &jsonData)
{

    try
    {
        nlohmann::json jsonDataObject = nlohmann::json::parse(jsonData);

        std::string configFilePath = "../configFiles/config.json";
        std::ofstream configFile(configFilePath);
        if (!configFile.is_open())
        {
            std::cout << "Error: Could not open the file " << configFilePath << std::endl;
            return 0;
        }
        // Write the JSON
        configFile << jsonDataObject.dump(4);
        configFile.close();

        std::cout << "Config data written to " << configFilePath << std::endl;
        return 1;
    }
    catch (nlohmann::json::parse_error &e)
    {
        std::cout << "Error: Failed to parse JSON data: " << e.what() << std::endl;
        return 0;
    }
}