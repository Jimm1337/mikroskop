#include "HTTPServer.h"
#include <SPIFFS.h>
#include "Logger.h"

HTTPServer::HTTPServer(uint_fast16_t port): m_server(port) {
  m_server.on("/", HTTP_GET, &HTTPServer::onRoot);
  m_server.on("/style.css", HTTP_GET, &HTTPServer::onCSS);
  m_server.on("/script.js", HTTP_GET, &HTTPServer::onJS);
  m_server.onNotFound(&HTTPServer::onNotFound);

  begin();
}

HTTPServer::~HTTPServer() {
  end();
}

void HTTPServer::begin() {
  m_server.begin();
}

void HTTPServer::end() {
  m_server.end();
}

void HTTPServer::onRoot(AsyncWebServerRequest* request) {
  Logger::debug("HTTP GET request of " + request->url());
  request->send(SPIFFS, ROOT_HTML, "text/html");
}

void HTTPServer::onCSS(AsyncWebServerRequest* request) {
  Logger::debug("HTTP GET request of " + request->url());
  request->send(SPIFFS, CSS_FILE, "text/css");
}

void HTTPServer::onJS(AsyncWebServerRequest* request) {
    Logger::debug("HTTP GET request of " + request->url());
    request->send(SPIFFS, JS_FILE, "text/javascript");
}

void HTTPServer::onNotFound(AsyncWebServerRequest* request) {
  Logger::debug("HTTP GET request of " + request->url());
  request->send(404, "text/plain", "Not found");
}
