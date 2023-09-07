#ifndef MIKROSKOP_HTTPSERVER_H
#define MIKROSKOP_HTTPSERVER_H

#include <ESPAsyncWebServer.h>

class HTTPServer {
  static constexpr auto ROOT_HTML = "/index.html";
  static constexpr auto CSS_FILE  = "/style.css";

  AsyncWebServer m_server;

public:
  explicit HTTPServer(uint_fast16_t port);
  ~HTTPServer();

  HTTPServer(const HTTPServer&)            = delete;
  HTTPServer(HTTPServer&&)                 = delete;
  HTTPServer& operator=(const HTTPServer&) = delete;
  HTTPServer& operator=(HTTPServer&&)      = delete;

  void begin();
  void end();

private:
  static void onRoot(AsyncWebServerRequest* request);
  static void onCSS(AsyncWebServerRequest* request);
  static void onNotFound(AsyncWebServerRequest* request);
};

#endif // MIKROSKOP_HTTPSERVER_H
