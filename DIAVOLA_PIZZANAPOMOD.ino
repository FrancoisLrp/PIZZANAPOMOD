#include "FS.h" // pour le SPIFFS
#include <ESP8266WiFi.h>
#include <max6675.h>
const char * nomDeFichier = "/dynamique.html";

int ktcSO = 12;//D6
int ktcCS = 13;//D7
int ktcCLK = 14;//D5
int ktcSO2 = 15;//D8 pas sûr sinon essayer autres pin digital
int ktcCS2 = 4;//D2 pas sûr sinon essayer autres pin digital
int ktcCLK2 = 5;//D1 pas sûr sinon essayer autres pin digital
int etatgeneral=0;
MAX6675 ktc(ktcCLK, ktcCS, ktcSO);
const char* ssid = "Hotspot";
const char* password = "12345678910";
int consignePierre = 500; // consigne de la regulation de la pierre 500°C
int modeCuisson=0;
int temppierre=0;//0
int tempchambre=0;
const uint16_t HTTPPort = 80;
WiFiServer serveurWeb(HTTPPort); // crée un serveur sur le port HTTP standard

const byte maxHTTPLine = 100;
char httpLine[maxHTTPLine + 1]; // +1 pour avoir la place du '\0'

const byte maxURL = 50;
char urlRequest[maxURL + 1]; // +1 pour avoir la place du '\0'

void algofour()
{
  
  temppierre=ktc.readCelsius();
  tempchambre=0;//ktc2.readCelsius();
  // Serial.println(temppierre);
if (modeCuisson == 1 && etatgeneral==1){
  //on active le relais de la resistance supérieure
    //on desactive le relais de la resistance inférieure 
      digitalWrite(D4, LOW);   // Turn the LED on (Note that LOW is the voltage level 
      digitalWrite(D3, HIGH); 
     Serial.println("en mode cuisson");
  }
  else{
  if(temppierre < (consignePierre-5) && etatgeneral==1) {
    //on active le relais de la resistance supérieure
    //on active le relais de la resistance inférieure
     if(digitalRead (D4) == LOW)
   digitalWrite(D4, HIGH);   // Turn the LED on (Note that LOW is the voltage level 
   if(digitalRead (D3) == LOW)
   digitalWrite(D3, HIGH);
   
  }
    
  if(temppierre >= (consignePierre+5) && etatgeneral==1) {
    //on desactive le relais de la resistance supérieure
    //on desactive le relais de la resistance inférieure
   if(digitalRead (D4) == HIGH)
   digitalWrite(D4, LOW);   // Turn the LED on (Note that LOW is the voltage level 
   if(digitalRead (D3) == HIGH)
   digitalWrite(D3, LOW);
     // Serial.println("tout coupé"); 
  }
}
if (etatgeneral==0){
   digitalWrite(D3, LOW);  //on desactive le relais de la resistance supérieure
    digitalWrite(D4, LOW);//on desactive le relais de la resistance inférieure
}
}
void printHTTPServerInfo()
{
  Serial.print(F("Site web http://")); Serial.print(WiFi.localIP());
  if (HTTPPort != 80) {
    Serial.print(F(":"));
    Serial.print(HTTPPort);
  }
  Serial.println();
}

void envoyerVariables(WiFiClient &cl)
{
  cl.print(F("HTTP/1.1 200 OK\r\n"
             "Content-Type: text/xml\r\n"
             "Connection: keep-alive\r\n\r\n"
             "<?xml version = \"1.0\" ?>"
             "<variables><reshaut>"));
   if(digitalRead(D3)==HIGH)
   cl.print("Active");
   else
   cl.print("Inactive");
  cl.print(F("</reshaut><resbas>"));
  if(digitalRead(D4)==HIGH)
   cl.print("Active");
   else
   cl.print("Inactive");
  cl.print(F("</resbas><tempbas>"));
  cl.print(temppierre);
  cl.print(F("</tempbas><temphaut>"));
  cl.print(tempchambre);
  cl.print(F("</temphaut><cons>"));
  cl.print(consignePierre);
  cl.print(F("</cons><etatgeneral>"));
  if(etatgeneral == 1)
   cl.print("ON");
   else
   cl.print("OFF");
  cl.print(F("</etatgeneral></variables>"));
}

boolean testRequeteWeb()
{
  algofour();
  boolean requeteHTTPRecue = false;
  byte indexMessage = 0;
  char * ptrGET, *ptrEspace;

  WiFiClient client = serveurWeb.available();
  if (!client) return requeteHTTPRecue; // pas de client connecté
  boolean currentLineIsBlank = true;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c == '\n' && currentLineIsBlank) { // une requête HTTP se termine par une ligne vide

        // ON GENERE LA PAGE WEB
        if (strcmp(urlRequest, "/favicon.ico")) { // si ce n'est pas pour le favicon
          requeteHTTPRecue = true;

          // on regarde si on a une requete qui continet /reqEtatBouton ( sinon on renvoie toute la page)
          if (strstr(urlRequest, "/reqEtatVariables")) { // http://www.cplusplus.com/reference/cstring/strstr/?kw=strstr
            // on va lire l'état du bouton et on renvoie l'information correctement
            envoyerVariables(client);
          }
           else if (strstr(urlRequest, "/activecuisson")) { // http://www.cplusplus.com/reference/cstring/strstr/?kw=strstr
            // action sur une variable
            // action 1 : activer mode cuisson : on active la resistance du haut et on coupe celle du bas
             modeCuisson=1;
             //Serial.println("requete activer cuisson");
              client.print(F("HTTP/1.1 200 OK\r\n"
             "Content-Type: text/xml\r\n"
             "Connection: keep-alive\r\n\r\n"
             "<?xml version = \"1.0\" ?>"
             ""));
          } 
          
          else if (strstr(urlRequest, "/desactivecuisson")) { // http://www.cplusplus.com/reference/cstring/strstr/?kw=strstr
            // action sur une variable
            // action 2 : désactiver mode cuisson : on coupe les deux resistances
            modeCuisson=0;
            client.print(F("HTTP/1.1 200 OK\r\n"
             "Content-Type: text/xml\r\n"
             "Connection: keep-alive\r\n\r\n"
             "<?xml version = \"1.0\" ?>"
             ""));
          } 
    
          else if (strstr(urlRequest, "/plus10cons")) { // http://www.cplusplus.com/reference/cstring/strstr/?kw=strstr
             // action 3 : +10°C consigne temp pierre
            consignePierre=consignePierre+10;
            client.print(F("HTTP/1.1 200 OK\r\n"
             "Content-Type: text/xml\r\n"
             "Connection: keep-alive\r\n\r\n"
             "<?xml version = \"1.0\" ?>"
             ""));
          } 
    
          else if (strstr(urlRequest, "/moins10cons")) { // http://www.cplusplus.com/reference/cstring/strstr/?kw=strstr
            // action sur une variable
            // action 4 : -10°C consigne temp pierre
            consignePierre=consignePierre-10;
            client.print(F("HTTP/1.1 200 OK\r\n"
             "Content-Type: text/xml\r\n"
             "Connection: keep-alive\r\n\r\n"
             "<?xml version = \"1.0\" ?>"
             ""));
          } 

           else if (strstr(urlRequest, "/marchearret")) { // http://www.cplusplus.com/reference/cstring/strstr/?kw=strstr
            // action sur une variable
            // action 4 : -10°C consigne temp pierre
                 Serial.println("requete ordre marche arret");
            if (etatgeneral==1)
            etatgeneral=0;
            else
            etatgeneral=1;
            
            client.print(F("HTTP/1.1 200 OK\r\n"
             "Content-Type: text/xml\r\n"
             "Connection: keep-alive\r\n\r\n"
             "<?xml version = \"1.0\" ?>"
             ""));
          } 
         
          else { // on envoie la page web par défaut
            Serial.println("par def");
            if (SPIFFS.exists(nomDeFichier)) {
              // On envoie un en tête de réponse HTTP standard de type HTML
              client.println("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: keep-alive\r\n");
              File pageWeb = SPIFFS.open(nomDeFichier, "r");
              client.write(pageWeb);
              pageWeb.close();
            } else {
              Serial.println(F("Erreur de fichier"));
            }
          }
        }
        break; // on sort du while et termine la requête
      } // fin de génération de la réponse HTTP

      if (c == '\n') {
        currentLineIsBlank = true;
        httpLine[indexMessage] = '\0'; // on termine la ligne correctement (c-string)
        indexMessage = 0; // on se reprépre pour la prochaine ligne
        if (ptrGET = strstr(httpLine, "GET")) {
          // c'est la requête GET, la ligne continent "GET /URL HTTP/1.1", on extrait l'URL
          ptrEspace = strstr(ptrGET + 4, " ");
          *ptrEspace = '\0';
          strncpy(urlRequest, ptrGET + 4, maxURL);
          urlRequest[maxURL] = '\0'; // par précaution si URL trop longue
        }
      } else if (c != '\r') {
        currentLineIsBlank = false;
        if (indexMessage <= maxHTTPLine - 1) {
          httpLine[indexMessage++] =  c; // sinon on ignore le reste de la ligne
        }
      }
    } // end if available
  } // end while
  delay(1);
  client.stop(); // termine la connexion
  return requeteHTTPRecue;
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     
  pinMode(D4, OUTPUT);// relai resistance du haut
  pinMode(D3, OUTPUT);//relai resistance du bas

  Serial.begin(74880); // parce que mon Wemos et par défaut à peu près à cette vitesse, évite les caractères bizarre au boot
  Serial.println("\n\nTest SPIFFS\n");

  // on démarre le SPIFSS
  if (!SPIFFS.begin()) {
    Serial.println("erreur SPIFFS");
    while (true); // on ne va pas plus loin
  }

  WiFi.begin(ssid, password);

  Serial.println();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.write('.');
  }
  Serial.println();

  // on démarre le serveur
  serveurWeb.begin();
  printHTTPServerInfo();
  etatgeneral=0;
   digitalWrite(D5, LOW);  //on desactive le relais de la resistance supérieure
    digitalWrite(D4, LOW);//on desactive le relais de la resistance inférieure
}

void loop() {
  testRequeteWeb();
}
