#include <iostream>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
// #include "./DwellTime.cpp"
#include <sstream>
class AlarmEventProcessor{
std::string alarmData = R"__({ 

"AssociatedEventUIDs" : [], 

"CameraID" : 39, 

"EventImagePath" : "RC9L_sanitycheck/2023-08-29/17_49_23_686.jpg", 

"EventSpecificData" :  

[ 

{ 

"key" : "Description", 

"value" : "(B12345,,,Entry,,1.00,UAE-Ras Al Khaimah,Private,NA on NA)" 

} 

], 

"EventTime" : "2023-08-29T12:19:23.686Z", 

"EventType" : 25, 

"EventUID" : 169331156573531039, 

"EventVideoPath" : "", 

"InputFPS" : 29.960053262316912, 

"ObjectInfo" : [], 

"PersonInfo" : [], 

"RegionID" : 0, 

"RegionName" : "Region 1", 

"VMSCameraID" : 1, 

"VMSID" : 1, 

"VehicleInfo" :  

[ 

{ 

"BoundingBox" :  

{ 

"Height" : 0.23240740597248077, 

"Left" : 0.59843748807907104, 

"Top" : 0.57685184478759766, 

"Width" : 0.14739583432674408 

}, 

"Class" : "Vehicle", 

"Color" : "NA", 

"Confidence" : "0.875", 

"CroppedImagePath" : "", 

"LPRInfo" :  

{ 

"BoundingBox" :  

{ 

"Height" : 0.23240740597248077, 

"Left" : 0.59843748807907104, 

"Top" : 0.57685184478759766, 

"Width" : 0.14739583432674408 

}, 

"Category" : "", 

"Country" : "UAE-Ras Al Khaimah", 

"DBId" : -1, 

"LPRNumber" : "B12345", 

"MatchConfidence" : 1.0, 

"PlateColor" : "NA", 

"PlateCropImagePath" : "RC9L_sanitycheck/2023-08-29/17_49_23_686_0977_crop.jpg", 

"PlateTextColor" : "NA", 

"PlateType" : "Private", 

"State" : "" 

}, 

"Speed" : "", 

"SubClass" : 0, 

"Trajectory" : [], 

"UID" : 0 

} 

] 

} )__";
};