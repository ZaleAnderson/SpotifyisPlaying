#include <WiFi.h>
#include <SpotifyArduino.h>
#include <SpotifyArduinoCert.h>
#include <WiFiClientSecure.h>

// WiFi credentials
const char* ssid = "Z.A. Studios @ 2.4GHz";
const char* password = "lastkiss";

// Spotify credentials
const char* clientId = "d580f87b9d654363bcb1a738ae2c0f72";
const char* clientSecret = "9d77c62cdec9411fbce66856ef92b8dc";

#define SPOTIFY_REFRESH_TOKEN "AQAdgT0M223U_W-eYqWrm3n7mmwAYITYuOrGFWGA1eIQqokIdZ7GmnggFPhTnque4AOuuKpajpr4HlslnKVRJbpI2RkCE1ZB3bfKzjA48lWpKorqllrfzeu4qXy7MjjLWpI"

#define SPOTIFY_MARKET "US"

//PWM
int pwmChannel = 0; // Selects channel 0
int frequence = 1000; // PWM frequency of 1 KHz
int resolution = 8; // 8-bit resolution, 256 possible values
int pwmPin = 6;
int i = 0;
int lighton = 0;

WiFiClientSecure client;
SpotifyArduino spotify(client, clientId, clientSecret, SPOTIFY_REFRESH_TOKEN);

unsigned long delayBetweenRequests = 30000; // Time between requests (1 minute)
unsigned long requestDueTime;               //time when request due

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  //PWM
  ledcSetup(pwmChannel, frequence, resolution);
  ledcAttachPin(pwmPin, pwmChannel);
  ledcWrite(pwmChannel, 0);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to WiFi...");
    delay(1000);
  }
  Serial.println("Connected to WiFi");

  client.setCACert(spotify_server_cert);

  Serial.println("Refreshing Access Tokens");
  if (!spotify.refreshAccessToken())
  {
    Serial.println("Failed to get access tokens");
  }
}

void ledON () {
  if (lighton == 0) {
    for (int i = 0; i <= 255; i++) {
      ledcWrite(pwmChannel, i);
      delay(2500 / 256);
      // ^ How many seconds (1.5)
    }
    lighton = 1;
  }
}

void ledOFF () {
  if (lighton == 1) {
    for (int i = 256; i >= 0; i--) {
      ledcWrite(pwmChannel, i);
      delay(2500 / 256);
    }
  }
  lighton = 0;
}


void printCurrentlyPlayingToSerial(CurrentlyPlaying currentlyPlaying)
{
  // Use the details in this method or if you want to store them
  // make sure you copy them (using something like strncpy)
  // const char* artist =

  Serial.println("--------- Currently Playing ---------");

  Serial.print("Is Playing: ");
  if (currentlyPlaying.isPlaying)
  {
    Serial.println("Yes");
    ledON();
  }
  else
  {
    Serial.println("No");
    ledOFF();
  }

  Serial.print("Track: ");
  Serial.println(currentlyPlaying.trackName);
  Serial.println("-------------------------------------");
}



void loop()
{
  if (millis() > requestDueTime)
  {
    // Market can be excluded if you want e.g. spotify.getCurrentlyPlaying()
    int status = spotify.getCurrentlyPlaying(printCurrentlyPlayingToSerial, SPOTIFY_MARKET);
    if (status == 200)
    {
      Serial.println("Got music status");
    }
    else if (status == 204)
    {
      Serial.println("Failed to get music status");
    }
    else
    {
      Serial.print("Error: ");
      Serial.println(status);
    }
    requestDueTime = millis() + delayBetweenRequests;
  }
}
