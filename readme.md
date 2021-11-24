# Read me

### Introduction
This Readme covers the steps I took to develop a simple wireless vibration monitoring system. 
It utilises development kit in order to prove a concept. A 9 axis bno055 oreinatiton sensor 
from bosch, a Esp32 devkit2 in order to prepare the signals for MQTT and a Raspberry Pi to 
receive the data through a python program.
 




#### 06/11 

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
	
	192.168.1.10:3000
	
you should see the grafana spash screeen, you can login to bu admin/admin and change from there.


###setup the data sources

go to configurations and then data sources

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
