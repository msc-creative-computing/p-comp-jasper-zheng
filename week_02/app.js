const express = require("express");
const five = require("johnny-five");
var board = new five.Board();
const app = express();

var sensor;
var sensorValue = 0;

app.listen(3000, () => {
    console.log("server running on port 3000");
});

app.use(function(req, res, next) {
    res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Headers", "*");
    next();
});

app.route("/")
    .get((req, res) => {
        readSensor();
        res.json({value: sensorValue});
});

board.on("ready", () => {
  sensor = new five.Sensor("A0");
});

function readSensor() {
  sensorValue = sensor.scaleTo(0, 490)
  // console.log(sensorValue);
}
