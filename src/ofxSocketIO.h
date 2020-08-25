//
//  ofxSocketIO.h
//
//  Created by hugohil on 05/02/16.
//
//
#pragma once

#include "ofMain.h"

#if defined(_MSC_VER) || defined(_WIN32) || defined(WIN32) || defined(__MINGW32__)
  // Windows stuff
#else
  // Linux and OSX here
  #include <sio_client.h>
#endif
#include "ofxSocketIOUtilities.h"

class ofxSocketIO : protected ofThread {
private :
  sio::client client;
  std::string currentStatus = "closed";

  void onConnect();
  void onClose(const sio::client::close_reason& reason);
  void onFail();
  void onTryReconnect();
  void onExit(ofEventArgs &);
public :
  std::string getStatus();

  void setup(const std::string& address, const std::map<std::string, std::string>& query = {});

  void bindEvent(ofEvent<const ofJson &>& event, const std::string& eventName, const std::string& namespace_ = "");
  void on(const std::string &eventName,
          std::function<void(const ofJson &json)> callback,
          const std::string &namespace_ = "");
  ofEvent<void> connectionEvent;
  ofEvent<std::string> notifyEvent;

  void emit(const std::string& eventName, const ofJson &data = {}, const std::string &namespace_ = "");
  void emitBinary(const std::string& eventName, std::shared_ptr<std::string> binaryStr, const std::string& namespace_ = "");
  
  void closeConnection();
  void openConnection(const std::string& address);
};
