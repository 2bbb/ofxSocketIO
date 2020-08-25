//
//  ofxSocketIO.cpp
//
//  Created by hugohil on 05/02/16.
//
//
#include "ofxSocketIO.h"

void ofxSocketIO::setup (const std::string &address, const std::map<std::string,std::string> &query) {
  currentStatus = "not connected";

  client.set_open_listener(std::bind(&ofxSocketIO::onConnect, this));
  client.set_close_listener(std::bind(&ofxSocketIO::onClose, this, std::placeholders::_1));
  client.set_fail_listener(std::bind(&ofxSocketIO::onFail, this));
  client.set_reconnect_listener(std::bind(&ofxSocketIO::onTryReconnect, this));

  client.connect(address, query);
  
  ofAddListener(ofEvents().exit, this, &ofxSocketIO::onExit);
}

void ofxSocketIO::onExit(ofEventArgs &) {
  closeConnection();
}

void ofxSocketIO::onConnect () {
  currentStatus = "connected";
  ofNotifyEvent(notifyEvent, currentStatus);
  ofNotifyEvent(connectionEvent);
}

void ofxSocketIO::onClose (sio::client::close_reason const& reason) {
  currentStatus = "closed";
  ofNotifyEvent(notifyEvent, currentStatus);
}

void ofxSocketIO::onFail () {
  currentStatus = "errored";
  ofNotifyEvent(notifyEvent, currentStatus);
}

void ofxSocketIO::onTryReconnect () {
  currentStatus = "reconnecting";
  ofNotifyEvent(notifyEvent, currentStatus);
}

string ofxSocketIO::getStatus() {
  return currentStatus;
}

void ofxSocketIO::bindEvent(ofEvent<const ofJson &>& event,
                            const std::string& eventName,
                            const std::string& nsp)
{
  auto callback = [&](const std::string &name,
                      const sio::message::ptr &data,
                      bool isAck,
                      sio::message::list &ack_resp)
  {
    ofJson json;
    if(data) json = toJSON(data);
    ofNotifyEvent(event, json, this);
  };
  auto listener = sio::socket::event_listener_aux(callback);
  client.socket(nsp)->on(eventName, listener);
}

void ofxSocketIO::on(const std::string &eventName,
                     std::function<void(const ofJson &json)> callback,
                     const std::string &nsp)
{
  auto cb = [=](const std::string &name,
                const sio::message::ptr &data,
                bool isAck,
                sio::message::list &ack_resp)
  {
    ofJson json;
    if(data) json = toJSON(data);
    callback(json);
  };
  auto listener = sio::socket::event_listener_aux(cb);
  if(nsp == "") client.socket()->on(eventName, listener);
  else client.socket(nsp)->on(eventName, listener);
}

void ofxSocketIO::emit (const std::string& eventName, const ofJson &data, const std::string &nsp) {
  if(client.opened()) {
    sio::message::list list = sio::message::list();
    list.push(toSIOMessage(data));
    client.socket(nsp)->emit(eventName, list);
  } else {
    ofLogWarning("ofxSocketIO", "socket is not available.");
  }
}

void ofxSocketIO::emitBinary (const std::string& eventName, std::shared_ptr<std::string> bStr, const std::string& nsp) {
  if (client.opened()) {
    client.socket(nsp)->emit(eventName, bStr);
  } else {
    ofLogWarning("ofxSocketIO", "socket is not available.");
  }
}

void ofxSocketIO::closeConnection () {
  currentStatus = "closed";
  if(client.opened()) client.sync_close();
}

void ofxSocketIO::openConnection (const std::string &address) {
    if(!client.opened()) client.connect(address);
}
