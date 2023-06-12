#include "log.h"
/*
 * wifi_info.h
 *
 *  Created on: 2020-05-15
 *      Author: Mixiaoxiao (Wang Bin)
 */

#ifndef WIFI_INFO_H_
#define WIFI_INFO_H_

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

const char *ssid = "TelstraD7F54B";
const char *password = "2w23aqev7m";

ESP8266WebServer server(80);

void wifi_connect()
{
	WiFi.persistent(false);
	WiFi.mode(WIFI_STA);
	WiFi.setAutoReconnect(true);
	WiFi.begin(ssid, password);
	Serial.println("WiFi connecting...");
	while (!WiFi.isConnected())
	{
		delay(100);
		Serial.print(".");
	}
	Serial.print("\n");
	Serial.printf("WiFi connected, IP: %s\n", WiFi.localIP().toString().c_str());
}

void handle_NotFound()
{
	server.send(404, "text/plain", "Not found");
}
void setup_server(TrivialCB up_fn, TrivialCB down_fn, TrivialCB set_fn)
{
	server.on("/up", up_fn);
	server.on("/down", down_fn);
	server.on("/set", set_fn);
	server.onNotFound(handle_NotFound);

	server.begin();
}

#endif /* WIFI_INFO_H_ */

String updateWebpage(bool standing)
{
	String ptr = "<!DOCTYPE html> <html>\n";
	ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
	ptr += "<title>LED Control</title>\n";
	ptr += "<style>html {font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
	ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
	ptr += ".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
	ptr += ".button-on {background-color: #3498db;}\n";
	ptr += ".button-on:active {background-color: #3498db;}\n";
	ptr += ".button-off {background-color: #34495e;}\n";
	ptr += ".button-off:active {background-color: #2c3e50;}\n";
	ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
	ptr += "</style>\n";
	ptr += "</head>\n";
	ptr += "<body>\n";
	ptr += "<h1>ESP8266 Web Server</h1>\n";
	ptr += "<h3>Using Station(STA) Mode</h3>\n";

	if (standing)
	{
		ptr += "<p>Desk: Standing</p><a class=\"button button-off\" href=\"/down\">Lower</a>\n";
	}
	else
	{
		ptr += "<p>Desk: Sitting</p><a class=\"button button-on\" href=\"/up\">Raise</a>\n";
	}

	ptr += "</body>\n";
	ptr += "</html>\n";
	return ptr;
}
