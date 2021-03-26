const express = require('express');
const app = express();
const mysql = require('mysql');
const morgan = require('morgan');
const util = require('util');
const bodyParser = require('body-parser');

console.log("Starting");

var pool = mysql.createPool({
    connectionLimit: 50,
    host: 'mysql1',
    user: 'root',
    password: 'secret',
    database: 'Humidity_data',
    timezone: 'utc'
});

app.use(morgan('short'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use((req, res, next) => {
    res.header('Access-Control-Allow-Origin', '*');
    next();
  });

// Base route
app.get("/api/", (req, res) => {
    console.log("Responding to root")
    res.send("Hello from route")
});

//-------------------------------- DATA ------------------------------------------

// Gets the latest data entry in database
app.get('/api/get_latest_data', function (req, res) {
    var queryString = "SELECT * FROM `humidity_data` ORDER BY `humidity_data`.`entry_id`  DESC LIMIT 1;";
    pool.getConnection(function(err, conn) {
        if (err) {
            res.send('Error occured');
            console.log(err);
        } else {
            conn.query(queryString, function (err2, records, fields) {
                if (!err2) {
                    res.send(records);
                }
                conn.release();
            });
        }
    });
});

// Gets all data
app.get('/api/get_all_data', function (req, res) {
    var queryString = "SELECT * FROM `humidity_data` ORDER BY `humidity_data`.`entry_id`  DESC;";
    pool.getConnection(function(err, conn) {
        if (err) {
            res.send('Error occured');
            console.log(err);
        } else {
            conn.query(queryString, function (err2, records, fields) {
                if (!err2) {
                    res.send(records);
                }
                conn.release();
            });
        }
    });
});

// Inserts entry into database
app.get('/api/update_data/:location/:temperature/:humidity', function (req, res) {
    var location = req.params.location;
    var temperature = req.params.temperature;
    var humidity = req.params.humidity;

    // console.log("location = " + location);
    // console.log("temperature = " + String(temperature));
    // console.log("humidity = " + String(humidity));

    var queryString = "INSERT INTO `humidity_data` (`entry_id`, `dateheure`, `location`, `temperature`, `humidity`) VALUES (NULL, CURRENT_TIMESTAMP, ?, ?, ?);";
    pool.getConnection(function(err, conn) {
        if (err) {
            res.send('Error occured');
            console.log(err);
        } else {
            conn.query(queryString, [location, temperature, humidity], function (err2, records, fields) {
                if (!err2) {
                    res.send(records);
                } else {
                    console.log(err2);
                }
                conn.release();
            });
        }
    });
});


// Promisify for Node.js async/await.
pool.query = util.promisify(pool.query);

module.exports = pool;

app.listen(8085, () => {
    console.log("Server is up and listening on 8085..");
});