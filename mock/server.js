const express = require('express')
const app = express()

const APP_PORT = 3001;

app.use((req, res, next) => {
  console.log(req.url)
  next()
})

app.get('/test', (req, res) => {
  const id = req.query.id;
  res.json({ someTestField: 'Hello World! ' + id })
})

// reqs per minute
app.get('/query', (req, res) => {
  const query = req.query.query;
  let result = {};
  // Instances
  if (query === 'count(up{job="analytics"} == 1) by (continent)') {
    result = {
      status: "success",
      data: {
        resultType: "vector",
        result: [
          {
            metric: {
              continent: "europe"
            },
            value: [
              1534777117.45,
              "2"
            ]
          },
          {
            metric: {
              continent: "us"
            },
            value: [
              1534777117.45,
              "3"
            ]
          }
        ]
      }
    }
    // CPU EU
  } else if (query === 'avg(node_load1{name=~".*ingress.*", continent="europe"})') {
    result = {
      status: "success",
      data: {
        resultType: "vector",
        result: [
          {
            metric: {},
            value: [
              1534777119.906,
              "0.2"
            ]
          }
        ]
      }
    };
    // CPU US
  } else if (query === 'avg(node_load1{name=~".*ingress.*", continent="us"})') {
    result = {
      status: "success",
      data: {
        resultType: "vector",
        result: [
          {
            metric: {},
            value: [
              1534777135.458,
              "0.45999999999999996"
            ]
          }
        ]
      }
    };
    // Requests by continent
  } else if (query === 'sum(rate(requests_total[1m])) by (continent)') {
    result = {
      status: "success",
      data: {
        resultType: "vector",
        result: [
          {
            metric: {
              continent: "europe"
            },
            value: [
              1534777133.752,
              "147.51785714285714"
            ]
          },
          {
            metric: {
              continent: "us"
            },
            value: [
              1534777133.752,
              "300.359693877551"
            ]
          }
        ]
      }
    };
  } else {
    result = { youAreOnTheRightPath: true }
  }
  res.json(result)
})

app.listen(APP_PORT, () => {
  console.log('Listening on Port ' + APP_PORT);
})
