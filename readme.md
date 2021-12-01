# Read me

### Introduction
This Readme covers the steps I took to develop a simple wireless sensing system. In this instance
I am using a picking up vibrations, but it could be any physical signal.

The project utilises development kit in order to prove a concept and consists of a 9 axis bno055 
orientation sensor from Bosch, a Esp32 devkit2 in order to prepare the signals for MQTT and a 
Raspberry Pi to receive the data through a python program. An instance of InfluxDB and Grafana are
running on the pi. Grafana is accessible through Nginx as reverse proxy so that the values can be 
monitored.

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
        cd Python-3.9.5
        ./configure --enable-optimizations
	
 ##### If error
        sudo make altinstall
        
 ##### Install mosquitto onto Pi
 (if continuing from above then cd out of python dir)
 import the repository package signing key
 
        wget http://repo.mosquitto.org/debian/mosquitto-repo.gpg.key
        sudo apt-key add mosquitto-repo.gpg.key
		sudo apt-get install mosquitto
       
 #### check mosquitto is running
        sudo systemctl status mosquitto

If you need to change the configuration then the configuration file is called mosquitto.conf and is located in the /etc/mosquitto folder.

#### Test Mosquitto is working

Stop the service 

	sudo systemctl stop mosquitto.service
				
start in verbose mode 

	mosquitto -v   #start in verbose mode
				
open 2 terminals in putty or on the raspbery Pi GUI and in one terminal type

	mosquitto_sub -h localhost -t test -d

in the other
	
	mosquitto_pub -h localhost -t test -m message
				
The message should show on the subscriber.
        
  
## setup pubsubclient on esp

The ESP32 program is found in the main repository the flowchart below details the flow of the program

![alt text](https://github.com/hadwll/VBA_1_1axis_mqtt/blob/main/esp_flow.png?raw=true)


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


###setup the data sources

go to configurations tab and then data sources

![alt text](https://github.com/hadwll/VBA_1_1axis_mqtt/blob/main/grafana1.png?raw=true)	


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
3.write float data point to influx

The remainder of the program is setting up clients for MQTT and Influx.

![alt text](https://github.com/hadwll/VBA_1_1axis_mqtt/blob/main/rpi_flow.png?raw=true)


## Configuring nginx as webserver and reverse proxy

nginx is a powerful fast webserver I will concentrate on the points required for our purpose. This installation 
was on the raspberypi. I did not have to alter the firewall but it may be neccessary in your case.

	sudo apt update
	sudo apt install nginx
	
check that nginx is running

	sudo systemctl status nginx


open a web browser and type your ip address. You should get the nginx welcome screen.
With that complete we need to point it to our grafana 

First is to setup a default index page, it likely wont be used for this task, but it is a good skill to learn. We will
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
	
	#Server block to serve page
	server {
	listen 80 default_server;
	listen [::]:80 default_server;
	root /var/www/integrated-vision.net;
	index index.html;
	server_name integrated-vision.net www.integrated-vision.net;
	location / {
		try_files $uri $uri/ =404;
	}
	}
	#Server block to serve the sub domain
	server {
	listen 80;
	server_name dashboard.integrated-vision.net;
	location / {
		proxy_set_header Host $host;
		proxy_pass http://127.0.0.1:3000;
		proxy_redirect off;
	}
	}


enable the file by creating a link from it to the sites-enabled directory 

	sudo ln -s /etc/nginx/sites-available/your_domain /etc/nginx/sites-enabled/
 
finally check the configration of the nginx file

	sudo nginx -t
	
if its all good then resrart Nginx

	sudo systemctl restart nginx
	
navigate to the domain and check its all working.





## Configure the ports on your router to forward incoming traffic to the RaspberryPi

This is mostly going to be outside the scope of this guide, you need to consult your routers documnetation.
The outcome is going to be that your router receives the data from the sensor on its external IP address and 
forwards it to the raspberry pi on your internal IP address.


## Purchase the DNS 

Get yourself over to google and buy a domain name you need to setup the records to point to the IP of your RaspberryPi

![alt text](https://github.com/hadwll/VBA_1_1axis_mqtt/blob/main/dns.png?raw=true)

