
// initializing Senderlist by flash-values - or, if not present, by defaults
void setup_senderList()
{
  // using a new, unified namespace to force updating the saved stations in flash
  sender.begin("senders_v6", false);

  const char *default_names[STATIONS] = {
    "Extra Radio", "NDR 2", "Nius Radio", "Radio Ramasuri", "Mainwelle",
    "RMC 80s", "RMC 90s", "Antenne Bayern", "Bayern 1", "Radio Galaxy"
  };

  const char *default_urls[STATIONS] = {
    "http://extra-radio.radionetz.de/extra-radio.mp3",
    "http://icecast.ndr.de/ndr/ndr2/niedersachsen/mp3/128/stream.mp3",
    "http://nius.stream23.radiohost.de/live_aac-64",
    "http://ramasuri.radioho.st/ramasuri-live/mp3-192/",
    "http://webstream.mainwelle.de/radio-mainwelle.mp3",
    "http://edge.radiomontecarlo.net/rmcweb008",
    "http://edge.radiomontecarlo.net/rmcweb009",
    "http://s6-webradio.antenne.de/antenne/stream/mp3",
    "http://dispatcher.rndfnk.com/br/br1/schwaben/mp3/mid",
    "http://rs4.stream24.net/galaxy-passau.mp3"
  };

  for (int i = 0; i < STATIONS; i++) {
    char keyN[4], keyU[4];
    sprintf(keyN, "n%d", i + 1);
    sprintf(keyU, "u%d", i + 1);
    strcpy(stationlist[i].name, sender.getString(keyN, default_names[i]).c_str());
    strcpy(stationlist[i].url, sender.getString(keyU, default_urls[i]).c_str());
  }
}
