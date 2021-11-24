import paho.mqtt.client as mqtt 
from influxdb import InfluxDBClient

# The callback for when the client connects to the broker
def on_connect(client, userdata, flags, rc):
    # Print result of connection attempt
	print(f"Connected with result code {rc}")
    # Subscribe to the topic “node1-sd”, receive any messages published on it
	client.subscribe("node1-sd")  

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    # Print a received msg
	print(f"Message received-> {msg.topic} {msg.payload}")
    # set the sensor value to local tag
	tag = float(msg.payload)

	json_body =[
		{
			"measurement": "Xvibration", 
			"tags": { 
				"Sensor_Node": "01"
			},
			# "time":"",
			"fields": { 
				"value": tag
			}
		}
	]
    
	client2.write_points(json_body) 


client2 = InfluxDBClient('localhost', 8086, 'root', 'root', 'example') 
client2.create_database('example') 

client = mqtt.Client("raspberrypi_client") # Create instance of client with client ID “digi_mqtt_test”
client.on_connect = on_connect # Define callback function for successful connection
client.on_message = on_message # Define callback function for receipt of a message 
client.connect('192.168.1.10', 1883) 
#result = client2.query('select value from cpu_load_short;') 
#print("Result: {0}".format(result))
client.loop_forever()  # Start networking daemon
