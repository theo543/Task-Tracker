# OOP Template

### Tema 0

- [x] Nume proiect: Task Tracker
- [x] Scurtă descriere a temei alese: Programul stochează o listă de task-uri într-un priority queue, 
      sortate după prioritate. Fiecare task e asociat unui student.

## Tema 1

#### Cerințe
- [x] definirea a minim 3-4 clase folosind compunere
- [x] constructori de inițializare
- [x] pentru o clasă: constructor de copiere, `operator=` de copiere, destructor (lui clang-tidy nu îi place asta)
<!-- - [ ] pentru o altă clasă: constructor de mutare, `operator=` de mutare, destructor -->
<!-- - [ ] pentru o altă clasă: toate cele 5 funcții membru speciale -->
- [x] `operator<<` pentru toate clasele
- [ ] cât mai multe `const` (unde este cazul)
- [x] implementarea a minim 3 funcții membru publice pentru funcționalități specifice temei alese
- [x] scenariu de utilizare a claselor definite:
  - crearea de obiecte și apelarea funcțiilor membru publice în main
  - vor fi adăugate în fișierul `tastatura.txt` exemple de date de intrare de la tastatură (dacă există)
- [ ] tag de `git`: de exemplu `v0.1`
- [ ] serviciu de integrare continuă (CI); exemplu: GitHub Actions

## Tema 2

#### Cerințe
- [ ] separarea codului din clase în `.h` (sau `.hpp`) și `.cpp`
- [ ] moșteniri
  - [ ] clasă cu atribut de tip pointer la o clasă de bază cu derivate
  - [ ] funcții virtuale (pure) apelate prin pointeri de bază din clasa de mai sus, constructori virtuali (clone)
    - minim o funcție virtuală va fi **specifică temei** (e.g. nu simple citiri/afișări)
  - [ ] apelarea constructorului din clasa de bază din constructori din derivate
  - [ ] smart pointers
  - [ ] `dynamic_cast`
- [ ] suprascris cc/op= pentru copieri/atribuiri corecte, copy and swap
- [ ] excepții
  - [ ] ierarhie proprie cu baza `std::exception` sau derivată din `std::exception`; minim 2 clase pentru erori specifice
  - [ ] utilizare cu sens: de exemplu, `throw` în constructor, `try`/`catch` în `main`
- [ ] funcții și atribute `static`
- [ ] STL
- [ ] cât mai multe `const`
- [ ] la sfârșit: commit separat cu adăugarea unei noi derivate fără a modifica restul codului
- [ ] tag de `git`: de exemplu `v0.2`

## Tema 3

#### Cerințe
- [ ] 2 șabloane de proiectare (design patterns)
- [ ] o funcție șablon (template) cu sens; minim 2 instanțieri
- [ ] o clasă șablon cu sens; minim 2 instanțieri
<!-- - [ ] o specializare pe funcție/clasă șablon -->
- [ ] tag de `git`: de exemplu `v0.3` sau `v1.0`

## Resurse

- adăugați trimiteri către resursele externe care v-au ajutat sau pe care le-ați folosit
