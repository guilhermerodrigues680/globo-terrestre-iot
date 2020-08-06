const express = require('express');
const path = require("path")

const app = express();
const PORT = 8000;

app.use((req, res, next) => {
  console.log(new Date().toLocaleString('pt-br'), req.ip, req.method, req.url);
  next();
});

const filesystemESP8266 = path.resolve(__dirname, '../data');

// app.use('/', express.static(filesystemESP8266));
app.use(express.static(filesystemESP8266));

app.listen(PORT, () => {
    console.log(`Serving: ${filesystemESP8266}`);
    console.log(`Listening on port ${PORT}!`);
});
