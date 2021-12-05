# Read me

### Introduction
This Readme covers the steps I took to develop a simple wireless sensing system. In this instance
I am using a picking up vibrations, but it could be any physical signal.

The project utilises development kit in order to prove a concept and consists of a 9 axis bno055 
orientation sensor from Bosch, a Esp32 devkit2 in order to prepare the signals for MQTT and a 
Raspberry Pi to receive the data through a python program. An instance of InfluxDB and Grafana are
running on the pi. Grafana is accessible through Nginx as reverse proxy so that the values can be 
monitored from the internet.

Hopefully this readme is clear if not then messaage me we can work through any issues and update the 
readme.
 



#### 06/11/21

  ## Setup Raspberry pi with mosquitto broker
  
  run the following commands from the terminal in your pi
  
apt update+upgrade

	sudo apt-get update && sudo apt-get upgrade
      
      
 ##### check python3 version
	python3 --version
      
  ##### upgrade python version
	wget https://www.python.org/ftp/python/3.9.5/Python-3.9.5.tgz
	tar -zxvf Python-3.9.5.tgz
		
set this flag, it enables Profile guided optimization (PGO) and Link Time Optimization (LTO). It will slow down the build process but yield a 10-20% speed boost.
	cd Python-3.9.5
	./configure --enable-optimizations
	
 ##### If error
	sudo make altinstall
        
 ##### Install mosquitto and paho lib onto PI and configure
 (if continuing from above then cd out of python dir)
 
 install mosquitto and python
 
	sudo apt-get install -y mosquitto mosquitto-clients python3-pip

install paho python library

	python3 -m pip install paho-mqtt
	
nano into the follwoing
	
		sudo nano /etc/mosquitto/mosquitto.conf
		
append these lines to the end of the file

		listener 1883
		listener 9001
		protocol websockets
       
 #### check mosquitto is running
        sudo systemctl status mosquitto

#### Test Mosquitto is working

Stop the service 

	sudo systemctl stop mosquitto.service
				
start in verbose mode 

	mosquitto -v   #start in verbose mode
				
open 2 terminals in putty or on the raspbery Pi GUI and in one terminal type

	mosquitto_sub -d -v -t 'test/topic'

in the other
	
	mosquitto_pub -t 'test/topic' -m 'Helloworld'
				
The message should show on the subscriber.
	
##  Setup Publisher on Esp32

The ESP32 program is found in the main repository the flowchart below details the flow of the program

![alt text](https://github.com/hadwll/VBA_1_1axis_mqtt/blob/main/esp_flow.png?raw=true)


##Install InfluxDB

InfluxDB can be installed by directly querying the official repositories of this tool. First, add them to your list, by typing the following commands:

	sudo curl -sL https://repos.influxdata.com/influxdb.key | sudo apt-key add -

	sudo echo "deb https://repos.influxdata.com/ubuntu bionic stable" | sudo tee /etc/apt/sources.list.d/influxdb.list

	sudo apt update

Once the list of repositories has been updated, proceed with the installation of InfluxDB using the command:

	sudo apt install influxdb
	
after install enable it on startup

	sudo systemctl enable --now influxdb
	
and check that it is running
	
	sudo systemctl status influxdb
	
## Install Influx DB and python-influxdb

	 pip install influxdb
	 
	 sudo apt-get install python-influxdb
	 
## Install Grafana on the Raspberry Pi


To install the latest Grafana Enterprise edition:

	sudo apt-get install -y apt-transport-https
	sudo apt-get install -y software-properties-common wget
	wget -q -O - https://packages.grafana.com/gpg.key | sudo apt-key add -

Add this repository

	echo "deb https://packages.grafana.com/enterprise/deb stable main" | sudo tee -a /etc/apt/sources.list.d/grafana.list

After you add the repository:

	sudo apt-get update
	sudo apt-get install grafana-enterprise
	
check grafana is running  

	sudo systemctl status grafana-server
	
set the server to start at boot

	sudo systemctl enable grafana-server.service
	
navigate to your raspbery pi IP address and port 3000 in your web brower
	
	pi-ip:3000
	
	eg 192.168.2.8:3000
	
you should see the grafana spash screeen, you can login to by using admin/admin and change from there.


## Setup the data sources

go to configurations tab and then data sources

![alt text](https://github.com/hadwll/VBA_1_1axis_mqtt/blob/main/grafana.png?raw=true)	

configure your DB ip addrees to http://localhost:8086  if this is the case

enter the DB infromation in this case I am using example save and test, ensure it is working.

![alt text](https://github.com/hadwll/VBA_1_1axis_mqtt/blob/main/grafana2.png?raw=true)	

Select add and dashbaord, new panel

![alt text](https://github.com/hadwll/VBA_1_1axis_mqtt/blob/main/grafana3.png?raw=true)

setup the query for your dataset

![alt text](https://github.com/hadwll/VBA_1_1axis_mqtt/blob/main/grafana4.png?raw=true)



## setup mqtt broker and InfluxWritePoints on Raspberry Pi

The Python program can be found in the repo, the function that does the work is
the On_message function, it 

1.receives mqtt msg as byte array
2. converts to float
3.write float data point to influxDB

The remainder of the program is setting up clients for MQTT and Influx.

![alt text](https://github.com/hadwll/VBA_1_1axis_mqtt/blob/main/rpi_flow.png?raw=true)


## Configuring nginx as webserver and reverse proxy

nginx is a powerful fast webserver and reverse proxy, I will concentrate on the points required for our purpose. This installation 
was on the raspberypi. I did not have to alter the firewall but it may be neccessary in your case.

	sudo apt update
	sudo apt install nginx
	
check that nginx is running

	sudo systemctl status nginx


open a web browser and type your ip address. You should get the nginx welcome screen.

![alt text](https://github.com/hadwll/VBA_1_1axis_mqtt/blob/main/nginx.png?raw=true)

With that complete we need to point it to our grafana 

First is to setup a default index page, it  wont be used for this task, but it is a good skill to learn. We will
configure the reverse proxy in the next step.

	sudo mkdir -p /var/www/your_domain/html
	
set the permissons to the directories so that they allow the owner to read, write, and execute the files while 
granting only read and execute permissions to groups and others.

	sudo chmod -R 755 /var/www/your_domain
	
create a sample index.html page using nano or vim

	nano /var/www/your_domain/html/index.html
	
In order for nginx to to serve the webpage we need to create 2 server blocks. 
one to serve the main page and one to reverse proxy the subdomain to the grafana 
GUI.

navigate to 

	/etc/nginx/sites-available/your_domain
	
add the folllowing code with your details.
	
	#Server block to serve main page
	server {
	listen 80 default_server;
	listen [::]:80 default_server;
	root /var/www/yourdomain.net/;
	index index.html;
	server_name yourdomain.net www.yourdomain.net;
	location / {
		try_files $uri $uri/ =404;
	}
	}
	#Server block to serve port 3000 to the sub-domain
	server {
	listen 80;
	server_name yoursubdomain.yourdomain.net;
	location / {
		proxy_set_header Host $host;
		proxy_pass http://127.0.0.1:3000;
		proxy_redirect off;
	}
	}


enable the file by creating a link from it to the sites-enabled directory 

	sudo ln -s /etc/nginx/sites-available/your_domain /etc/nginx/sites-enabled/
	
You need to delete the default file from the sites-available and sites-enabled dir

	sudo rm default
 
finally check the configration of the nginx file

	sudo nginx -t
	
if its all good then resrart Nginx

	sudo systemctl restart nginx
	
navigate to the domain and check its all working.





## Configure the ports on your router to forward incoming traffic to the RaspberryPi

This is mostly going to be outside the scope of this guide, you need to consult your routers documnetation.
The outcome is going to be that your router receives the data from the sensor from the ESP32 on port 1883 on 
its external IP address and forwards it to the raspberry pi on your internal IP address on the same port number.


## Purchase the DNS 

Get yourself over to google and buy a domain name you need to setup the records to point to the IP of your RaspberryPi
setup a subdomain for the grafana dashbaord.

![alt text](https://github.com/hadwll/VBA_1_1axis_mqtt/blob/main/dns.PNG?raw=true)



## 02/12/21
