import paho.mqtt.client as mqtt
from paho.mqtt.client import CallbackAPIVersion
from telegram import Bot
import asyncio
import os

# Environment Variables
MQTT_BROKER = os.getenv("MQTT_BROKER")
MQTT_PORT = int(os.getenv("MQTT_PORT"))
MQTT_TOPIC = os.getenv("MQTT_TOPIC")
MQTT_USERNAME = os.getenv("MQTT_USERNAME")
MQTT_PASSWORD = os.getenv("MQTT_PASSWORD")

TELEGRAM_TOKEN = os.getenv("TELEGRAM_TOKEN")
CHAT_ID = os.getenv("CHAT_ID")

# Initialize Telegram Bot
bot = Bot(token=TELEGRAM_TOKEN)

# Create and set up asyncio event loop
loop = asyncio.new_event_loop()
asyncio.set_event_loop(loop)


async def send_telegram_message(message):
    """
    Sends a message to the specified Telegram chat asynchronously.
    """
    await bot.send_message(chat_id=CHAT_ID, text=message)


async def process_message(topic, message):
    """
    Processes the received MQTT message and sends a Telegram notification.
    """
    try:
        if message == "1":
            await send_telegram_message("Emergency stop button pressed! 🚨")
        elif message == "L":
            await send_telegram_message("Emergency stop button released! ✅")
        else:
            await send_telegram_message(
                f"New MQTT Message\nTopic: {topic}\nMessage: {message}"
            )
    except Exception as e:
        print(f"Error processing message: {e}")


def on_connect(client, userdata, flags, rc, properties=None):
    """
    MQTT on_connect callback - subscribes to the specified topic.
    """
    print("Connected to MQTT Broker with result code " + str(rc))
    client.subscribe(MQTT_TOPIC)


def on_message(client, userdata, msg):
    """
    MQTT on_message callback - schedules message processing.
    """
    try:
        message = msg.payload.decode("utf-8")
        topic = msg.topic
        print(f"Received message: {message} on topic: {topic}")
        # Schedule the async processing task
        asyncio.run_coroutine_threadsafe(process_message(topic, message), loop)
    except Exception as e:
        print(f"Error handling message: {e}")


async def mqtt_loop():
    """
    Asynchronous MQTT client loop.
    """
    client = mqtt.Client(CallbackAPIVersion.VERSION2)  # Use updated API version
    client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    client.on_connect = on_connect
    client.on_message = on_message

    client.enable_logger()

    try:
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
    except Exception as e:
        print(f"Error connecting to MQTT Broker: {e}")

    client.loop_start()
    while True:  # Keep the event loop running
        await asyncio.sleep(1)


if __name__ == "__main__":
    try:
        # Run MQTT loop asynchronously
        loop.run_until_complete(mqtt_loop())
    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        loop.close()
