===============================
 WARCABY – GRA NAPISANA W C
===============================

Autor: Jędrzej Augustyniak
Przedmiot: Programowanie w języku C  
Temat: Gra w warcaby (z GUI, zapisem i wczytywaniem)

Wersja: 1.0  
Data: maj 2025  
Środowisko: MSYS2 + MinGW64, Allegro5, Windows 10/11

================================
 OPIS GRY
================================

Gra w warcaby dla dwóch graczy (biały i czarny) na jednej klawiaturze.
Pionki poruszają się po planszy 8x8 zgodnie z zasadami gry w warcaby:
- pionki biją tylko do przodu
- damki biją w obu kierunkach
- obowiązkowe bicie
- awans na damkę po dojściu do końca planszy
- koniec gry po zbiciu wszystkich pionków przeciwnika
  lub gdy przeciwnik nie ma już możliwych ruchów

================================
 STEROWANIE
================================

Sterowanie odbywa się z klawiatury:

- Strzałki ← ↑ → ↓ — poruszają kursorem
- Spacja         — wybierz pionek / wykonaj ruch
- S              — zapisz grę do pliku `save/save1.txt`
- L              — wczytaj grę z pliku `save/save1.txt`
- Esc            — zakończ grę

================================
 URUCHOMIENIE
================================

Kompilacja:
'bash'
make
uruchomienie:
./warcaby.exe
