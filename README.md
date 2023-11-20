![Lily](https://github.com/satria-mitra/Plant-Monitor/blob/main/resources/image/lily.jpg)

Say Hi to Lily!

# Lily - Plant Monitoring project
This repository is CASA0014 Connected Environment project for plant monitor, workshop no.2. This github will be updated regularly to records what are the challanges and problems faces during the development

# Overview
This project is based on [CASA CE Plant Monitor](https://github.com/ucl-casa-ce/casa0014/tree/main/plantMonitor) that could monitor Lily conditions such as temperature, humidity and soil mosture, send this data to MQTT CE server via wifi and display live data via webserver and from Grafana. From this MQTT data, I can know that she's okay in the lab in realtime.

However, she sometimes feel alone and worry about me if I'm not around. So I want to add a way of interaction between me and Lily. I add a display so I can send messages to her while we are separated. Adding display making the interactions between me and Lily borderless.

Eventhough Lily is a plant, she is still a creature, we just do not know how to interact and communicate with them. So I want to develop a way to make interaction between human and a plant limitless.

## Changelog

Current version V1.1.0
- Subscribing to topic `student/CASA0014/ucfnmut/forLily` and sending the messages for Lily and displaying on display
- Add text input at web server for sending message to Lily (you need to connec to CE Hub Wifi)

### Version 1.1.0
Added :
- Add text input at web server (IP 10.129.125.198 in CASA CE Network) to send message to Lily



## Prerequisites
Before using this code, make sure you have the following prerequisites:
- An ESP8266 microcontroller.
- DHT 22 sensor
- Two nails and two cables as soil mosture sensors
- Two pieces of 10K Ohm resistors
- A 200K Ohm resistor and a 100 Ohm resistors
- Breadboard for prototyping
- Libraries installed for ESP8266, MQTT, and OLED display (see below)
- Sensitive data (such as Wi-Fi network credentials and MQTT server details) and in a file named `arduino_secrets.h`.

If you want to see the original project, [Plant Monitor project from Duncan Wilson](https://github.com/ucl-casa-ce/casa0014/tree/main/plantMonitor) explain in depth about this project

## Libraries Used

The code includes the following libraries:

- [ESP8266WiFi](https://github.com/esp8266/Arduino): For ESP8266 Wi-Fi functionality.
- [PubSubClient](https://pubsubclient.knolleary.net): For MQTT communication.
- [ssd1306.h](https://www.arduino.cc/reference/en/libraries/ssd1306/): For controlling an OLED display.


# Building process

The original project was built using Plant Monitor Shield V2 from [Plant Monitor](https://github.com/ucl-casa-ce/casa0014/tree/main/plantMonitor) and mainly built during CE Wokrshop in the class. For prototyping process, I use breadboard rather than using the original Shield

![Prototyping process](https://github.com/satria-mitra/Plant-Monitor/blob/main/resources/image/prototyping_process.jpg)

Image: Prototyping process with LCD added

## Schematis and paper design
Started with thinking of what features that I want to add to plant monitor, I ended up with adding OLED Display to the original projects. Apart from the original schematics (which can be seen in [Duncan's Plant monitor projects](https://github.com/ucl-casa-ce/casa0014/tree/main/plantMonitor), I added schematics for OLED Display. The sketch below shows the design of the circuit :

![Schematics](https://github.com/satria-mitra/Plant-Monitor/blob/main/resources/image/schematiscs.jpg)

Image: Schematics on paper, red dotted line shows original project, blue dotted line shows additional OLED Display 

## V 1.0 explanation

I add an oled display so I can send messages to Lily and display it on. Try to say hello to Lily by sending the message to topic `student/CASA0014/ucfnmut/forLily`. SubscribeToTopic() function is called inside the loop function


# Uploading the data to Internet

- I  use MQTT protocols to send the data to the CE server. The data (Temperature, humidity, and soil mosture) was sent to the topic `student/CASA0014/plant/ucfnmut/inTopic`.

 ![mqttdata](https://github.com/satria-mitra/Plant-Monitor/blob/main/resources/image/mqttexplorer.jpg)

 Image: Data on MQTT Explorer 


## Setting up the Rasp PI as a gateway and storing data
I use Rasp PI as a gateway. The whole process is long enough, but it could be read from the [CE Workshop Plant Monitor](https://workshops.cetools.org/codelabs/CASA0014-2-Plant-Monitor/#0). In short, the process include :
- Setting up Raspberry Pi as a gateway
- Install InfluxDB, Telegraf, Grafana in Raspberry PI
- Configure InfluxDB and Telegraf to capture the data from MQTT topics.
- Create a dashboard in Grafana to visualize the data

![dashboard](https://github.com/satria-mitra/Plant-Monitor/blob/main/resources/image/influxdb.jpg)
Image: InfluxDB shows data from Raspberry PI


## Visualizing the data in Grafana

Grafana could be used as a dashboard to display the data. In order to do this, in Grafana we create a new dashboard and copy this code to the query to the query panel on the bottom. Again, this code can be seen on [CE Workshop Plant Monitor](https://workshops.cetools.org/codelabs/CASA0014-2-Plant-Monitor/#0)

![dashboard](https://github.com/satria-mitra/Plant-Monitor/blob/main/resources/image/grafana.png)

Image: Grafana Dashboard showing data from Lily


# Results

This is the final project looks like that I build during my workshop at CASA. I hope to make this even better in the future

 ![final](https://github.com/satria-mitra/Plant-Monitor/blob/main/resources/image/final1.jpg)

 
 ![How this V.1 looks like](https://github.com/satria-mitra/Plant-Monitor/blob/main/resources/image/final2.jpg)

If you are around CASA Lab, and connected to CE wifi, you can send message to Lily by scan qrcode below and input your message on text box.

  ![QRCode](https://github.com/satria-mitra/Plant-Monitor/blob/main/resources/image/qrcode.png)
|  ![Web server](https://github.com/satria-mitra/Plant-Monitor/blob/main/resources/image/webserver.png)



# Challange and lesson learned during development process
- ESP8266 only works in `CE-Hub-Student` network
- Try to see data from a large big picture, so it can show you the trend of data and see what's going on
- Explore data from another perspective, e.g using another type of soil mosture ensor, put it deep or wide from each probe
- It is always easier to access Raspberry Pi using ethernet cable, because you dont have to set up different SSID and password for each wifi hotspot
- There is no offline store in London that sell you Arduino component, so try not to procastinate your project untill the final time

# Future Improvement

I also want to implement improvement in the future if I have a chance to upgrade this project. Some cool improvements that came into my mind were :

- Add an led as a sign of incoming message
- Prototype my own pcb board so it can fit bigger oled display
- [Sonification project](https://www.instructables.com/Biodata-Sonification/) using sound to illustrate how a Plant could singing :)
- [RGB Audio visualizer](https://projecthub.arduino.cc/janux/rgb-32-band-audio-spectrum-visualizer-2f4788) also would be a perfect companion for that sonification idea

