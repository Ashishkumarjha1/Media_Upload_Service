Media Upload Service
This module is a component of a larger service responsible for handling the ingestion and encoding of image and video data from media files or snapshot requests. It reads configuration, processes media data, encodes it (typically to JPEG), packages it with metadata in JSON format, and passes it to an output handler ( for publishing over MQTT).

Key Features
Configuration Loader: Reads and parses JSON-based configuration files.


Image Encoding: Reads image files and encodes them into JPEG format, embedding them in JSON along with metadata.


Snapshot Processing: Extracts and processes snapshot images using a key (snapURL) from incoming JSON.

Data Publishing: Transmits encoded JSON payloads via a provided service handler.
