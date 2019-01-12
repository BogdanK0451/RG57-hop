# RG57-hop


## Kontrole

F11 Fullscreen
F10 No clip mod
F9  Iscrtavanje bounding boxova
F8 Visestruki viewportovi

WASD kretanje i dijagonalno kretanje
 
Strelice rotacija kamere

C cucanje

Space skok

Escape za kraj programa

## Setup
Da bi pomeranje kamere misem radilo normalno, program mora da se pokrene sa VMWWARE Workstation-a ili native (ne Virtual box i ne Vmware Player) i u njemu treba da se podesi edit>preferences>input>optimize mouse for games> always.
U suprotnom ce mis imati ogroman sensitivity i jedina opcija ce biti kretanje kamere na strelice.

## Poznati problemi

Ukoliko se igrac krece brzinom koja je veca od dimenzija objekta nece doci do kolizije.

Pri koliziji igraс ne "slide-uje" uz zid.

Dijagonalna brzina kretanja veca od vertikalne i horizontalne.

## Spoljasni resursi

koriscene su biblioteke:

Za kratke regex-e: https://github.com/kokke/tiny-regex-c

Za ucitavanje tekstura: https://github.com/nothings/stb/blob/master/stb_image.h
