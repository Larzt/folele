#pragma once

// Control panel served at GET /. Polls /status every 2 s.
constexpr const char RootPage[] = R"rawliteral(<!doctype html>
<html lang="es">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Folele</title>
<style>
  :root {
    --bg: #0f1419; --card: #182029; --line: #263240; --text: #e6edf3;
    --muted: #8b98a5; --accent: #3b82f6; --ok: #22c55e; --warn: #f59e0b; --danger: #ef4444;
  }
  * { box-sizing: border-box; }
  body {
    margin: 0; background: var(--bg); color: var(--text);
    font: 15px/1.45 -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
  }
  main { max-width: 560px; margin: 0 auto; padding: 20px 16px 40px; }
  header { display: flex; align-items: center; justify-content: space-between; margin-bottom: 18px; }
  h1 { margin: 0; font-size: 1.6rem; letter-spacing: .5px; }
  .sub { color: var(--muted); font-size: .85rem; }
  .pill { padding: 6px 12px; border-radius: 999px; font-weight: 600; font-size: .85rem; border: 1px solid; }
  .pill.idle { color: var(--warn); border-color: var(--warn); }
  .pill.flying { color: var(--ok); border-color: var(--ok); }
  .pill.off { color: var(--danger); border-color: var(--danger); }
  section { background: var(--card); border: 1px solid var(--line); border-radius: 14px; padding: 16px; margin-bottom: 14px; }
  h2 { margin: 0 0 12px; font-size: .8rem; text-transform: uppercase; letter-spacing: 1px; color: var(--muted); }
  .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; }
  .label { color: var(--muted); font-size: .8rem; }
  .value { font-size: 1.05rem; font-weight: 600; word-break: break-all; }
  .bar { height: 8px; background: var(--line); border-radius: 4px; overflow: hidden; margin-top: 6px; }
  .bar > div { height: 100%; width: 0; background: var(--accent); transition: width .4s; }
  .row { display: flex; gap: 10px; }
  button {
    flex: 1; padding: 12px; border: 0; border-radius: 10px; font-size: 1rem; font-weight: 600;
    color: #fff; background: var(--accent); cursor: pointer; transition: opacity .15s, transform .1s;
  }
  button:active { transform: scale(.98); }
  button:disabled { opacity: .5; cursor: default; }
  button.arm { background: var(--ok); }
  button.disarm { background: var(--danger); }
  button.ghost { background: transparent; border: 1px solid var(--line); color: var(--text); }
  input {
    width: 100%; padding: 10px 12px; margin: 4px 0 12px; border-radius: 8px;
    border: 1px solid var(--line); background: var(--bg); color: var(--text); font-size: 1rem;
  }
  input:focus { outline: none; border-color: var(--accent); }
  .hint { color: var(--muted); font-size: .8rem; margin: 10px 0 0; }
  #toast {
    position: fixed; left: 50%; bottom: 20px; transform: translateX(-50%); background: var(--line);
    padding: 10px 16px; border-radius: 10px; opacity: 0; transition: opacity .2s; pointer-events: none;
  }
  code { color: var(--text); }
</style>
</head>
<body>
<main>
  <header>
    <div>
      <h1>Folele</h1>
      <div class="sub">Drone &middot; ESP32</div>
    </div>
    <span id="pill" class="pill off">Sin conexi&oacute;n</span>
  </header>

  <section>
    <h2>Control</h2>
    <div class="row">
      <button id="toggle" disabled>&hellip;</button>
      <button id="ping" class="ghost">Ping</button>
    </div>
    <p class="hint">Tambi&eacute;n se puede armar/desarmar con el bot&oacute;n BOOT de la placa (interrupci&oacute;n GPIO0).</p>
  </section>

  <section>
    <h2>Energ&iacute;a</h2>
    <div class="grid">
      <div><div class="label">Modo</div><div class="value" id="mode">&ndash;</div></div>
      <div><div class="label">Entradas en light sleep</div><div class="value" id="count">&ndash;</div></div>
      <div><div class="label">Light sleep (&uacute;ltimos segundos)</div><div class="value" id="recent">&ndash;</div>
        <div class="bar"><div id="recentBar"></div></div></div>
      <div><div class="label">Light sleep (desde el arranque)</div><div class="value" id="total">&ndash;</div>
        <div class="bar"><div id="totalBar"></div></div></div>
    </div>
  </section>

  <section>
    <h2>Red</h2>
    <div class="grid">
      <div><div class="label">Modo WiFi</div><div class="value" id="net">&ndash;</div></div>
      <div><div class="label">IP</div><div class="value" id="ip">&ndash;</div></div>
    </div>
  </section>

  <section>
    <h2>Conectar a una red (modo estaci&oacute;n)</h2>
    <form id="wifi">
      <label class="label" for="ssid">SSID</label>
      <input id="ssid" name="ssid" autocomplete="off" required>
      <label class="label" for="password">Contrase&ntilde;a</label>
      <input id="password" name="password" type="password">
      <button type="submit">Guardar y conectar</button>
    </form>
    <p class="hint">El dron dejar&aacute; su punto de acceso y se unir&aacute; a esa red (su IP aparece en el monitor serie).
      Si no lo consigue, vuelve al punto de acceso.</p>
  </section>

  <p class="hint">API: <code>GET /status</code> &middot; <code>GET /ping</code> &middot; <code>POST /arm</code> &middot;
    <code>POST /disarm</code> &middot; <code>GET /power/stats</code> &middot; <code>POST /wifi</code></p>
</main>
<div id="toast"></div>

<script>
const $ = (id) => document.getElementById(id);
let last = null, state = null;

function toast(text) {
  $('toast').textContent = text;
  $('toast').style.opacity = 1;
  setTimeout(() => $('toast').style.opacity = 0, 2200);
}

function pct(part, whole) { return whole > 0 ? Math.round(part * 100 / whole) : 0; }

function render(s) {
  state = s.power;
  const flying = s.power === 'flying';
  $('pill').className = 'pill ' + s.power;
  $('pill').textContent = flying ? 'Volando' : 'En reposo';
  $('toggle').disabled = false;
  $('toggle').className = flying ? 'disarm' : 'arm';
  $('toggle').textContent = flying ? 'Desarmar' : 'Armar';

  $('mode').textContent = flying ? 'Activo' : 'Light sleep autom.';
  $('count').textContent = s.sleep_count;
  const total = pct(s.sleep_us, s.uptime_us);
  $('total').textContent = total + ' %';
  $('totalBar').style.width = total + '%';
  if (last) {
    const recent = pct(s.sleep_us - last.sleep_us, s.uptime_us - last.uptime_us);
    $('recent').textContent = recent + ' %';
    $('recentBar').style.width = recent + '%';
  }
  last = s;

  $('net').textContent = s.network === 'station' ? 'Estación (STA)' : 'Punto de acceso (' + s.ap_ssid + ')';
  $('ip').textContent = s.ip;
  if (!$('ssid').value && document.activeElement !== $('ssid')) $('ssid').value = s.sta_ssid;
}

async function refresh() {
  if (document.hidden) return;
  try {
    const res = await fetch('/status', { cache: 'no-store' });
    render(await res.json());
  } catch (e) {
    $('pill').className = 'pill off';
    $('pill').textContent = 'Sin conexión';
    $('toggle').disabled = true;
  }
}

$('toggle').onclick = async () => {
  $('toggle').disabled = true;
  await fetch(state === 'flying' ? '/disarm' : '/arm', { method: 'POST' });
  refresh();
};

$('ping').onclick = async () => {
  const res = await fetch('/ping', { cache: 'no-store' });
  const data = await res.json();
  toast('pong · LED ' + (data.led_state ? 'encendido' : 'apagado'));
};

$('wifi').onsubmit = async (event) => {
  event.preventDefault();
  const body = new URLSearchParams(new FormData($('wifi')));
  const res = await fetch('/wifi', { method: 'POST', body });
  toast(res.ok ? 'Guardado. Conectando a ' + $('ssid').value + '…' : 'Error al guardar');
};

refresh();
setInterval(refresh, 2000);
</script>
</body>
</html>
)rawliteral";
