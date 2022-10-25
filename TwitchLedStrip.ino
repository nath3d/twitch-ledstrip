#include <Arduino.h>
#include <Esp.h>
#include <ESP8266WiFi.h>
#include <IRCClient.h>
#include <StreamlabsAPI.h>
#define IRC_SERVER   "irc.chat.twitch.tv"
#define IRC_PORT     6667

//BASTIUI
#define SSID "AGRID"
#define PASSWORD "pass"
#define TWITCH_BOT_NAME "agrid"
#define TWITCH_OAUTH_TOKEN "token"
#define SOCKET_TOKEN "token"
const String twitchChannelName = "bastiui";

String ircChannel = "";
String bastiuiNickName = "BASTIUI";
String lunathNickName = "LUNATH_";
String stringToFindEmote = "bstiDSGN";
String stringToFindCheer1 = "Cheer1";
String stringToFindCheer5 = "Cheer5";

//brutal method
String streamlabsNickName = "STREAMLABS";
String stringToFindBits = "bits";
String stringToFindSub = "BastiPote";
String stringToFindRaid = "raid";
//brutal method

int blinkSpeed = 100;
int blinkCount = 6;
int relay1ID = 2;
StreamlabsAPI streamlabsAPI;

WiFiClient wiFiClient;
IRCClient client(IRC_SERVER, IRC_PORT, wiFiClient);

void setup()
{
  Serial.begin(9600);
  delay(1000);
  //RELAYS
  pinMode(relay1ID, OUTPUT);
  helloStrip();
  //WIFI
  WiFi.begin(SSID, PASSWORD);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }
  Serial.println('\n');
  Serial.println("Wifi connection established.");
  Serial.print("IP address\t");
  Serial.println(WiFi.localIP());
  //IRC
  ircChannel = "#" + twitchChannelName;
  client.setCallback(callback);
  //API
  //streamlabsAPI.followTwitchEvent(followerEvent);
  //streamlabsAPI.subscriptionsTwitchEvent(subscriptionEvent);
  //streamlabsAPI.bitsTwitchEvent(bitsEvent);
  //streamlabsAPI.donationEvent(donationsEvent);
  //streamlabsAPI.connect(SOCKET_TOKEN);
  //END
  helloStrip();
}

void loop()
{
  //blinkStrip();
  //IRC
  if (!client.connected()) {
    Serial.println("Attempting to connect to IRC channel " + ircChannel );
    if (client.connect(TWITCH_BOT_NAME, "", TWITCH_OAUTH_TOKEN))
    {
      client.sendRaw("JOIN " + ircChannel);
      Serial.println("IRC connected.");
      sendTwitchMessage(stringToFindEmote);
    }
    else
    {
      Serial.println("IRC connection failed.");
      delay(5000);
    }
    return;
  }
  client.loop();
  //API
  streamlabsAPI.loop();
}


/////////////////////////////////////////////////////IRC

void sendTwitchMessage(String message)
{
  client.sendMessage(ircChannel, message);
}

void callback(IRCMessage ircMessage)
{
  Serial.println("ORIGINAL IRC MESSAGE " + ircMessage.original);
  if (ircMessage.command == "PRIVMSG" && ircMessage.text[0] != '\001')
  {
    ircMessage.nick.toUpperCase();
    String message("<" + ircMessage.nick + "> " + ircMessage.text);
    Serial.println(message);
    if (ircMessage.nick.indexOf(bastiuiNickName) >= 0)
    {
      if (ircMessage.text.indexOf(stringToFindEmote) >= 0)
        blinkStrip();
    }
    if (ircMessage.nick.indexOf(lunathNickName) >= 0)
    {
      if (ircMessage.text.indexOf(stringToFindEmote) >= 0)
        blinkStrip();
    }
    if (ircMessage.nick.indexOf(streamlabsNickName) >= 0)
    {
      if (ircMessage.text.indexOf(stringToFindBits) >= 0)
        bitsStrip(ircMessage.text);
      if (ircMessage.text.indexOf(stringToFindSub) >= 0)
        subStrip(ircMessage.text);
      if (ircMessage.text.indexOf(stringToFindRaid) >= 0)
        raidStrip(ircMessage.text);
    }
    //    else
    //    {
    //      if (ircMessage.text.indexOf(stringToFindCheer1) >= 0)
    //        blinkStrip();
    //      if (ircMessage.text.indexOf(stringToFindCheer5) >= 0)
    //        blinkStrip();
    //    }
    return;
  }
}

void subStrip(String message)
{
  String subMessage = message.substring(message.indexOf('(') + 1, message.length());
  int monthsCount =  subMessage.substring(0, subMessage.indexOf(' ')).toInt();
  if (monthsCount == 0)
    monthsCount = blinkCount;
  for (int i = 0; i < monthsCount; i++)
  {
    hardStrip();
    delay(blinkSpeed);
    noStrip();
    delay(blinkSpeed);
  }
  softStrip();
}

void bitsStrip(String message)
{
  int bitsCount = message.substring(0, message.indexOf(' ')).toInt();
  if(bitsCount > 10)
    bitsCount = bitsCount /10;
  else if(bitsCount >= 10000)
    bitsCount = bitsCount /100;
    
  for (int i = 0; i < bitsCount; i++)
  {
    hardStrip();
    delay(blinkSpeed);
    noStrip();
    delay(blinkSpeed);
  }
  softStrip();
}

void raidStrip(String message)
{
  int raidersCount = message.substring(0, message.indexOf(' ')).toInt();
  for (int i = 0; i < raidersCount; i++)
  {
    hardStrip();
    delay(blinkSpeed);
    noStrip();
    delay(blinkSpeed);
  }
  softStrip();
}

void blinkStrip()
{
  for (int i = 0; i < blinkCount; i++)
  {
    hardStrip();
    delay(blinkSpeed);
    noStrip();
    delay(blinkSpeed);
  }
  softStrip();
}

void helloStrip()
{
  for (int i = 0; i < 2; i++)
  {
    hardStrip();
    delay(blinkSpeed);
    noStrip();
    delay(blinkSpeed);
  }
  softStrip();
}

void softStrip()
{
  digitalWrite(relay1ID, HIGH);
}

void hardStrip()
{
  digitalWrite(relay1ID, LOW);
}

void noStrip()
{
  digitalWrite(relay1ID, LOW);
}

/////////////////////////////////////////////////////API
void followerEvent(const char * payload)
{
  Serial.print("got followerEvent message: ");
  Serial.println(payload);
  //blinkStrip();
}

void subscriptionEvent(const char * payload)
{
  Serial.print("got subscriptionEvent message: ");
  Serial.println(payload);
  blinkStrip();
}

void bitsEvent(const char * payload)
{
  Serial.print("got bitsEvent message: ");
  Serial.println(payload);
  blinkStrip();
}

void donationsEvent(const char * payload)
{
  Serial.print("got donationsEvent message: ");
  Serial.println(payload);
  blinkStrip();
}
