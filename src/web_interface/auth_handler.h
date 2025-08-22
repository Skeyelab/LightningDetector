#ifndef WEB_INTERFACE_AUTH_HANDLER_H
#define WEB_INTERFACE_AUTH_HANDLER_H

#include <Arduino.h>
#include <WebServer.h>

class AuthHandler {
public:
    // Always returns true for now; placeholder for future auth logic.
    static bool isAuthorized(WebServer &server) {
        // TODO: implement authentication checks
        (void)server;
        return true;
    }
};

#endif // WEB_INTERFACE_AUTH_HANDLER_H