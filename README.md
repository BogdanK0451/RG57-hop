# RG57-hop


## Kontrole

F11 - fullscreen

F10 - no clip mod

F9 - iscrtavanje bounding boxova

F8 - visestruki viewportovi

F7 - textureless mode

F6 - smena dana i noci

F5 - restart

Page Up - ubrzavanje animacije iscrtavanja stormtroopera

Page Down - usporavanje animacije i "deiscrtavanje" stormtroopera ako je vrednost <0

WASD - kretanje i dijagonalno kretanje
 
Strelice - rotacija kamere

C - cucanje

TAB - trcanje

Space - skok

E - jetpack

T - teleport

Middle mouse + WASD - dash

Desni klik - zoom

Escape - kraj programa

Plavi krug oko nisana govori da je "jetpack" istrosen [primer](https://gyazo.com/39829cedca9250ff2d76b6d2d3365b02)

Crveni krug oko nisana govori da je je teleport i blink na cooldown-u od 5 sek [primer](https://gyazo.com/ae9c9c8d92ad44f0d73c8d08c2539232)

## Setup

Za ubuntu:

Instalirati gcc, make i glut
sudo apt install gcc
sudo apt install make
sudo apt-get install binutils-gold freeglut3 freeglut3-dev 

Da bi pomeranje kamere misem radilo normalno, program mora da se pokrene sa VMWWARE Workstation Pro-a ili native Ubuntu (ne Virtual box i ne Vmware Player) i u njemu (VMWARE Workstation pro-u) treba da se podesi edit>preferences>input>optimize mouse for games> always.
U suprotnom ce mis imati ogroman sensitivity i jedina opcija ce biti kretanje kamere na strelice

**Iz nekog razloga na mojoj virtuelnoj masini kod programa postoje artifakti kada se scena iscrtava u rezimu GL_LINES [primer](https://gyazo.com/fb9e1695cfcf707c4b7928b48d046156)
mozda na vasoj tog problema nece biti. Na windowsu tog artifakta nema [primer](https://gyazo.com/e42b0237a4c03971c76a8568a321619c)**


Za windows:

Na windowsu se program moze pokrenuti ili otvoriti kroz Visual Studio ukoliko su dostupni **glut i glew**
tutorijal za "instalaciju"

https://www.youtube.com/watch?v=8p76pJsUP44

Struktura direktorijuma programa je malo drugacija, za kompajliranju na windows-u iskoristiti program koji sam stavio u
OpenGL.rar

## Poznati problemi

Ukoliko se igrac krece brzinom koja je veca od dimenzija objekta nece doci do kolizije.

Dijagonalna brzina kretanja veca od brzine kretanja pri drzanju samo W ili A ili S ili D.

Teleportovanje radi samo ukoliko je kamera iznad horizonta 
(kad je ispod odma dolazi do kolizije igraca i najblizeg bloka i nista se ne desava)

Na ulaznim blockovima u belu "kucu" ne dolazi do bojenja blockova prilikom kolizije

Ako se skok pritisne tokom trajanja prethodnog skoka prilikom dodira sa zemljom docice do jos jednog skoka

## Spoljasni resursi

Koriscene su biblioteke:

Za kratke regex-e: https://github.com/kokke/tiny-regex-c

Za ucitavanje tekstura: https://github.com/nothings/stb/blob/master/stb_image.h


## Pozeljno implementirati

Pri koliziji treba da "slide-uje" uz zid

Bolje osvetljenje

Dodati biblioteku za zvuk

Pull "tip" kretanja

Zamena mape u runtime-u

Upotreba freetype biblioteke za jednostavnije ispisivanje informacija o stanju programa

