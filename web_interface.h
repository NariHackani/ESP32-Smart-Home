#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>SESNUM Smart Home</title>
  <style>
    :root { --bg: #0f172a; --card: #1e293b; --accent: #38bdf8; }
    body { background-color: var(--bg); color: #ffffff; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; text-align: center; margin: 0; padding: 20px; }
    
    h1 { color: var(--accent); letter-spacing: 2px; text-transform: uppercase; margin-bottom: 30px; }
    
    .grid { display: flex; flex-wrap: wrap; justify-content: center; gap: 20px; max-width: 600px; margin: 0 auto; }
    
    .card { background: var(--card); width: 250px; padding: 20px; border-radius: 15px; border: 1px solid #334155; box-shadow: 0 10px 15px -3px rgba(0,0,0,0.5); }
    .card h3 { margin-top: 0; font-size: 1.2rem; color: var(--accent); }

    .btn { 
      display: block; width: 200px; margin: 10px auto; padding: 15px; 
      border: none; border-radius: 10px; color: white; font-weight: bold; 
      font-size: 1.1rem; cursor: pointer; transition: transform 0.2s, background 0.3s;
    }
    /* Classes d'etat */
    .button { background-color: #22c55e; box-shadow: 0 4px 0 #15803d; } /* Vert (ON possible) */
    .button2 { background-color: #64748b; box-shadow: 0 4px 0 #475569; } /* Gris (OFF possible) */
    .btn:active { transform: translateY(4px); box-shadow: none; }

    .reset-area { margin-top: 40px; }
    .btn-reset { background: transparent; border: 2px solid #ef4444; color: #ef4444; padding: 10px 25px; border-radius: 50px; cursor: pointer; font-weight: bold; }
    .btn-reset:hover { background: #ef4444; color: white; }
  </style>
</head>
<body>
    <h1>🏠 Smart Home Dashboard</h1>
    
    <div class="grid">
      <div class="card">
        <h3>SALON</h3>
        <a href="/set?button_id=1"><button id="btn1" class="%btn1class%">%btn1txt%</button></a>
      </div>
      
      <div class="card">
        <h3>CUISINE</h3>
        <a href="/set?button_id=2"><button id="btn2" class="%btn2class%">%btn2txt%</button></a>
      </div>

      <div class="card">
        <h3>CHAMBRE</h3>
        <a href="/set?button_id=3"><button id="btn3" class="%btn3class%">%btn3txt%</button></a>
      </div>

      <div class="card">
        <h3>GARAGE</h3>
        <a href="/set?button_id=4"><button id="btn4" class="%btn4class%">%btn4txt%</button></a>
      </div>
    </div>

    <div class="reset-area">
      <a href="/reset"><button class="btn-reset">TOUT ÉTEINDRE</button></a>
    </div>

    <script>
      // Connexion aux evenements en temps reel (SSE)
      if (!!window.EventSource) {
        var source = new EventSource('/events');
        source.addEventListener('toggleState', function(e) {
          var data = JSON.parse(e.data);
          var element = document.getElementById(data.id);
          if(element) {
            element.innerHTML = (data.status == 1) ? "OFF" : "ON";
            element.className = (data.status == 1) ? "btn button2" : "btn button";
          }
        }, false);
      }
    </script>
</body>
</html>)rawliteral";

#endif