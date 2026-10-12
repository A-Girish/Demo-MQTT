import threading
import time
from collections import deque

import paho.mqtt.client as mqtt
from flask import Flask, jsonify, render_template

MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883
MQTT_TOPIC = "machinemonitoring/ldr"

MAX_POINTS = 200

app = Flask(__name__)

readings_lock = threading.Lock()
readings = deque(maxlen=MAX_POINTS)


def on_connect(client, userdata, flags, reason_code, properties=None):
    print(f"Connected with result code {reason_code}")
    client.subscribe(MQTT_TOPIC)


def on_message(client, userdata, msg):
    try:
        value = int(msg.payload.decode())
    except ValueError:
        return
    with readings_lock:
        readings.append({"t": time.time(), "value": value})


def start_mqtt():
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    client.loop_forever()


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/data")
def data():
    with readings_lock:
        return jsonify(list(readings))


if __name__ == "__main__":
    mqtt_thread = threading.Thread(target=start_mqtt, daemon=True)
    mqtt_thread.start()
    app.run(host="127.0.0.1", port=5000, debug=False)
