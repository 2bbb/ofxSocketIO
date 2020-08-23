//
//  ofxSocketIO.h
//
//  Created by hugohil on 05/02/16.
//
//
#pragma once

#include "ofMain.h"
#include "ofEvents.h"
#if defined(_MSC_VER) || defined(_WIN32) || defined(WIN32) || defined(__MINGW32__)
  // Windows stuff
#else
  // Linux and OSX here
  #include <sio_client.h>
#endif
#include "ofxSocketIOData.h"

class ofxSocketIO : protected ofThread {
private :
  sio::client client;
  std::string currentStatus = "closed";

  void onConnect();
  void onClose(const sio::client::close_reason& reason);
  void onFail();
  void onTryReconnect();

public :
  std::string getStatus();

  void setup(const std::string& address);
  void setup(const std::string& address, const std::map<std::string,std::string>& query);

  void bindEvent(ofEvent<ofxSocketIOData&>& event, const std::string& eventName, const std::string& nsp="");

  ofEvent<void> connectionEvent;
  ofEvent<std::string> notifyEvent;

  void emit(const std::string& eventName);
  void emit(const std::string& eventName, const std::string& data, const std::string& nsp="");
  void emitBinary(const std::string& eventName, std::shared_ptr<std::string> bStr, const std::string& nsp="");

  void closeConnection();
  void openConnection(const std::string& address);
};
