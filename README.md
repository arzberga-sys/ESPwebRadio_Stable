# ESPwebRadio_Stable

Ein stabiles und robustes Webradio-Projekt für den ESP32 mit Unterstützung für große Streams (NDR 2, FFH, Absolut Bella etc.), Webinterface mit Lautstärkeregler und echtem Standby-Modus!

## Features & Highlights

1. **Echter Standby-Modus (0 KB/s Datenverkehr):**
   * Per Klick auf den Dreh-Encoder-Taster (Pin 27) oder über den Power-Button im Webinterface kann das Radio jederzeit pausiert werden.
   * Im Standby wird der Stream getrennt, der Puffer freigegeben und der I2S-DAC stummgeschaltet – kein unnötiger Traffic, keine Serverlast und minimale CPU-Erwärmung.
   * Drehen am Encoder oder Klick auf einen Sender weckt das Radio sofort wieder auf.

2. **Webinterface mit Live-Steuerung:**
   * Power-Button (Ein / Aus) mit synchroner Statusanzeige.
   * Stufenloser Schieberegler für die Lautstärke sowie Stummschaltung (Mute).
   * Dynamische Konfiguration aller 6 Senderplätze inklusive Speicherung im NVS-Flash.

3. **Nahtloser Senderwechsel ohne ESP-Restart:**
   * Der Senderwechsel erfolgt komplett zur Laufzeit per `startUrl()`, ohne den Chip neu zu starten (verhindert Bootloader-Hänger bestimmter ESP32-Boards).

4. **Stream-Redirect & Timeout-Schutz:**
   * Unterstützung für HTTP 302 Weiterleitungen (`rndfnk.com`, `streamonkey` etc.) über die gepatchte `ESP8266Audio`-Bibliothek.
   * 5-Sekunden-Timeout verhindert Hänger bei instabilen Netzwerkverbindungen.

## Installation & Patch-Anleitung

Damit Sender mit Weiterleitungen stabil funktionieren und keine Netzwerk-Hänger auftreten, muss die Datei `AudioFileSourceICYStream.cpp` in der `ESP8266Audio`-Bibliothek angepasst werden (liegt auch als `ESP8266Audio_ICY_Redirect.patch` bei).

1. Gehe in den Ordner deiner Arduino-Bibliotheken: `...\Arduino\libraries\ESP8266Audio\src\`
2. Öffne die Datei `AudioFileSourceICYStream.cpp`
3. Suche die Funktion `bool AudioFileSourceICYStream::open(const char *url)` (ca. Zeile 42)
4. Füge direkt vor `int code = http.GET();` folgendes ein:
```cpp
#ifdef ESP32
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
#else
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
#endif
  http.setTimeout(5000);
```
5. Kompiliere und lade den Sketch hoch.

## Bedienung
- **Rotary Encoder Drehen:** Sender in der Liste vor/zurück (automatischer Wechsel nach 600 ms).
- **Rotary Encoder Drücken (Pin 27):** Schaltet das Radio zwischen Wiedergabe und Standby um.
- **Webinterface (`http://<ESP32-IP>/`):** Senderwahl, Senderliste konfigurieren, Lautstärke regeln, Mute und Standby schalten.

