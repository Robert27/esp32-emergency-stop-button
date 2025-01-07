# Use an official Python runtime as a parent image
FROM python:3.13-slim

# Set the working directory in the container
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . /app

# Install any needed packages specified in requirements.txt
RUN pip install --no-cache-dir -r requirements.txt

# Make port 1883 available to the world outside this container
EXPOSE 1883

# Define environment variables
ENV MQTT_BROKER=${MQTT_BROKER}
ENV MQTT_PORT=${MQTT_PORT}
ENV MQTT_TOPIC=${MQTT_TOPIC}
ENV MQTT_USERNAME=${MQTT_USERNAME}
ENV MQTT_PASSWORD=${MQTT_PASSWORD}
ENV TELEGRAM_TOKEN=${TELEGRAM_TOKEN}
ENV CHAT_ID=${CHAT_ID}

# Run main.py when the container launches
CMD ["python", "main.py"]
