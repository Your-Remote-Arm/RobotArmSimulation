// server.js
const express = require('express');
const app = express();
app.use(express.json());

const PLANE_SIZE = 5.0;

// buffer for the last received data
let lastLandmarks = null;

// receive updates but don’t print immediately
app.post('/landmarks', (req, res) => {
  lastLandmarks = req.body;
  res.sendStatus(200);
});

app.get('/landmarks', (req, res) => {
    res.json(lastLandmarks || {});
  });

// every 3 seconds, print the buffered landmarks rounded to 2 sig figs
setInterval(() => {
  if (!lastLandmarks) return;

  // map and round to 2 sig figs
  const rounded = {};
  for (let [name, [x, y, z, vis]] of Object.entries(lastLandmarks)) {
    const X = +(x * PLANE_SIZE).toPrecision(2);
    const Y = +((1 - y) * PLANE_SIZE).toPrecision(2);
    const Z = +(z * PLANE_SIZE).toPrecision(2);
    const V = +vis.toPrecision(2);
    rounded[name] = { X, Y, Z, Visibility: V };
  }

  console.clear();
  console.log(`Landmarks mapped to ${PLANE_SIZE}×${PLANE_SIZE} plane (updated every 3 s):`);
  console.table(rounded);

}, 3000);

const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Server listening on http://localhost:${PORT}/landmarks`);
});
