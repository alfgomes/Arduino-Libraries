#include "AFG_WifiMulti.h"
#include <limits.h>
#include <string.h>

AFG_WifiMulti::AFG_WifiMulti() {
}

AFG_WifiMulti::AFG_WifiMulti(IPAddress fixedIp, IPAddress gateway, IPAddress subnet, IPAddress primaryDNS, IPAddress secondaryDNS) {
	/*this->fixedIp = fixedIp;
	this->gateway = gateway;
	this->subnet = subnet;
	this->primaryDNS = primaryDNS;
	this->secondaryDNS = secondaryDNS;*/
	WiFi.config(fixedIp, gateway, subnet, primaryDNS, secondaryDNS);
}

AFG_WifiMulti::~AFG_WifiMulti() {
    APlistClean();
}

bool AFG_WifiMulti::addAP(const char* ssid, const char *password) {
    return APlistAdd(ssid, password);
}

void AFG_WifiMulti::cleanAPlist(void) {
    APlistClean();
}

void AFG_WifiMulti::setMulticastDNS(String name) {
	MDNS.begin(name);
}
		
String AFG_WifiMulti::SSID() {
	return WiFi.SSID();
}

IPAddress AFG_WifiMulti::localIP() {
	return WiFi.localIP();
}

bool AFG_WifiMulti::existsAP(const char* ssid, const char *password) {
    return APlistExists(ssid, password);
}

wl_status_t AFG_WifiMulti::run(void) {
    wl_status_t status = WiFi.status();
    if (status == WL_DISCONNECTED || status == WL_NO_SSID_AVAIL || status == WL_IDLE_STATUS || status == WL_CONNECT_FAILED) {
        int8_t scanResult = WiFi.scanComplete();

        if (scanResult == WIFI_SCAN_RUNNING) {
            // scan is running, do nothing yet
            status = WL_NO_SSID_AVAIL;
            return status;
        } 

        if (scanResult == 0) {
            // scan done, no ssids found. Start another scan.
            DEBUG_WIFI_MULTI("[WIFI] scan done\n");
            DEBUG_WIFI_MULTI("[WIFI] no networks found\n");
            WiFi.scanDelete();
            DEBUG_WIFI_MULTI("\n\n");
            delay(0);
            WiFi.disconnect();
            DEBUG_WIFI_MULTI("[WIFI] start scan\n");
            // scan wifi async mode
            WiFi.scanNetworks(true);
            return status;
        } 

        if (scanResult > 0) {
            // scan done, analyze
            WifiAPEntry bestNetwork { NULL, NULL };
            int bestNetworkDb = INT_MIN;
            uint8 bestBSSID[6];
            int32_t bestChannel;

            DEBUG_WIFI_MULTI("[WIFI] scan done\n");
            delay(0);

            DEBUG_WIFI_MULTI("[WIFI] %d networks found\n", scanResult);
            for (int8_t i = 0; i < scanResult; ++i) {
                String ssid_scan;
                int32_t rssi_scan;
                uint8_t sec_scan;
                uint8_t* BSSID_scan;
                int32_t chan_scan;
                bool hidden_scan;

                WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan, hidden_scan);

                bool known = false;
                for (auto entry : APlist) {
                    if (ssid_scan == entry.ssid) { // SSID match
                        known = true;
                        if (rssi_scan > bestNetworkDb) { // best network
                            if (sec_scan == ENC_TYPE_NONE || entry.password) { // check for password if not open wlan
                                bestNetworkDb = rssi_scan;
                                bestChannel = chan_scan;
                                bestNetwork = entry;
                                memcpy((void*) &bestBSSID, (void*) BSSID_scan, sizeof(bestBSSID));
                            }
                        }
                        break;
                    }
                }

                if (known) {
                    DEBUG_WIFI_MULTI(" ---> ");
                } else {
                    DEBUG_WIFI_MULTI("      ");
                }

                DEBUG_WIFI_MULTI(" %d: [%d][%02X:%02X:%02X:%02X:%02X:%02X] %s (%d) %c\n", i, chan_scan, BSSID_scan[0], BSSID_scan[1], BSSID_scan[2], BSSID_scan[3], BSSID_scan[4], BSSID_scan[5], ssid_scan.c_str(), rssi_scan, (sec_scan == ENC_TYPE_NONE) ? ' ' : '*');
                delay(0);
            }

            // clean up ram
            WiFi.scanDelete();

            DEBUG_WIFI_MULTI("\n\n");
            delay(0);

            if (bestNetwork.ssid) {
                DEBUG_WIFI_MULTI("[WIFI] Connecting BSSID: %02X:%02X:%02X:%02X:%02X:%02X SSID: %s Channel: %d (%d)\n", bestBSSID[0], bestBSSID[1], bestBSSID[2], bestBSSID[3], bestBSSID[4], bestBSSID[5], bestNetwork.ssid, bestChannel, bestNetworkDb);

                WiFi.begin(bestNetwork.ssid, bestNetwork.password, bestChannel, bestBSSID);
                status = WiFi.status();

                static const uint32_t connectTimeout = 5000; //5s timeout
                
                auto startTime = millis();
                // wait for connection, fail, or timeout
                while (status != WL_CONNECTED && status != WL_NO_SSID_AVAIL && status != WL_CONNECT_FAILED && (millis() - startTime) <= connectTimeout) {
                    delay(10);
                    status = WiFi.status();
                }
                
#ifdef DEBUG_ESP_WIFI
                IPAddress ip;
                uint8_t * mac;
                switch(status) {
                    case WL_CONNECTED:
                        ip = WiFi.localIP();
                        mac = WiFi.BSSID();
                        DEBUG_WIFI_MULTI("[WIFI] Connecting done.\n");
                        DEBUG_WIFI_MULTI("[WIFI] SSID: %s\n", WiFi.SSID().c_str());
                        DEBUG_WIFI_MULTI("[WIFI] IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
                        DEBUG_WIFI_MULTI("[WIFI] MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                        DEBUG_WIFI_MULTI("[WIFI] Channel: %d\n", WiFi.channel());
                        break;
                    case WL_NO_SSID_AVAIL:
                        DEBUG_WIFI_MULTI("[WIFI] Connecting Failed AP not found.\n");
                        break;
                    case WL_CONNECT_FAILED:
                        DEBUG_WIFI_MULTI("[WIFI] Connecting Failed.\n");
                        break;
                    default:
                        DEBUG_WIFI_MULTI("[WIFI] Connecting Failed (%d).\n", status);
                        break;
                }
#endif
            } else {
                DEBUG_WIFI_MULTI("[WIFI] no matching wifi found!\n");
            }
				
			MDNS.update();

            return status;
        }
		
        // scan failed, or some other condition not handled above. Start another scan.
        DEBUG_WIFI_MULTI("[WIFI] delete old wifi config...\n");
        WiFi.disconnect();
        DEBUG_WIFI_MULTI("[WIFI] start scan\n");
		
        // scan wifi async mode
        WiFi.scanNetworks(true);
    }
	
    return status;
}

// ##################################################################################

bool AFG_WifiMulti::APlistAdd(const char* ssid, const char *password) {
    WifiAPEntry newAP;

    if (!ssid || *ssid == 0x00 || strlen(ssid) > 32) {
        // fail SSID too long or missing!
        DEBUG_WIFI_MULTI("[WIFI][APlistAdd] no ssid or ssid too long\n");
        return false;
    }

    //for password, max is 63 ascii + null. For psk, 64hex + null.
    if (password && strlen(password) > 64) {
        // fail password too long!
        DEBUG_WIFI_MULTI("[WIFI][APlistAdd] password too long\n");
        return false;
    }

    if (APlistExists(ssid, password)) {
        DEBUG_WIFI_MULTI("[WIFI][APlistAdd] SSID: %s already exists\n", ssid);
        return true;
    }

    newAP.ssid = strdup(ssid);

    if (!newAP.ssid) {
        DEBUG_WIFI_MULTI("[WIFI][APlistAdd] fail newAP.ssid == 0\n");
        return false;
    }

    if (password) {
        newAP.password = strdup(password);
    } else {
        newAP.password = strdup("");
    }

    if (!newAP.password) {
        DEBUG_WIFI_MULTI("[WIFI][APlistAdd] fail newAP.password == 0\n");
        free(newAP.ssid);
        return false;
    }

    APlist.push_back(newAP);
    DEBUG_WIFI_MULTI("[WIFI][APlistAdd] add SSID: %s\n", newAP.ssid);
    return true;
}

bool AFG_WifiMulti::APlistExists(const char* ssid, const char *password) {
    if (!ssid || *ssid == 0x00 || strlen(ssid) > 32) {
        // fail SSID too long or missing!
        DEBUG_WIFI_MULTI("[WIFI][APlistExists] no ssid or ssid too long\n");
        return false;
    }
    for (auto entry : APlist) {
        if (!strcmp(entry.ssid, ssid)) {
            if (!password) {
                if (!strcmp(entry.password, "")) {
                    return true;
                }
            } else {
                if (!strcmp(entry.password, password)) {
                    return true;
                }
            }
        }
    }
	
    return false;
}

void AFG_WifiMulti::APlistClean(void) {
    for (auto entry : APlist) {
        if (entry.ssid) {
            free(entry.ssid);
        }
        if (entry.password) {
            free(entry.password);
        }
    }
	
    APlist.clear();
}