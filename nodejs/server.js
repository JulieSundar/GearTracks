var express = require('express');
var app = express();
//REQUIRE//
//var five = require("johnny-five");
//var Raspi = require("raspi-io");
//var rpio = require("rpio");
//var gpioService = require("./gpioService.js");
var staticSiteCss = __dirname + '/public/css';
var staticSiteImg = __dirname + '/public/img';
//var swaggerUI = __dirname + '/public/swag-ui';
//var server = require('http').Server(app);
//var io = require('socket.io')(server);

app.use(express.static(__dirname + '/public'));

var ip = process.env.OPENSHIFT_NODEJS_IP || '127.0.0.1';
var port = process.env.OPENSHIFT_NODEJS_PORT || 3000;
app.listen(port, ip);


