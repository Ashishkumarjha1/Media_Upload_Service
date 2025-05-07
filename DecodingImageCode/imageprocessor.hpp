#ifndef IMAGEPROCESSOR_HPP
#define IMAGEPROCESSOR_HPP

#include <mqtt/async_client.h>
#include <opencv2/opencv.hpp>
#include<random>
#include<tuple>
#include<filesystem>
#include <nlohmann/json.hpp>

class ImageProcessor {
public:
    std::tuple<std::string, std::string, std::string,std::string, unsigned long int>  decode_message(uint8_t *receivedBuffer,std::string path);
};

#endif // IMAGEPROCESSOR_HPP
