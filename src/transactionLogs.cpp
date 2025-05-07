#include "transactionLogs.hpp"
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <sstream>
#include <cstdint>
#include <cstring>

using namespace tLogs;

eErrorCode transactionLogs::createTransactionLog(int time)
{
    eErrorCode status= eErrorCode::FAILURE;
    driver = sql::mysql::get_mysql_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306","root","pass");
    con->setSchema("configDB");
    sql::Statement *stmt;
    sql::ResultSet *res;
    // std::string tableName = ";

    try {
        // stmt = con->createStatement();
        // res = stmt->executeQuery("SHOW TABLES LIKE 'ALARMS'");

        // if ( !res->next())
        // {
            stmt = con->createStatement();
        
            // SQL query to create a table
            std::string createTableQuery =  "CREATE TABLE IF NOT EXISTS `Alarms` (ID INT AUTO_INCREMENT PRIMARY KEY,Source VARCHAR(255),Type VARCHAR(255),Data VARBINARY(4000))";

            stmt->execute(createTableQuery);

            stmt = con->createStatement();
        
            // SQL query to create a table
            createTableQuery = "CREATE TABLE IF NOT EXISTS `configData` ("
                                "ID INT AUTO_INCREMENT PRIMARY KEY,"
                                "Source VARCHAR(255),"
                                "Type VARCHAR(255),"
                                "Data VARBINARY(4000)"
                            ")";

            stmt->execute(createTableQuery);
            
            std::cout << "Table created successfully." << std::endl;
        // } 
        status= eErrorCode::SUCCESS;
    } catch (sql::SQLException &e) {
        std::cout << "MySQL Exception: " << e.what() << std::endl;
        return eErrorCode::FAILURE;
    } catch (std::runtime_error &e) {
        std::cout << "Runtime Error: " << e.what() << std::endl;
        return eErrorCode::FAILURE;
    } 
        delete stmt;
        return status;
}

transactionData transactionLogs::readTransactionLog(std::string type, std::string source)
{
    transactionData log ;
    sql::Statement *stmt;
    sql::ResultSet *res;
    std::string order;
    uint8_t *data;
    std::string tableName;
    if (type == "live_alarms"){
        order ="DESC";
        tableName="Alarms";}
    else
    {
        order = "ASC";
        tableName = "configData";
    }

    try {
        stmt = con->createStatement();

        // SQL statement to select data from the table 
        std::string selectSQL = "SELECT * FROM "+ tableName +" ORDER BY ID "+order+" LIMIT 1" ; 
        res = stmt->executeQuery(selectSQL);

        if (res->next())
        {
            sql::SQLString varbinaryData = res->getString("Data");
            const char*  charData = varbinaryData.c_str();
            data= new uint8_t[strlen(charData)];
            data = reinterpret_cast<uint8_t*>(const_cast<char*>(charData));
            log.data=data;
            log.size= strlen(charData);

            // int deletedId = res->getInt("ID"); 
            // std::string deleteSQL = "DELETE FROM "+ tableName +" WHERE ID = " + std::to_string(deletedId);
            // stmt->executeUpdate(deleteSQL);
        }

    
    } 
    catch (sql::SQLException &e) {
        std::cout << "MySQL Exception: " << e.what() << std::endl;
        
    }
     return log;
}

eErrorCode transactionLogs::writeTransactionLog(std::string type, std::string source, uint8_t *data, int dataLength)
{
     sql::PreparedStatement *pstmt;
     eErrorCode status= eErrorCode::FAILURE;
     std::string table;
  

    try {
        if (type == "Live_Alarms") 
           table = "Alarms";
        else
            table = "configData";

        std::string insertSQL = "INSERT INTO "+table+" (Source, Type, Data) VALUES (?, ?, ?)";

        pstmt = con->prepareStatement(insertSQL);
        pstmt->setString(1, source);
        pstmt->setString(2, type);
        std::istringstream dataStream(std::string(reinterpret_cast<const char*>(data), dataLength));
        pstmt->setBlob(3, &dataStream);  // Use std::istream*

        // Execute the SQL statement to insert data
        pstmt->execute();
        
        

    } catch (sql::SQLException &e) {
        std::cout << "MySQL Exception: " << e.what() << std::endl;
    } catch (std::runtime_error &e) {
        std::cout << "Runtime Error: " << e.what() << std::endl;
    } 
        delete pstmt;
        return status;
}

int tLogs::transactionLogs::countTransactionLog(std::string tableName)
{
    sql::Statement *stmt;
    int rowCount =0;
    stmt = con->createStatement();

    sql::ResultSet *res;
    res = stmt->executeQuery("SELECT COUNT(*) AS row_count FROM "+tableName);


    if (res->next()) {
        rowCount = res->getInt("row_count");
        std::cout << "Number of rows in the table: " << rowCount << std::endl;
    }
    return rowCount;
}
