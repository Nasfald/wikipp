Installer WikiPP
----------------

### Dépendences

Le guide d\'installation est relativement bien fait¹ Cependant
l\'emplacement des dépendences est un peu floue :

-   cppCMS : l\'installation est déjà renseigner [ici](#cppcms)
-   CPPDB : il faut chopper le code et le compiler

                
                  git clone https://github.com/melpon/cppdb
                  cd cppdb
                  mkdir build
                  cd build
                  cmake ..
                  make
                  sudo make install
                
              

-   Gettext : est normalement disponible via le gestionnaire de packet
    `apt install gettext`

### Notes sur l\'installation

La procédure l\'installation est plutôt bien décrite mais il y une typo
a un endroit, je remet la liste des commandes dans l\'ordre :

          
            wget https://github.com/cppweb/wikipp.git
            cd wikipp
            mkdir build
                cd build
            cmake ..
            make
            sudo make install
          
        

De même, il manque la création de l\'utilisateur gérant da base de
donnée, je reprend cette phase en l\'y ajoutant. Les commandes suivantes
sont pour les moteurs mariadb ou mysql; réferes-toi sur le site¹ pour
une base sqlite3 ou postgres. Adaptes la premières commande pour
t\'authentifier avec les droits de créations de base et sur le moteur de
base de données ; dans mon cas `sudo mariadb`, mais il peut s\'agir de
`mysql -u root -p`.

        
          sudo mariadb
          CREATE DATABASE wikipp;
          CREATE USER wikipp IDENTIFIED BY "mot de passe fort";
          GRANT ALL PRIVILEGES ON wikipp.* TO wikipp ;
        
          

### Note sur la configuration

Si tu doutes de l\'emplacement de l\'installation de wikipp, lance la
commande suivante :

        
          find / -iname wikipp 2>/dev/null
        
          

Ignore le résultat dans `/usr`, il s\'agit de la base de donnée; dans la
suite je comsidère que ce répertoire est ` /usr/local/share/wikipp`. Une
fois obtenu, nous pouvons finaliser da base de donénes en important la
structure par défaut; souviens toi que je suis sur mariadb, adaptes à
ton moteur :

        
          sudo mariadb wikipp < /usr/local/share/wikipp/sql/mysql.sql 
        
          

Sauvegarde le modele de configuration, avant de le modifier (renplace
emacs par ton éditeur texte) :

        
          cp /usr/local/share/wikipp/sample_config.js  /usr/local/share/wikipp/sample_config.js.example
          emacs /usr/local/share/wikipp/sample_config.js
        
          

Il faut maintenant décommenter et modifier les lignes; la première
permet de se connecter à la base de données. Dans mon cas :

        
              // MySQL Sample Connection String
          //
          "connection_string" : "mysql:database=wikipp;user=user wikipp;password=mot de passe fort;@pool_size=16",
          //
        
          

Il faut ensuite générer une clef privée pour l\'application, en fonction
de la librairie utilisée.

#### Avec libgcrypt et openssl

La commande génerera le morceau de configuration a ajouter dans le
fichier de configuration. Executes la commande de manière à pouvoir
copier son résultat ou en le redirigeant dans un fichier (i.e
`$gt;/tmp`)

        
          cppcms_make_key --hmac sha1 --cbc aes
        
          

En cherchant les clefs des valeurs renvoyés par cette commande,
j\'obtiens le bloc suivant dans le fichier de configuration, où «CLEF
PRIVÉE » finit les chaines de caractère générées :

        
          "session" : {
              "expire" : "renew",
              "location" : "client",
              "timeout" : 2592000, // One month 24*3600*30
              "cookies" :  {
                  "prefix" : "wikipp"
              },
              "client" : {
                  "cbc" :"aes",
                  "cbc_key" :"CBC CLEF PRIVÉE",
                  "hmac" :"sha1",
                  "hmac_key" :"HMAC CLEF PRIVÉE"
              }
          }
        
          

#### Sans libgcrypt et openssl

La procédure est la même que précédemment, cependant la commande de
génération est la configuration sont différentes :

        
          "session" : {
              "expire" : "renew",
              "location" : "client",
              "timeout" : 2592000, // One month 24*3600*30
              "cookies" :  {
                  "prefix" : "wikipp"
              },
              "client" : {
                  "hmac" :"sha1",
                  "hmac_key" :"HMAC CLEF PRIVÉE"
              }
          }
        
          

#### Configuration du socket

L\'executable de wikipp peut-être atteinds via différents types de
socket; la configuration se fait dans la partie `"service"` de notre
fichier conf.js qu\'on passe en option. On peut utiliser un fichier
socket ou l\'interface réseau interne `127.*.*.*`. Je trouve plus simple
d\'utiliser cette dernière solution, qui est aussi la solution retenue
dans mes autres billets sur nginx.

        
        "service" : {
          "port" : 8065,
          "api" : "fastcgi",
          "ip" : "127.0.0.89"
        },
        
          

Depuis ma machine, l\'adresse du wiki sera sur `http://127.0.0.89:8065`.

#### Configuration du serveur web

Je me suis basé sur la doc officiel pour configurer le serveur nginx;
pour les autres serveurs, regarde la doc officiel dans les sources.
Sinon c\'est exactement le même bloc, mais avec une redirection vers
l\'interface interne⁴. J\'ai ajouter toute la configuration, avec le
certificat SSL multi-domaine générer par certbot, la redirection de HTTP
vers HTTPS. Des billets dédiés existent pour expliquer spécifiquement
ces configurations; si tu n\'en veux pas, ajoute simplement au bloc
`location ~ ^/wikipp.*$`. Il faut aussi que les fichiers
`/var/log/www/wiki/access.log` et `/var/log/www/wiki/error.log`
existent. Dans le fichie
`/etc/nginx/sites-avalaible/wiki.domain.org.conf`

        
     server {
         server_name wiki.domain.org;

         access_log /var/log/www/wiki/access.log;
         error_log /var/log/www/wiki/error.log error;

         root /var/www/wiki;

         location ~ ^/wikipp.*$ {
          fastcgi_pass 127.0.0.89:8065;

          # Setup value of PATH_INFO variable
          fastcgi_split_path_info ^(/wikipp)((?:/.*))?$;
          fastcgi_param  PATH_INFO       $fastcgi_path_info;

          #
          # You can either use "include fastcgi_params;"
          # or set the variables below manually
          #

          # All supported CGI variables
          fastcgi_param  SCRIPT_NAME     /wikipp;
          fastcgi_param  QUERY_STRING    $query_string;
          fastcgi_param  REQUEST_METHOD  $request_method;
          fastcgi_param  CONTENT_TYPE    $content_type;
          fastcgi_param  CONTENT_LENGTH  $content_length;

          fastcgi_param  REQUEST_URI     $request_uri;
          fastcgi_param  DOCUMENT_URI    $document_uri;
          fastcgi_param  DOCUMENT_ROOT   $document_root;
          fastcgi_param  SERVER_PROTOCOL $server_protocol;
          fastcgi_param  GATEWAY_INTERFACE  CGI/1.1;
          fastcgi_param  SERVER_SOFTWARE    nginx;

          fastcgi_param  REMOTE_ADDR        $remote_addr;
          fastcgi_param  REMOTE_PORT        $remote_port;
          fastcgi_param  SERVER_ADDR        $server_addr;
          fastcgi_param  SERVER_PORT        $server_port;
          fastcgi_param  SERVER_NAME        $server_name;

          # end of server variables
        }

        listen [::]:443 ssl; # managed by Certbot
        listen 443 ssl; # managed by Certbot
        ssl_certificate /etc/letsencrypt/live/wiki.domain.org/fullchain.pem; # managed by Certbot
        ssl_certificate_key /etc/letsencrypt/live/wiki.domain.org/privkey.pem; # managed by Certbot

     }server {
         if ($host = wiki.domain.org) {
              return 301 https://$host$request_uri;
         } # managed by Certbot

         listen 80;
         listen [::]:80;
         server_name wiki.domain.org;
         return 404; # managed by Certbot
     }
        
          

Après avoir fait un lien symbolique dans `/etc/nginx/sites-enabled/`,
redémarre nginx; avec systemd : `systemctl restart nginx`

#### Droits d\'utilisations

Il faut que l\'utilisateur qui execute l\'application ai les droits de
lecture sur le repo, voir d\'écriture sur le fichier socket (s\'il est
utilisé). Je te conseille de créer un utilisateur dédié, membre du group
www-data :

`adduser wikipp --ingroup www-data --no-create-home`

Vérifie que le repo soit effectivement lisible pour ce nouvel
utilisateur, sinon va dedans (chez moi `/var/www/wiki`):

        
          chown -R wikipp /var/www/wiki
          chmod -R u+rwx /var/www/wiki
            
          

### Tester

Pour tester, place toi dans le répertoire des sources de wikipp en tant
qu\'utilisateur wikipp, après les avoir compilées, et lance la commande
suivante :

`wikipp -c config.js `

Si tu as interdit de s\'identifier en tant que wikipp, utilise su:

`su -c "wikipp -c config.js"`

Le site est désormais accessible ! Depuis la même machine :

`http://127.0.0.89:8065`

Ou si tu as déclaré ton nom de domaine :

`http://wiki.domaine.org/wikipp`

#### Configuration de la racine

En omettant `wikipp` à la fin de l\'url, on se tape une sale erreur
`403`. Ce qui n\'est pas top. Il suffit simplemet de modifier les
configuration nginx. Remplace la ligne :

`location ~ ^/wikipp.*$`

Par :

`location ~ ^/(?!(favicon\.ico|.*.css|robots\.txt|.*.js)) {`

Relance nginx, et c\'est bon.

Au lieu de lancer l\'application à partir du point wikipp, on la lance
toujours, sauf exceptions; ces exceptions étant les feuille de style
(.css), les scripts .js et le fichier pour les crawlers (robots.txt). Ça
ajoute une couche de sécurité; ce qui ne dispense bien sûr pas des
autres.

`mv /var/www/wiki/conf.js /var/www/wiki/conf.ini`

Ça pose un problème de sécunité; le fichier de conf est lisible via le
serveur, puisqu\'il est en .js. Je propose le le changer en conf.ini

#### Wikipp en tant que service

On va pas lancer wikipp à la main à chaque démarrage de la machine. Le
plus simple est de créer un service; je part lu principe que t\'es sur
systemd, regarde de la doc de ton gestionnaire de service pour adapter.
On va créer un fichier de service; remplace ton éditeur de texte :

`emacs /etc/systemd/system/wiki.service`

        
          [Unit]
          Description=WikiPP manager
          After=mysql.service network.target

          [Service]
          ExecStart=/usr/local/bin/wikipp -c /var/www/wiki/config.ini
          Type=simple
          PIDFile=/run/wikipp.pid
          WorkingDirectory=/var/www/wiki
          User=wikipp

          [Install]
          WantedBy=multi-user.target
        
          

Ensuite on donne l\'autorisation et on actualise les daemon :

        
          chmod +x /etc/systemd/system/wiki.service
          systemctl daemon-reload 
        
          

Tu peux démarrer le serveur, et tester :

`sudo systemctl start wiki`

Et si tu veux que le service se lance automatiquement au démarrage⁵ :

`sudo systemctl enable wiki`

#### Autres configurations

Le guide officiel prévoit d\'autres configuration pour les dictionnaires
de traductions et la connection de la libraire CPPDB. Je n\'en ai pas eu
besoin, sauf pour les langues; le français étant absent par défaut.

### Messages d\'erreurs

-   `wikipp, error: system: Socket operation on non-socket (main.cpp:14)`
    : Le socket est mal définit dans le fichier de configuration ou le
    fichier de configuration est mal définit.
-   `Failed to load skin:view, no shared object/dll found` : il manque
    des fichiers de la compilation. Soit la compilation a échouée, soit
    l\'execution n\'a pas eu lieu dans le répertoire des sources.

La commande `wikipp` permet de lancer le logiciel. Il faut cependant
spécifier `-c` en option, pour le fichier de configuration fournit avec
les sources `config.js`

### Sources

-   \[1\][cppcms.com
    install\_wikipp](http://cppcms.com/wikipp/en/page/install_wikipp)
-   \[2\][stackoverflow
    compiler-error-msgfmt-command-not-found](https://stackoverflow.com/questions/9500898/compiler-error-msgfmt-command-not-found)
-   \[3\][matt.scharley.ne socket et
    socat](https://matt.scharley.me/2012/03/debugging-application-interactions-with-socat.html)
-   \[4\][cppcms.com
    cppcms\_tut\_web\_server](http://cppcms.com/wikipp/en/page/cppcms_1x_tut_web_server_config)
-   \[5\][digitalocean.com
    manage-systemd-services-and-units](https://www.digitalocean.com/community/tutorials/how-to-use-systemctl-to-manage-systemd-services-and-units)
:::

::: {#20210311-audio-stream .billet data-tag="audio stream html5"}
Lecteur web de flux audio
-------------------------

HTML5 introduit des lecteurs média par défaut, capabde ve lire des fluxs
de média. Bien sûr, les navigateurs intègre cette norme de manière
inégale. La balise audio prend les atrributs suivants :

-   `controls` : affiche les controles
-   `preload` : pré-charge autonmatiquement
-   `autoplay` : lance aucomatiquement de contenu

La source du flux peu-être précisée en attribut (`src`) ou dans des
sous-éléments de types `<source>`. Il est possible de donner plusieurs
sources; en cas d'échec, le lecteur tentera la suivante. C'est
indispensable pour assurer une compatibilité entre les navigateurs.
Aucun format audio n'est compatible partout pour le moment. Dans
l'exemple suivant, jqai une source ogg ec ure autre en mp3, ce qui
devrait mancher sur Opéra, Firefox, et Chromium.

`     `

            <audio controls preload autoplay>
                    <source src="https://server.de.stream/1.mp3" type="audio/mpeg">
                    <source src="https://server.de.stream/1.ogg" type="audio/ogg">
                </audio>
        
:::

::: {#20210310-vlc-radio .billet data-tag="vlc nginx"}
Faire une webradio avec VLC
---------------------------

On le dira jamais assez : VLC est magique. Je m\'attarderais pas sur
tout ce que ce petit soft est capable de faire; c\'est très long. Ce qui
nous intéresse ici c\'est sa capacité à être compdètement utilisé dans
la console, et à pouvoir créer un flux audio ou vidéo sur le réseau. Par
exemple, si mon adresse de réseau local est 192.168.1.2, et que je veux
diffuser le fichier radio.mp4 :

`     `

    vlc -vvv radio.mp4 --sout "#standard{access=http,mux=ogg,dst=192.168.1.2}"
        

Pour tester, il est possible d\'ouvrir un flux réseau depuis
l\'interface graphique, et y écrire `http://192.168.1.2`

Pour faire une webradio, je propose de diffuser le contenu sur une
interface réseau local (loop), et de rediriger les demandes de
connexions externe vers ce flux interne. Pour cela, suit le tuto pour
ajouter un sous-réseau, et créer le fichier de configuration suivant
(ici pour radio\@example.cccp). Lance la cammande précédante, mais
remplace l\'adresse de diffusion par 127.0.0.1.1032:

`     `

   server {
        listen 80;
        listen [::]:80;
        server_name radio.example.com;

        location / {
            proxy_pass http://127.0.0.1:1032/;
        }
    }
        

Il est tout a fait possuble de faire passer le stream par une connexion
https via nginx.

Sources :
---------

-   [wiki.videolan.org RTSP on demand
    streaming](https://wiki.videolan.org/Documentation:Streaming_HowTo/Command_Line_Examples/#RTSP_on-demand_streaming)
:::
