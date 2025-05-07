#include "imageprocessor.hpp"
 
 
std::tuple<std::string, std::string, std::string,std::string, unsigned long int>  ImageProcessor::decode_message(uint8_t *receivedBuffer,std::string path ) {
    
    std::tuple<std::string, std::string, std::string,std::string, unsigned long int> resultTuple;
 
    try {
         //Get the siteid,messageid,type size
        int siteidSize = *reinterpret_cast<const int*>(receivedBuffer);
        int messageidSize = *reinterpret_cast<const int*>(receivedBuffer+sizeof(int));
        int typeSize = *reinterpret_cast<const int*>(receivedBuffer+sizeof(int)*2);
        int messagetypesize=*reinterpret_cast<const int*>(receivedBuffer+sizeof(int)*3);
        
        //Get the siteid data
        const char* siteidData = reinterpret_cast<const char*>(receivedBuffer + sizeof(int) * 4);
        std::string siteid(siteidData, siteidSize);
        
       //Get the messageid
        unsigned long int messageid = *reinterpret_cast<const unsigned long int*>(receivedBuffer+ sizeof(int) * 4+siteidSize);
     
        //Get the typeData
        const char* typeData = reinterpret_cast<const char*>(receivedBuffer + sizeof(int) *4 +siteidSize+messageidSize);
        std::string type(typeData, typeSize);
 
        //Get the messagetypedata
        const char* messagetypeData = reinterpret_cast<const char*>(receivedBuffer + sizeof(int) *4+messageidSize+siteidSize+typeSize);
        std::string messagetype(messagetypeData, messagetypesize);
 
        size_t size=*reinterpret_cast<const size_t*>(receivedBuffer + sizeof(int)*4+messageidSize+siteidSize+typeSize+messagetypesize);
     
        int meta_size = sizeof(int)*4+siteidSize+messageidSize+typeSize+sizeof(size)+messagetypesize;
 
        // Create a vector to hold the image data in uchar format
        std::vector<uchar> extractedJpegData(receivedBuffer+meta_size, receivedBuffer + meta_size+size);
 
        // Decode JPEG image data using OpenCV
        cv::Mat decodedImage = cv::imdecode(extractedJpegData, cv::IMREAD_COLOR);
 
        // Check if the image is successfully decoded
        if (decodedImage.empty()) {
            std::cerr << "Error decoding image." << std::endl;
            
        }
        //Create the fullpath
        std::string imageName = std::to_string(messageid)+"_" + type + "_.png";
        std::string fullPath = path +imageName;
        
        // Check if a file with the current fullPath already exists
        while (std::filesystem::exists(fullPath)) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 9999);
        int randomSuffix = dis(gen);
 
        // Create a new image name based on messageid, type, and the random suffix
        imageName = std::to_string(messageid)+"_" + type +std::to_string(randomSuffix)+ "_.jpg";
        // Update fullPath with the new path including the generated image name
        fullPath = path+imageName;
        }
        
        cv::imwrite(fullPath, decodedImage);
        
        std::cout << "Image successfully saved" << std::endl;
    
        // Assigning values to the tuple
        std::get<0>(resultTuple) = siteid;
        std::get<1>(resultTuple) = type;
        std::get<2>(resultTuple) = fullPath;
        std::get<3>(resultTuple)=messagetype;
        std::get<4>(resultTuple) = messageid;
    } catch (const std::exception& e) {
        std::cerr << "Error decoding image: " << e.what() << std::endl;
    }
    return resultTuple;
}