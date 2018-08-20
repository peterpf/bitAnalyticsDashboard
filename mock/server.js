const express = require('express')
const app = express()

app.use((request, response, next) => {
  console.log(request.headers)
  next()
})

app.use((request, response, next) => {
  request.chance = Math.random()
  next()
})

// Instances
app.get('/query?query=count(up{job="analytics"} == 1) by (continent)', (request, response) => {
  response.json({
    "status": "success",
    "data": {
      "resultType": "vector",
      "result": [
        {
          "metric": {
            "continent": "europe"
          },
          "value": [
            1534777117.45,
            "2"
          ]
        },
        {
          "metric": {
            "continent": "us"
          },
          "value": [
            1534777117.45,
            "3"
          ]
        }
      ]
    }
  })
})

// CPU EU
app.get('/query?query=avg(node_load1{name=~".*ingress.*", continent="europe"})', (request, response) => {
  response.json({
    "status": "success",
    "data": {
      "resultType": "vector",
      "result": [
        {
          "metric": {},
          "value": [
            1534777119.906,
            "0.2"
          ]
        }
      ]
    }
  })
})
// CPU US
app.get('/query?query=avg(node_load1{name=~".*ingress.*", continent="us"})', (request, response) => {
  response.json({
    "status": "success",
    "data": {
      "resultType": "vector",
      "result": [
        {
          "metric": {},
          "value": [
            1534777135.458,
            "0.45999999999999996"
          ]
        }
      ]
    }
  })
})
// Requests per minute
app.get('/query?query=sum(rate(requests_total[1m]))', (request, response) => {
  response.json({
    "status": "success",
    "data": {
      "resultType": "vector",
      "result": [
        {
          "metric": {},
          "value": [
            1534777131.995,
            "448.36734693877554"
          ]
        }
      ]
    }
  })
})
// Requests per minute by country
app.get('/query=sum(rate(requests_total[1m])) by (continent)', (request, response) => {
  response.json({
    "status": "success",
    "data": {
      "resultType": "vector",
      "result": [
        {
          "metric": {
            "continent": "europe"
          },
          "value": [
            1534777133.752,
            "147.51785714285714"
          ]
        },
        {
          "metric": {
            "continent": "us"
          },
          "value": [
            1534777133.752,
            "300.359693877551"
          ]
        }
      ]
    }
  })
})

app.listen(3000)
