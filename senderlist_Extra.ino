
// initializing Senderlist by flash-values - or, if not present, by defaults
void setup_senderList()
{
  // using a new, unified namespace to force updating the saved stations in flash
  sender.begin("senders_v11", false);

  const char *default_names[STATIONS] = {
    "Extra Radio", "NDR 2", "Mainwelle",
    "FFH 2000er", "Absolut Bella", "80s80s", "Radio Galaxy"
  };

  const char *default_urls[STATIONS] = {
    "http://extra-radio.radionetz.de/extra-radio.mp3",
    "http://icecast.ndr.de/ndr/ndr2/niedersachsen/mp3/128/stream.mp3",
    "http://webstream.mainwelle.de/radio-mainwelle.mp3",
    "http://mp3.ffh.de/ffhchannels/hq2000er.mp3",
    "http://absolut-bella.live-sm.absolutradio.de/absolut-bella/stream/mp3",
    "http://regiocast.streamabc.net/regc-80s80smweb2517500-mp3-128-7712602",
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
