#include "ESPWifiAssist.h"
#include "../index_html.h"

ESPWifiAssist::ESPWifiAssist(String _apSsid, String _apPassword)
{
    // Constructor implementation
    WiFi.mode(WIFI_AP_STA);
    initFs();
    apSsid = _apSsid;
    apPassword = _apPassword;
    webServer = new ESP8266WebServer(80); // Assign the server to the member variable
    initWebServer();
}

ESPWifiAssist::ESPWifiAssist(String _apSsid, String _apPassword, ESP8266WebServer& _server) : webServer(&_server)
{
    // Constructor implementation
    WiFi.mode(WIFI_AP_STA);
    initFs();
    apSsid = _apSsid;
    apPassword = _apPassword;
    initWebServer();
}

void ESPWifiAssist::beginWifi(){
    if(isFileExists("/config/wifi-config.json")){
        // read the JSON file
        DynamicJsonDocument doc = readJsonFromFile("/config/wifi-config.json");

        // set saved ssid
        String value = doc["ssid"].as<String>();
        strncpy(ssid, value.c_str(), sizeof(ssid) - 1);
        ssid[sizeof(ssid) - 1] = '\0';  // Ensure null termination

        // set saved password
        value = doc["password"].as<String>();
        strncpy(password, value.c_str(), sizeof(password) - 1);
        password[sizeof(password) - 1] = '\0';

        if(ssid == "" || password == ""){
            startAp();            
            return;
        }
        connectToWiFi();
    } else {
        startAp();        
    }
}

void ESPWifiAssist::connectToWiFi()
{
    if(ssid == "" || ssid == NULL || password == "" || password == NULL){
        return;
    }
    wifiConnecting = true;
    connectionStartTime = millis(); // Store the connection start time
    WiFi.begin(ssid, password); // Connect to the WiFi network    
    WiFi.setAutoReconnect(true);
}

void ESPWifiAssist::startAp()
{
    WiFi.softAP(apSsid, apPassword); // Start the Access Point with SSID and password
    Serial.println("Access Point started");
    Serial.println("Connect to the Access Point:");
    Serial.println("SSID: " + apSsid);
    Serial.println("Password: " + apPassword);
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
}

void ESPWifiAssist::monitorWifiConnection(){
    webServer->handleClient();
    dnsServer.processNextRequest();
    if(!wifiConnecting) return;
    
    if(WiFi.status() == WL_CONNECT_FAILED || millis() - connectionStartTime > 30000){
        wifiConnecting = false;
        delay(3000);
        deleteFile("/config/wifi-config.json"); // Delete the config file if connection fails
        beginWifi(); // Start the Access Point again
    } else if(WiFi.status() == WL_CONNECTED){
        wifiConnecting = false;
        WiFi.softAPdisconnect(true); // Disconnect from the Access Point
        dnsServer.stop(); // Stop the DNS server
        WiFi.setHostname(apSsid.c_str());
        Serial.println("Connected to WiFi");
        Serial.println("IP Address: " + WiFi.localIP().toString());
        delay(3000);
    }        
}

void ESPWifiAssist::initWebServer(){
    webServer->on("/wifi", HTTP_GET, [this](){        
        webServer->send_P(200, "text/html", (const char*)index_html, index_html_len);
    });

    webServer->on("/scan", HTTP_GET, [this](){
        int n = WiFi.scanNetworks();
        String scanData;
        const size_t arrSize = JSON_ARRAY_SIZE(1024);
        StaticJsonDocument<arrSize> arr;
        JsonArray aplist = arr.to<JsonArray>();
        if (n == 0) { 
            
        } else {
            for (int i = 0; i < n; ++i) {
            // Serial.println(WiFi.SSID(i)+"\t"+WiFi.encryptionType(i)+"\t"+WiFi.RSSI(i));
            const size_t CAPACITY = JSON_OBJECT_SIZE(5);
            StaticJsonDocument<CAPACITY> doc;
            JsonObject ap = doc.to<JsonObject>();
            ap["ssid"] = WiFi.SSID(i);
            if (WiFi.isConnected() && WiFi.SSID(i) == WiFi.SSID()){
                ap["status"] = "Connected";
            } else {
                ap["status"] = WiFi.encryptionType(i)==5?"Secured(WEP)":WiFi.encryptionType(i)==2?"Secured(WPA)":WiFi.encryptionType(i)==4?"Secured(WPA2)":WiFi.encryptionType(i)==7?"Open":"Secured(WPA2/WPA Auto)";
            }
            
            ap["signal"] = WiFi.RSSI(i);
            aplist.add(ap);
            }
            serializeJson(aplist, scanData);
            webServer->send(200, "application/json", scanData);
        }
    });

    webServer->on("/connect", HTTP_POST, [this](){
        String postData = webServer->arg("plain");
        DynamicJsonDocument doc(1000);
        DeserializationError error = deserializeJson(doc, postData);
        if (error) {
            Serial.println(F("ERROR: deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        } else {
            const char* ssid = doc["ssid"];
            const char* password = doc["password"];
            setWifiCredentials(ssid, password);
            connectToWiFi();
        }
        webServer->send(200, "text/plain", "");
    });

    webServer->on("/generate_204", HTTP_GET, [this](){
        webServer->sendHeader("Location", "/wifi", true);
        webServer->send(302, "text/plain", "");
    });
    
    webServer->on("/redirect", HTTP_GET, [this](){
        webServer->sendHeader("Location", "/wifi", true);
        webServer->send(302, "text/plain", "");
    });
    
    webServer->on("/hotspot-detect.html", HTTP_GET, [this](){
        webServer->sendHeader("Location", "/wifi", true);
        webServer->send(302, "text/plain", "");
    });

    webServer->onNotFound([this](){
        webServer->sendHeader("Location", "/wifi", true);
        webServer->send(302, "text/plain", "");
    });
    
    webServer->begin();
}

void ESPWifiAssist::setWifiCredentials(const char *inputSsid, const char *inputPassword)
{
    if (inputSsid == NULL || inputPassword == NULL)
    {
        // Serial.println("SSID or password is null");
        return;
    }
    // Serial.println("Setting WiFi credentials: ssid: " + String(inputSsid) + ", password: " + String(inputPassword));
    strncpy(ssid, inputSsid, sizeof(ssid) - 1);
    ssid[sizeof(ssid) - 1] = '\0';  // Ensure null termination
    strncpy(password, inputPassword, sizeof(password) - 1);
    password[sizeof(password) - 1] = '\0'; // Ensure null termination
    // Serial.println("Writing to wifi config file");
    DynamicJsonDocument doc(1024);
    doc["ssid"] = ssid;
    doc["password"] = password;
    writeJsonToFile("/config/wifi-config.json", doc);
}

bool ESPWifiAssist::isConnected(){
  return WiFi.status() == WL_CONNECTED;
}