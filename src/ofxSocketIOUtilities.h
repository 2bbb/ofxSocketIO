//
//  ofxSocketIOUtilities.h
//
//  Created by 2bit on 2020/08/24.
//

#ifndef ofxSocketIOUtilities_h
#define ofxSocketIOUtilities_h

#include "ofMain.h"

#if defined(_MSC_VER) || defined(_WIN32) || defined(WIN32) || defined(__MINGW32__)
  // Windows stuff
#else
  // Linux and OSX here
  #include <sio_client.h>
#endif

ofJson toJSON(sio::message::ptr message);
sio::message::ptr toSIOMessage(const ofJson &data);

#endif /* ofxSocketIOUtilities_h */
