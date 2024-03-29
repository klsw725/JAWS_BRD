const char Config_Page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <title>SHACS Configure</title>
  <meta charset="UTF-8" content="width=device-width, initial-scale=1" name="viewport">
  <style>
    html {
      text-align: center;
    }

    p {
      color: blue;
      font-size: 20px;
      text-align: left;
      margin: 0;
    }

    div {
      width: 100%;
      margin: auto;
    }

    @media screen and (min-width: 300px) {
      div {
        width: 300px;
      }
    }

    .s_serif {
      font-family: Arial, Helvetica, sans-serif;
    }

    input[type=text], input[type=password] {
      width: 100%;
      font-size: 24px;
      border: none;
      border-bottom: 2px solid blue;
      margin-bottom: 20px;
    }

    input[type=submit] {
      background-color: #4CAF50;
      color: white;
      padding: 14px 20px;
      margin: 8px 0;
      border: none;
      border-radius: 4px;
    }

    input[type=submit]:active:after {
      background-image: radial-gradient(circle, #00ff00, #4CAF50);
      color: black;
    }

    input[type=submit]:after {
      background-color: #4CAF50;
      transition: 0.8s;
    }
  </style>
</head>
<body>

  <h1 class="s_serif">Network Setting</h1>

  <form action="/" class="s_serif" method="post" name="setting">
    <div>
      <p>SSID :</p>
      <input accept-charset="utf-8" name="ssid" placeholder="네트워크 ID 입력" required type="text">
      <p>Password :</p>
      <input name="pw" placeholder="비밀번호 입력" required type="password">
      <p>MQTT :</p>
      <input name="broker" placeholder="Broker URL" required type="text">
      <p>Port :</p>
      <input name="port" placeholder="Broker Port" required type="text">
    </div>
    <input type="submit" value="Submit">
  </form>

  <script>
    var listForm = [
      {field: "ssid", width: 200},
      {field: "rssi", width: 100}
    ];
    var ssidList = [
      {"ssid": "shacs", "rssi": "-10"}
    ]
  </script>
</body>
</html>
)=====";

const char Refresh_Page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <title>Configuration...</title>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="3; url=/">
  </head>
  <body>
    <p>설정중입니다...</p>
  </body>
</html>
)=====";

/*
const char SubmitAccepted_page[] PROGMEM = R"=====(

<!DOCTYPE html>
<html>
  <head>
    <title>SHACS Configure</title>
    <meta charset="UTF-8">
    </head>
  <body>
    <h1 class="s_serif"> SHACS Configuration</h1>
    <div>
      <a href="/reset">
        <input type="button" value="reset" onclick="location.href=/reset">
      </a>
    </div>
  </body>
</html>

<!DOCTYPE html>
<html>
  <head>
    <title>Configuration Success</title>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="3; url=/">
  </head>
  <body>
    <p>설정 완료! 잠시후 이전 페이지로 이동합니다.</p>
  </body>
</html>
)=====";
*/
