//holds the current upload
File fsUploadFile;

//format bytes
String formatBytes(size_t bytes){
  if (bytes < 1024){
    return String(bytes)+"B";
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0)+"KB";
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0)+"MB";
  } else {
    return String(bytes/1024.0/1024.0/1024.0)+"GB";
  }
}

String getContentType(String filename){
  if(webServer.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path){
  LOG_DEBUG("handleFileRead:", path);
  if(path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(LittleFS.exists(pathWithGz) || LittleFS.exists(path)){
    if(LittleFS.exists(pathWithGz))
      path += ".gz";
    File file = LittleFS.open(path, "r");
    size_t contentLength = webServer.streamFile(file, contentType);
    file.close();
    LOG_DEBUG("Send contentLength:", contentLength);
    return true;
  }
  LOG_DEBUG("File not found:", pathWithGz);
  return false;
}

void handleFileUpload(){
#ifndef FILE_WRITE
#define FILE_WRITE "w"
#endif
  if(webServer.uri() != "/edit") return;
  HTTPUpload& upload = webServer.upload();
  String filename = upload.filename;
  if(!filename.startsWith("/")) {
    filename = "/"+filename;
  }
  if(upload.status == UPLOAD_FILE_START){
    LOG_DEBUG("handleFileUpload Name:", filename);
    fsUploadFile = LittleFS.open(filename, FILE_WRITE);
    if(!fsUploadFile){
      LOG_ERROR("- failed to open file for writing:", filename);
      return;
    }
  } else if(upload.status == UPLOAD_FILE_WRITE){
    LOG_DEBUG("handleFileUpload Data:", upload.currentSize);
    if (!fsUploadFile.write(upload.buf, upload.currentSize)) {
        LOG_ERROR("- write failed");
    }
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile)
      fsUploadFile.close();
    LOG_DEBUG("handleFileUpload Size:", upload.totalSize);
  }
}

void handleFileDelete(){
  if(webServer.args() == 0) return webServer.send(500, "text/plain", "BAD ARGS");
  String path = webServer.arg(0);
  LOG_DEBUG("handleFileDelete:", path);
  if(path == "/")
    return webServer.send(500, "text/plain", "BAD PATH");
  if(!LittleFS.exists(path))
    return webServer.send(404, "text/plain", "FileNotFound");
  LittleFS.remove(path);
  webServer.send(200, "text/plain", "");
  path = String();
}

void handleFileCreate(){
  if(webServer.args() == 0)
    return webServer.send(500, "text/plain", "BAD ARGS");
  String path = webServer.arg(0);
  LOG_DEBUG("handleFileCreate:", path);
  if(path == "/")
    return webServer.send(500, "text/plain", "BAD PATH");
  if(LittleFS.exists(path))
    return webServer.send(500, "text/plain", "FILE EXISTS");
  File file = LittleFS.open(path, "w");
  if(file)
    file.close();
  else
    return webServer.send(500, "text/plain", "CREATE FAILED");
  webServer.send(200, "text/plain", "");
  path = String();
}

#if defined(ESP8266)
void handleFileList() {
  if(!webServer.hasArg("dir")) {webServer.send(500, "text/plain", "BAD ARGS"); return;}
  
  String path = webServer.arg("dir");
  LOG_DEBUG("handleFileList: ", path);
  Dir dir = LittleFS.openDir(path);
  path = String();

  String output = "[";
  while(dir.next()){
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir)?"dir":"file";
    output += "\",\"name\":\"";
    output += String(entry.name());
    output += "\"}";
    entry.close();
  }
  
  output += "]";
  webServer.send(200, "text/json", output);
}
#elif defined(ESP32)
String listDir(fs::FS &fs, String dirname, uint8_t levels) {
  LOG_DEBUG("Listing directory:", dirname);
  File root = fs.open(dirname);
  String output = "[";
  File file = root.openNextFile();
  while (file) {
    if (output != "[") output += ',';
    output += "{\"type\":\"";
    if (file.isDirectory()) {
      output += "dir";
      output += "\",\"name\":\"";
      output += String(file.name());
      output += "\",\"content\":";
      LOG_DEBUG("DIR:", file.name());
      if (levels) {
        output += listDir(fs, file.path(), levels - 1);
      }
    } else {
      output += "file";
      output += "\",\"name\":\"";
      output += String(file.path());
      LOG_DEBUG("FILE:", file.path(), "SIZE:", file.size());
      output += "\"";
    }
    output += "}";
    file = root.openNextFile();
  }
  output += "]";
  return output;
}
void handleFileList() {
  if(!webServer.hasArg("dir")) {webServer.send(500, "text/plain", "BAD ARGS"); return;}
  
  String path = webServer.arg("dir");
  File dir = LittleFS.open(path);
  LOG_DEBUG("handleFileList: ", path, "dir:", dir);
  if (!dir) {
    LOG_ERROR("- failed to open directory");
    webServer.send(500, "text/plain", "failed to open directory");
    return;
  }
  if (!dir.isDirectory()) {
    LOG_ERROR(" - not a directory");
    webServer.send(500, "text/plain", "not a directory");
    return;
  }
  String output = listDir(LittleFS, path, 1);

  webServer.send(200, "text/json", output);
}
#endif
