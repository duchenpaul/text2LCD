# text2LCD
Show strings on the screen LCD2004 from API post

## Usage
Send the post data to `http:<esp8266_ip>/api`. 
Data example:
```json
{
    "LINE_1_TEXT": "LINE_1_TEXT",
    "LINE_2_TEXT": "LINE_2_TEXT",
    "LINE_3_TEXT": "LINE_3_TEXT",
    "LINE_4_TEXT": "LINE_4_TEXT",
    "BACKLIGHT": true
}
```