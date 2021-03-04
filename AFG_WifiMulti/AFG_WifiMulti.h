#ifndef AFG_WIFIMULTI_H_INCLUDED
#define AFG_WIFIMULTI_H_INCLUDED

#include "ESP8266WiFi.h"
#include "ESP8266mDNS.h"
#include <vector>

#ifdef DEBUG_ESP_WIFI
#ifdef DEBUG_ESP_PORT
#define DEBUG_WIFI_MULTI(fmt, ...) DEBUG_ESP_PORT.printf_P( (PGM_P)PSTR(fmt), ##__VA_ARGS__ )
#endif
#endif

#ifndef DEBUG_WIFI_MULTI
#define DEBUG_WIFI_MULTI(...) do { (void)0; } while (0)
#endif

struct WifiAPEntry {
    char * ssid;
    char * password;
};

typedef std::vector<WifiAPEntry> WifiAPlist;

class AFG_WifiMulti {
    public:
		AFG_WifiMulti();
        AFG_WifiMulti(IPAddress fixedIp, IPAddress gateway, IPAddress subnet, IPAddress primaryDNS, IPAddress secondaryDNS);
        ~AFG_WifiMulti();

        bool addAP(const char* ssid, const char *password = NULL);
        bool existsAP(const char* ssid, const char *password = NULL);

        wl_status_t run(void);

        void cleanAPlist(void);
		void setMulticastDNS(String name);
		
		String SSID();
		IPAddress localIP();
    private:
		/*IPAddress fixedIp;
		IPAddress gateway;
		IPAddress subnet;
		IPAddress primaryDNS;
		IPAddress secondaryDNS;*/
        WifiAPlist APlist;
        bool APlistAdd(const char* ssid, const char *password = NULL);
        bool APlistExists(const char* ssid, const char *password = NULL);
        void APlistClean(void);
};

#endif