const char SENDER_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <style>
  body {
    text-align:center;
    font-family: helvetica;
  }
  </style>
  <head>
    <meta charset="utf-8" />
    <title>
      ESP-Webradio Senderliste
    </title>
    <style type="text/css">

      a:link, a:visited {
        font-family: Arial, Helvetica, sans-serif;
        font-size: 20pt;
        color: #000000;
        text-decoration: none;
        margin-right: 30px; 
      }
      a:hover, a:active {
        color: #000000;
        text-decoration: underline;
      }

      p {
          //width: 20em;
          font-size: 3.0em;
      }

      .playing {
        font-family: Arial, Helvetica, sans-serif;
        font-size: 40pt;
        color: #00AA00;
        text-decoration: none;
        margin-right: 30px; 
      }

      .btn {
        font-family: Arial, Helvetica, sans-serif;
        font-size: 16pt !important;
        color: #ffffff !important;
        background-color: #5555FF;
        padding: 10px 20px;
        border-radius: 10px;
        text-decoration: none !important;
        margin-right: 10px;
        display: inline-block;
      }
      .btn:hover {
        opacity: 0.85;
      }

    </style>
  <head>
  <body>
    <h1>ESP-Webradio</h1>
    <div style="font-size: 1.3em; font-weight: bold; margin: 10px 0 15px 0; color: *statuscolor*;">
      *radiostatus*
    </div>
    <p>
      *feedback1*
    </p>
    <p style="margin: 20px 0;">
      <a href="?power=toggle" class="btn" style="background-color: *powercolor*;">*powerlabel*</a>
      <a href="?vol=mute" class="btn">*mutelabel*</a>
    </p>
    <hr>
    <h3>Lautstärke</h3>
    <form method="POST">
      <input type="range" name="volume" min="0" max="20" value="*volvalue*" onchange="this.form.submit()" style="width: 80%; height: 30px; margin-bottom: 20px;">
    </form>
    <hr>
    <h3>Senderliste</h3>
    <form method="POST">
      <input type="submit" value="alles speichern" style="background-color:#FF5555;">
      <hr>
*fields*
      <hr>
      <input type="submit" value="alles speichern" style="background-color:#FF5555;">
      <hr>
    </form>
    <p>
      *feedback2*
    </p>
  </body>
</html>
)=====";
