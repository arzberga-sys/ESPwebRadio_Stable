
// initializing Senderlist by flash-values - or, if not present, by defaults
void setup_senderList()
{
  // using a new, unified namespace for both the webinterface and the startup script
  sender.begin("senders_v4", false);

  // 1. Station
  strcpy(stationlist[0].name, sender.getString("n1", "Extra Radio").c_str());
  strcpy(stationlist[0].url, sender.getString("u1", "http://extra-radio.radionetz.de/extra-radio.mp3").c_str());
  
  // 2. Station (Dank Library-Patch funktioniert NDR 2 jetzt!)
  strcpy(stationlist[1].name, sender.getString("n2", "NDR 2").c_str());
  strcpy(stationlist[1].url, sender.getString("u2", "http://icecast.ndr.de/ndr/ndr2/niedersachsen/mp3/128/stream.mp3").c_str());

  // 3. Station
  strcpy(stationlist[2].name, sender.getString("n3", "Swiss Jazz").c_str());
  strcpy(stationlist[2].url, sender.getString("u3", "http://stream.srg-ssr.ch/m/rsj/mp3_128").c_str());

  // 4. Station
  strcpy(stationlist[3].name, sender.getString("n4", "WDR 2").c_str());
  strcpy(stationlist[3].url, sender.getString("u4", "http://wdr-wdr2-rheinruhr.icecast.wdr.de/wdr/wdr2/rheinruhr/mp3/128/stream.mp3").c_str());

  // 5. bis 10. Station (Reserveplätze)
  for (int i = 4; i < STATIONS; i++) {
    char keyN[4], keyU[4];
    sprintf(keyN, "n%d", i + 1);
    sprintf(keyU, "u%d", i + 1);
    strcpy(stationlist[i].name, sender.getString(keyN, "Leer").c_str());
    strcpy(stationlist[i].url, sender.getString(keyU, "").c_str());
  }
}
