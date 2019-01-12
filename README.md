# RG57-hop


## Kontrole

F11 - Fullscreen

F10 - No clip mod

F9 - Iscrtavanje bounding boxova

F8 - Visestruki viewportovi

F7 - Restart

WASD - kretanje i dijagonalno kretanje
 
Strelice - rotacija kamere

C - cucanje

Space - skok

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

## Spoljasni resursi

koriscene su biblioteke:

Za kratke regex-e: https://github.com/kokke/tiny-regex-c

Za ucitavanje tekstura: https://github.com/nothings/stb/blob/master/stb_image.h


## Pozeljno implementirati

Ubrzanje, usporenje kod dash kretanja.

Pri koliziji treba da "slide-uje" uz zid.
