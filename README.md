# RG57-hop


## Kontrole

F11 - fullscreen

F10 - no clip mod

F9 - iscrtavanje bounding boxova

F8 - visestruki viewportovi

F7 - textureless mode

F5 - restart

WASD - kretanje i dijagonalno kretanje
 
Strelice - rotacija kamere

C - cucanje

Space - skok

E - pull ka objektu

T - teleport

Middle mouse + WASD - dash

Desni klik - zoom

Escape - kraj programa

## Setup

Instalirati gcc, make i glut

sudo apt install gcc
sudo apt install make
sudo apt-get install binutils-gold freeglut3 freeglut3-dev 

Da bi pomeranje kamere misem radilo normalno, program mora da se pokrene sa VMWWARE Workstation Pro-a ili native Ubuntu (ne Virtual box i ne Vmware Player) i u njemu (VMWARE Workstation pro-u) treba da se podesi edit>preferences>input>optimize mouse for games> always.
U suprotnom ce mis imati ogroman sensitivity i jedina opcija ce biti kretanje kamere na strelice.

## Poznati problemi

Ukoliko se igrac krece brzinom koja je veca od dimenzija objekta nece doci do kolizije.

Dijagonalna brzina kretanja veca od brzine kretanja pri drzanju samo W ili A ili S ili D.

Na ulaznim blockovima u belu "kucu" ne dolazi do bojenja blockova prilikom kolizije

## Spoljasni resursi

koriscene su biblioteke:

Za kratke regex-e: https://github.com/kokke/tiny-regex-c

Za ucitavanje tekstura: https://github.com/nothings/stb/blob/master/stb_image.h


## Pozeljno implementirati

Ubrzanje, usporenje kod dash kretanja.

Dinamicki skybox.

Pri koliziji treba da "slide-uje" uz zid.
