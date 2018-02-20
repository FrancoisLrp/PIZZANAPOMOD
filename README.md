# PIZZANAPOMOD
Mod four spice diavola connecté : http://pizzanapo.fr/index.php?/topic/180-projet-en-cours-spice-diavola-connecté/&page=1

Vidéo de présentation de l'interface : https://www.youtube.com/watch?v=XZTqRmCiPII

Schéma electrique : http://www.zupimages.net/viewer.php?id=18/08/7h6o.png (attention les MAX6675 s'alimentent avec le 3.3V de l'esp et non le 5V comme sur le schéma).

Si utilisation de deux thermocouples (non testé, pas certain que l'esp8266 gère deux interfaces SPI) :
Placez le fichier dynamique.html dans un dossier "data" situé au même niveau que le .ino
Upload du html via SPIFFS : http://www.instructables.com/id/Using-ESP8266-SPIFFS/

Si utilisation d'un seul thermocouple (sous la pierre) :
Renommer le fichier  dynamique_1tc.html en  dynamique.html.
Placez le fichier dynamique.html dans un dossier "data" situé au même niveau que le .ino
Upload du html via SPIFFS : http://www.instructables.com/id/Using-ESP8266-SPIFFS/

Merci @J-M-L de arduino.cc pour son tuto sur les techniques avancées de serveur web : https://forum.arduino.cc/index.php?topic=511147.0
                                                                                                                             
