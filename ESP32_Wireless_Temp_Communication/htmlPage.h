const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html lang="pl">
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="utf-8">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    body { 
    background-image: url('https://www.pixelstalk.net/wp-content/uploads/images6/The-best-Minimalist-Desktop-Wallpaper-HD.jpg'); 
    background-repeat: repeat;
    background-size: cover; 
    }
    h2 { font-size: 3.0rem; }
    h3 { font-size: 2.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .ds-labels{
      font-size: 1.5rem;
      vertical-align: middle;
      padding-bottom: 15px;
    } 
    input[type=text] {
    border: 2px;
    border-radius: 4px;
    font-size: 1.5rem;
    }
    input[type=submit] {
    border: 2px;
    border-radius: 4px;
    font-size: 2rem;
    }
    label {
    font-size: 1.5rem;}

    .container {
    display: inline-block;
    position: relative;
    padding-right: 35px; /* Odstęp po prawej stronie tekstu */
    margin-bottom: 12px;
    cursor: pointer;
    font-size: 1.5rem;
    user-select: none;
    color: #000; /* Kolor tekstu */
  }

/* Ukrycie domyślnego checkboxa */
  .container input {
    position: absolute;
    opacity: 0;
    cursor: pointer;
  }

  /* Stylizowanie "checkmark" - widocznego elementu checkboxa */
  .checkmark {
    position: absolute;
    top: 0;
    right: 0; /* Umieszczenie checkboxa po prawej stronie */
    height: 25px;
    width: 25px;
    background-color: #fff;
    border: 2px solid #000; /* Czarna ramka */
    border-radius: 5px; /* Zaokrąglone rogi */
  }

  /* Checkbox zaznaczony */
  .container input:checked ~ .checkmark {
    background-color: #87CEEB; /* Zmieniony kolor na błękitny przy zaznaczeniu */
  }

  /* Dodanie ikony "check" po zaznaczeniu checkboxa */
  .checkmark:after {
    content: "";
    position: absolute;
    display: none;
  }

  .container input:checked ~ .checkmark:after {
    display: block;
  }

  /* Stylizowanie "check" */
  .container .checkmark:after {
    left: 9px;
    top: 5px;
    width: 5px;
    height: 10px;
    border: solid white;
    border-width: 0 3px 3px 0;
    transform: rotate(45deg);
  }
  </style>
</head>
<body>
  <h2>Temperatury zasilania i powrotów z podłogówki</h2>
  <h3>Temperatury piętro</h3>
  <p>
    <i class="fas fa-thermometer-half" style="color:#f21d1d;"></i>
    <span class="ds-labels">Temperatura zasilania </span>
    <span id="temperaturecPz">%TEMPERATURECPZ%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-thermometer-half" style="color:#1565c0;"></i> 
    <span class="ds-labels">Temperatura powrotu </span>
    <span id="temperaturecPp">%TEMPERATURECPP%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <h3>Temperatury parter</h3>
  <p>
    <i class="fas fa-thermometer-half" style="color:#f21d1d;"></i>
    <span class="ds-labels">Temperatura zasilania </span>
    <span id="temperaturecParZ">%TEMPERATURECPARZ%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-thermometer-half" style="color:#1565c0;"></i> 
    <span class="ds-labels">Temperatura powrotu </span>
    <span id="temperaturecParP">%TEMPERATURECPARP%</span>
    <sup class="units">&deg;C</sup>
  </p>
  
  <h2>Regulacja temperatury wody zasilającej podłogówki</h2>
  <label for="regulationTempEn" class="container">Włączenie regulacji temperatury
  <input type="checkbox" onclick="toggleForm()" checked="checked" id="regulationTempEn"></input>
  <span class="checkmark"></span></label>
  <br><br>
  
  <div id="formBlock">
    <h3>Aktualna ustawiona temperatura piętro:</h3>
    <p>%SET_TEMP_UP%</p><br>
    <h3>Aktualna ustawiona temperatura parter:</h3>
    <p>%SET_TEMP_DOWN%</p><br>

    <form action="/update" target="hidden-form">
      <label for="tempUp">Temperatura zasilania piętro:</label>
      <input type="text" id="tempUp" name="tempUp"><br><br>
      <label for="tempDown">Temperatura zasilania parter:</label>
      <input type="text" id="tempDown" name="tempDown"><br><br>
      <input type="submit" onclick="submitMessage()" value="Zapisz">
    </form>
    <iframe style="display:none" name="hidden-form"></iframe>
  </div>
</body>
<script>
setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperaturecPz").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperaturecPz", true);
  xhttp.send();
}, 2000);

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperaturecPp").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperaturecPp", true);
  xhttp.send();
}, 2000);

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperaturecParZ").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperaturecParZ", true);
  xhttp.send();
}, 2000);

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperaturecParP").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperaturecParP", true);
  xhttp.send();
}, 2000);

function submitMessage() {
  alert("Zapisano ustawioną temperaturę");
  setTimeout(function () {document.location.reload(false); }, 500);
}

function toggleForm() {
  let formBlock = document.getElementById("formBlock");
  let checkbox = document.getElementById("regulationTempEn");
  if (checkbox.checked){
    formBlock.style.display = 'inline';
  }
  else {
    formBlock.style.display = 'none';
  }

}

</script>
</html>)rawliteral";