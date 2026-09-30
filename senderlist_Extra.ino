
// initializing Senderlist by flash-values - or, if not present, by defaults
void setup_senderList()
{
  // changed namespace to 'senderlist5' to force overwrite of the saved URL
  sender.begin("senderlist5", false);

  // 1. Station
  strcpy(stationlist[0].name, sender.getString("n1", "Extra Radio").c_str());
  strcpy(stationlist[0].url, sender.getString("u1", "http://extra-radio.radionetz.de/extra-radio.mp3").c_str());
}
