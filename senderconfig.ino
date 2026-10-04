// must use asynchronous webserver, otherwise playing will be interrupted :)
#include "ESPAsyncWebServer.h"
#include <Preferences.h>

#include "senderconf.h"  //HTML code template

// Webserver on port 80
AsyncWebServer serverC(80);

extern float currentVolume;
extern bool isMuted;
void applyVolume();

// Webserver-config - called once in 'Setup()' 
void setup_senderConfig() {

  // eventhandling when loading site: "http://<radio-IP>/"
  serverC.on("/", HTTP_ANY, [](AsyncWebServerRequest *request){

    Preferences sender;                 // preferences-instance for senderlist
    // match the namespace from senderlist_Extra.ino
    sender.begin("senders_v6", false);

    extern bool webPlayRequest;

    int paramsNr = request->params();   // if submit, here we'll get 40 POST-parameters
    Serial.println(paramsNr);
    int paramCountOk = 0;
    int gotAnswer = false;
    int j;
    String para = "";

    // Check for action parameters (play, vol, volume)
    bool isAction = false;
    bool isVolumeChanged = false;
    
    for (int i=0; i<paramsNr; i++) {
      AsyncWebParameter* p2 = request->getParam(i);
      
      if (p2->name() == "play") {
        curStation = atoi((p2->value()).c_str());
        actStation = curStation;
        pref.putUShort("station",curStation);
        webPlayRequest = true;
        isAction = true;
      }
      else if (p2->name() == "vol") {
        String volStr = p2->value();
        if (volStr == "mute") {
          isMuted = !isMuted;
          isVolumeChanged = true;
        }
        isAction = true;
      }
      else if (p2->name() == "volume") {
        float newVol = p2->value().toFloat() / 10.0;
        currentVolume = newVol;
        if (currentVolume > 2.0) currentVolume = 2.0;
        if (currentVolume < 0.0) currentVolume = 0.0;
        isVolumeChanged = true;
        isAction = true;
      }
    }
    
    if (isVolumeChanged) {
        pref.begin("radio", false);
        pref.putFloat("volume", currentVolume);
        pref.putBool("mute", isMuted);
        pref.end();
        applyVolume();
    }
    
    if (isAction) {
        request->redirect("/");
        return;
    }

    
    // ### Parameter handling
    // is only called, when list-form is "submitted". In this case 
    // it loops STATIONS*2 times
    for(int i=0;i<paramsNr;i++){ 
  
      AsyncWebParameter* p = request->getParam(i);    // read parameter
      Serial.print("Param name: ");
      Serial.println(p->name());
      Serial.print("Param value: ");
      Serial.println(p->value());
      Serial.println("------");

      // ### search parameter in 'stationlist' and actualize
      for (j=0; j<STATIONS; j++) {
        para = "name";
        para.concat(j+1);
        if (p->name() == para  ) { strcpy(stationlist[j].name, (p->value()).c_str()); paramCountOk++; }
      }
      for (j=0; j<STATIONS; j++) {
        para = "url";
        para.concat(j+1);
        if (p->name() == para ) { strcpy(stationlist[j].url, (p->value()).c_str()); paramCountOk++; }
      }

      // ### search parameter in prefenrences object 'sender' and actualize
      if (paramCountOk >= paramsNr) {
        for(j=0; j<STATIONS; j++) {
          para = "n";
          para.concat(j+1);
          sender.putString(para.c_str(), stationlist[j].name);
        }
        for(j=0; j<STATIONS; j++) {
          para = "u";
          para.concat(j+1);
          sender.putString(para.c_str(), stationlist[j].url);
        }

        sender.end();
        gotAnswer = true;
      }
    }

    // ### Fill HTML template
    String s = SENDER_page;         // read HTML template
    
    if (isMuted) {
      s.replace("*mutelabel*", "Ton an");
    } else {
      s.replace("*mutelabel*", "Stumm");
    }
    s.replace("*volvalue*", String((int)(currentVolume * 10)));
    
    String fields = "";             // workmemory for field-HTML-code


    for (j=0; j<STATIONS; j++){
      fields.concat("<p>\n<h4>Platz ");
      fields.concat(j+1);
      fields.concat(":</h4>\n");
      if (j == curStation) {
        fields.concat("<span class='playing'>&#9836;</span>");
      } else {
        fields.concat("<a title='Diesen Sender spielen' href='?play=");
        fields.concat(j);
        fields.concat("'>&#9654;</a>");
      }
      fields.concat("&nbsp;<input type='text' name='name");
      fields.concat(j+1);
      fields.concat("' value='");
      fields.concat(stationlist[j].name);
      fields.concat("' size='20'>\n<input type='text' name='url");
      fields.concat(j+1);
      fields.concat("' value='");
      fields.concat(stationlist[j].url);
      fields.concat("' size='100'>\n</p>\n");
    }
    s.replace("*fields*", fields);  // put fieldcode into template

    // if something is saved, feedback shoud be given
    if (gotAnswer) {
      s.replace("*feedback1*", "<div style='color:#CC0000'><h1>Gespeichert!</h1></div>");
      s.replace("*feedback2*", "<div style='color:#CC0000'><h1>Gespeichert!</h1></div>");
    } else {
      s.replace("*feedback1*", "");
      s.replace("*feedback2*", "");
    }

    // ### Send HTML-code to Browser
    request->send(200, "text/html", s);

    // ### Switch happens seamlessly in loop() now  

  }); 

  // ### activate the obove configured webserver
  serverC.begin();
  Serial.println("Webserver ist gestartet");

}
