# ESPwebRadio_Stable

Ein stabiles und robustes Webradio-Projekt für den ESP32 mit Unterstützung für große Streams (NDR 2, WDR, etc.) und direkten Senderwechsel über einen Rotary Encoder!

## Bugfixes in diesem Release

1. **ESP32 Bootloader Hang (Hardware-Bug):** 
   Der Senderwechsel über `ESP.restart()` wurde entfernt, da einige ESP32 Boards einen Hard-Fault beim Software-Reset auslösen (`ets Jul 29 2019...` Boot-Loop). Der Stream-Wechsel erfolgt nun komplett nahtlos zur Laufzeit per `startUrl()`, ohne den Chip neu zu starten!

2. **Stream Freezes bei NDR 2 / WDR 1Live (Library-Bug):**
   Große Sender nutzen HTTP 302 Weiterleitungen (`rndfnk.com`). Die ESP8266Audio-Bibliothek folgt standardmäßig keinen Weiterleitungen für ICY Streams, was zum Einfrieren des `Decoder start...` und anschließendem Watchdog-Absturz führt.
   
   **WICHTIG:** Um diese Sender abzuspielen, muss die Bibliothek `ESP8266Audio` gepatcht werden. Eine fertige Patch-Datei liegt in diesem Repository bei!

3. **Speicher-Stabilität (Heap Fragmentation):**
   Der Puffer für den `AudioFileSourceBuffer` (60 KB) wird nun 1x statisch alloziert (`preallocateBuffer = malloc(preallocateBufferSize)`). Dadurch kann beim Senderwechsel der Speicher nicht mehr zersplittern!

## Installation & Patch-Anleitung

Damit Sender wie NDR 2 und WDR 1Live funktionieren, musst du deine `ESP8266Audio` Bibliothek patchen.
Die beiliegende Datei `ESP8266Audio_ICY_Redirect.patch` fügt die Funktion `http.setFollowRedirects(...)` hinzu.

1. Gehe in den Ordner deiner Arduino-Bibliotheken: `...\Arduino\libraries\ESP8266Audio\src\`
2. Öffne die Datei `AudioFileSourceICYStream.cpp`
3. Suche die Funktion `bool AudioFileSourceICYStream::open(const char *url)` (Zeile 42)
4. Füge direkt vor `int code = http.GET();` folgendes ein:
```cpp
#ifdef ESP32
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
#else
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
#endif
```
5. Kompiliere und lade den Webradio-Code hoch. Nun laufen auch NDR 2 und WDR 1Live absolut fehlerfrei!

## Bedienung
- **Rotary Encoder Drehen:** Sender in der Liste vor/zurück (Anzeige im LCD).
- **Automatischer Wechsel:** Nach 1,5 Sekunden ohne Eingabe stoppt der ESP32 automatisch den alten Sender und verbindet sich nahtlos mit dem Neuen.
- **Webinterface:** Alle 4 Sender können im Webinterface unter `192.168.178.x` editiert werden.
