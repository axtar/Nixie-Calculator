// HTMLTests.h

// provides the ratpak/calculator self-test page (CALC_TESTS builds only)

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

const char htmlTests[] = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
  <meta charset="UTF-8">
  <title>Nixie Calculator Tests</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    :root {
      --bg: #f4f1ec;
      --card-bg: #ffffff;
      --border: #e2dccf;
      --text: #2b2620;
      --text-muted: #7a7266;
      --accent: #ff8c00;
      --accent-dark: #d97400;
      --header-bg: #2f6f9f;
      --pass: #2e8b40;
      --fail: #c0392b;
      --radius: 8px;
    }

    * {
      box-sizing: border-box;
    }

    html {
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
      background: var(--bg);
      color: var(--text);
      text-align: center;
    }

    body {
      margin: 0;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
    }

    h1 {
      margin: 0;
      font-size: 1.25rem;
      font-weight: 600;
      color: #fff;
      letter-spacing: .02em;
    }

    .headline {
      background: var(--header-bg);
      padding: 10px 16px;
      border-bottom: 3px solid var(--accent);
    }

    .content {
      max-width: 700px;
      width: 100%;
      box-sizing: border-box;
      margin: 12px auto;
      text-align: left;
      padding: 0 14px 14px;
    }

    .toolbar {
      display: flex;
      gap: 10px;
      flex-wrap: wrap;
      align-items: center;
      margin: 12px 0;
    }

    button {
      padding: 9px 20px;
      font-size: .95rem;
      font-weight: 600;
      border: 1px solid var(--accent);
      background-color: var(--accent);
      color: #fff;
      border-radius: var(--radius);
      cursor: pointer;
      transition: background-color .15s ease, border-color .15s ease;
    }

    button:hover {
      background-color: var(--accent-dark);
      border-color: var(--accent-dark);
    }

    button:disabled {
      opacity: .6;
      cursor: default;
    }

    #summary {
      display: block;
      margin: 0 0 12px;
      font-weight: 600;
      color: var(--text-muted);
    }

    .group {
      background: var(--card-bg);
      border: 1px solid var(--border);
      border-radius: var(--radius);
      margin: 10px 0;
      overflow: hidden;
      box-shadow: 0 1px 2px rgba(0, 0, 0, .04);
    }

    .group-title {
      background: #efe9df;
      padding: 6px 12px;
      font-weight: 600;
      font-size: .85rem;
      text-transform: uppercase;
      letter-spacing: .04em;
      color: var(--text-muted);
    }

    .row {
      display: flex;
      flex-wrap: wrap;
      justify-content: space-between;
      align-items: baseline;
      gap: 10px;
      padding: 7px 12px;
    }

    .row+.row {
      border-top: 1px solid var(--border);
    }

    .row .name {
      font-weight: 600;
    }

    .row .status {
      flex: 0 0 auto;
      font-weight: 700;
    }

    .row.pass .status {
      color: var(--pass);
    }

    .row.fail .status {
      color: var(--fail);
    }

    .row .meta {
      flex-basis: 100%;
      font-size: .85rem;
      color: var(--text-muted);
    }

    .row.fail .meta {
      color: var(--fail);
    }

    #saveBar {
      flex: 0 0 auto;
      margin-top: auto;
      position: sticky;
      bottom: 0;
      text-align: center;
      padding: 14px 0;
      background: rgba(244, 241, 236, .92);
      backdrop-filter: blur(6px);
      border-top: 1px solid var(--border);
    }

    #backButton {
      display: inline-block;
      padding: 9px 28px;
      font-size: .95rem;
      font-weight: 600;
      text-decoration: none;
      background-color: var(--accent);
      border: 1px solid var(--accent);
      color: #fff;
      border-radius: var(--radius);
      transition: background-color .15s ease, border-color .15s ease;
    }

    #backButton:hover {
      background-color: var(--accent-dark);
      border-color: var(--accent-dark);
    }
  </style>
</head>

<body>
  <div class="headline">
    <h1>Nixie Calculator Tests</h1>
  </div>
  <div class="content">
    <div class="toolbar">
      <button id="runCorrectness" onclick="runTests('/api/tests', false)">Run correctness tests</button>
      <button id="runPerformance" onclick="runTests('/api/tests/performance', true)">Run performance tests</button>
    </div>
    <span id="summary"></span>
    <div id="results"></div>
  </div>
  <div id="saveBar">
    <a href="/" id="backButton">Back</a>
  </div>
  <script>
    function formatDuration(us) {
      if (us >= 1000000) {
        return (us / 1000000).toFixed(2) + " s";
      }
      if (us >= 1000) {
        return (us / 1000).toFixed(1) + " ms";
      }
      return us + " µs";
    }

    function render(tests) {
      const resultsEl = document.getElementById('results');
      resultsEl.innerHTML = '';
      const groups = new Map();
      for (const t of tests) {
        if (!groups.has(t.category)) {
          groups.set(t.category, []);
        }
        groups.get(t.category).push(t);
      }
      let passCount = 0;
      for (const t of tests) {
        if (t.pass) {
          passCount++;
        }
      }
      document.getElementById('summary').textContent = tests.length ? `${passCount} / ${tests.length} passed` : '';

      for (const [category, items] of groups) {
        const group = document.createElement('div');
        group.className = 'group';
        const title = document.createElement('div');
        title.className = 'group-title';
        title.textContent = category;
        group.appendChild(title);
        for (const t of items) {
          const row = document.createElement('div');
          row.className = 'row ' + (t.pass ? 'pass' : 'fail');
          const name = document.createElement('span');
          name.className = 'name';
          name.textContent = t.name;
          const status = document.createElement('span');
          status.className = 'status';
          status.textContent = (t.pass ? 'PASS' : 'FAIL') + ' · ' + formatDuration(t.durationUs);
          row.appendChild(name);
          row.appendChild(status);
          if (t.detail) {
            const meta = document.createElement('span');
            meta.className = 'meta';
            meta.textContent = t.detail;
            row.appendChild(meta);
          }
          group.appendChild(row);
        }
        resultsEl.appendChild(group);
      }
    }

    function runTests(url, isSlow) {
      const buttons = document.querySelectorAll('button');
      buttons.forEach(b => b.disabled = true);
      document.getElementById('results').innerHTML = '';
      document.getElementById('summary').textContent = isSlow ? 'Running (this can take a while)…' : 'Running…';
      fetch(url)
        .then(response => response.json())
        .then(data => {
          render(data);
        })
        .catch(() => {
          document.getElementById('summary').textContent = 'Request failed';
        })
        .finally(() => {
          buttons.forEach(b => b.disabled = false);
        });
    }
  </script>
</body>

</html>
)rawliteral";
