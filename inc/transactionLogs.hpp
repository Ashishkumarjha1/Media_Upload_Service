#include <iostream>
#include <string>
#include "types.hpp"
#include <mysql_driver.h>
#include <mysql_connection.h>

using namespace SFTypes;

namespace tLogs{

struct transactionData
{
    uint8_t* data;
    size_t size;
};

class transactionLogs
{
    private:
        sql::mysql::MySQL_Driver *driver;
        sql::Connection *con;

    public:
        /**
         * @brief will create a connection with transaction log database
         * 
        */
        eErrorCode createTransactionLog(int time);//pass the time till which hold the logs
        /**
         * @brief read from the transaction log database
         * 
         * @param [in] type : topic associated with the data that needs to be read
         * @param [in] source : service name associated with the data that needs to be stored
         * 
         * @return  pointer to unsigned char buffer
        */
        transactionData readTransactionLog(std::string type,std::string source);

        /**
         * @brief read from the transaction log database
         * 
         * @param [in] type : topic associated with the data that needs to be stored
         * @param [in] source : service name associated with the data that needs to be stored
         * @param [in] data : data to be stored in the transaction log
         * @param [in] dataLength : length of the data to be stored in the transaction log
         * @return  error code
        */
        eErrorCode writeTransactionLog(std::string type,std::string source,uint8_t* data,int dataLength); 

        eErrorCode deleteTransactionLog(std::string type,int id);   

        int countTransactionLog(std::string tableName); 
};
}