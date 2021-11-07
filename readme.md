# Read me

### Introduction
{spam}




#### 06/11 

  ## Setup Raspberry pi with mosquitto broker
  
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
				
start is verbose mode 
				mosquitto -v   #start in verbose mode
				
open 2 terminals in pi or putty and in one
				mosquitto_sub -h localhost -t test -d

in the other
				mosquitto_pub -h localhost -t test -m message
        
  
