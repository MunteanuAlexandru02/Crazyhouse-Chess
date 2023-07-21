# Crazyhouse Chess

## Echipa: The Escapists
	- Chiriac Cristian-Alexandru
	- Mogodeanu Claudiu
	- Munteanu Alexandru-Constantin
	- Pandelica Mihai-Lucian

# Etapa 1:
Un bot care este capabil sa joace un joc de sah complet, respectand toate
miscarile legale din jocul de Crazyhouse

Acest proiect isi propune sa rezolve cerinta primei etape din cadrul proiectului
la PA. Botul este capabil sa:

* Efectueze mutarile de baza al pieselor, asigurandu-se de respectarea
regulilor.
* Execute promovarea pionilor.
* Indeplineasca Rocadele (Mica + Mare), dar si En-passant.
* Verifice daca regele este in sah si sa efectueze o mutare legala.
* Realizeze drop-in-urile specifice modului de joc Crazyhouse.

## Instructiuni de utilizare:
	Compilare: make build

	Rulare: make run
	
	Rulare in xboard: xboard -fcp "make run" -debug

## Detalii despre structura proiectului	

Pentru memorarea pieselor de pe tabla de sah am folosit o matrice de 8x8,
in care o piesa va fi codificata folosind campurile type si color din
cadrul structurii PieceData. 

Pentru a obtine toate mutarile posibile (valide), pe care le vom stoca
intr-o coada uzitam functiile din fisierul "GetValidMoves", care vor
verfica pentru fiecare piesa mutarile corecte, iar in cazul mutarii
regelui, functia va incerca sa forteze realizarea unei Rocade, cat timp
aceasta este posibila.

Functia checkDropIn va verifica posibilitatea executarii drop-in-ului
specific Crazyhouse-ului, atunci cand un drop-in este valid, functia va
returna mutarea respectiva, urmand a fi adaugata ulterior in coada de
mutari.

Functia isCheck are rolul de a verifica daca regele culorii care va
trebui sa mute se afla in sah. Aceasta va fi apelata pentru fiecare mutare
pe care vom incerca sa o realizam pentru a verifica daca o aceasta este
valida, urmand a fi adaugata in coada.

O piesa va putea promova fie intr-un cal, fie intr-o regina, deoarece
am considerat ca promovarea intr-un nebun / intr-o tura nu reprezinta
un avantaj tactic fata de o regina si am dorit, de asemenea,
simplificarea algoritmului.

In fisierul "Bot.cpp" avem:

Constructorul fara parametri al bot-ului care construieste tabla initiala,
urmarind exemplul  https://i.imgur.com/UUiOE31.png.

Pentru o urmatoare mutare vom parcurge matricea, dupa care vom verifica urmatoarele:

	* Daca pe pozitia curenta se afla o piesa si culoarea piesei este
	cea care ar trebui sa efectueze o mutare, vom verifica mutarile
	posibile pentru acea piesa. Daca avem posibilitatea realizarii
	unei Rocade, o vom face.

	* Daca pe pozitia curenta nu se afla o piesa, vom verifica
	posibilitatea realizarii unui drop-in.

Daca in urma parcurgerii matricei, coada de mutari este vida (nu exista mutari
valide posibile), apelam resign(), altfel vom realiza o mutare random din coada.

recordMove: primeste o tabla si o mutare valida, va realiza mutarea respectiva,
actualizand piesele capturate / piesele promovate/ pozitiile regelui si a turelor
(realizeaza o Rocada), dupa caz.

## Detalii despre complexitatea functiilor

Functiile folosite la aceasta etapa implementeaza un numar standard
de parcurgeri de tabla, variind neglijabil in functie de tipul piesei.
De asemenea, acestea efectueaza exclusiv verificari liniare, nefiind
folositi algoritmi de optimizare.

Complexitatea functiilor de verificare a mutarilor valide este
data de functia isCheck, care va fi apelata pentru fiecare incercare de
mutare si va realiza independent de piesa care urmeaza a fi mutata,
aproximativ 40 de verificari, urmand a fi multiplicata cu aproximativ
numarul de miscari posibile pentru piesa care va fi testata.

## Surse de inspiratie

Am folosit mai multe articole publicate pe site-uri de profil (chess.com,
lichess.com) pentru a ne asigura ca luam in calcul toate cazurile specifice
variantei Crazyhouse in implementarea functiilor de la aceasta etapa.
De asemenea, ne-am folosit de simulatoarele de joc puse la dispozitie de
acestea pentru a intelege mai bine cum se desfasoara un joc.

## Responsabilitatea fiecarui membru al echipei

Pentru primele cateva intalniri in cadrul echipei fiecare membru a
implementat mutarea si asezarea unor piese:

	Chiriac  : Nebunul
	Mogodeanu: Calul
	Munteanu : Tura
	Pandelica: Pionul

Dupa ce ne-am asigurat ca intelegem destul de bine functionalitatea xboard-ului
am ales sa lucram folosind extensia "Live Share" din VSCode pentru a simplifica
interactiunea, dar si procesul de scris cod, fiind mult mai simplu sa scriem
intr-un singur proiect, evitand astfel problemele de merge.

Pe langa mutarea de baza a pieselor corespondende fiecarui membru al echipei,
fiecare dintre noi s-a ocupat de o parte din verificarile si cazurile speciale
de mutari, fie specifice jocului clasic de sah, fie variantei Crazyhouse.

	Pandelica: Verificarea conditiilor de sah + En-passant
	Munteanu : Verificarea indeplinirii regulilor de sah + En-passant
	Mogodeanu: Verificarea corectitudinii cozii de mutari + Rocade
	Chiriac  : Verificarea flow-ului jocului + drop-in + Rocade

In ciuda faptului ca unele functii au fost implementate separat, verificarea
acestora a fost realizata in grup, toti membrii echipei analizand aceeasi
functie, pentru a descoperi mult mai usor anumite bug-uri si defecte in cod.

## Resurse:

	https://www.gnu.org/software/xboard/engine-intf.html
	https://en.wikipedia.org/wiki/En_passant
	https://en.wikipedia.org/wiki/Castling#Requirements

# Etapa 2:
La fel ca in prima etapa, botul de sah este capabil sa joace un joc
complet, de aceasta data fiind capabil sa analizeze tabla de joc si sa
aleaga o mutare avantajoasa.

## Imbunatatiri aduse etapei 1:

Am schimbat forma functiilor astfel incat acestea sa primeasca o structura de
tip 'Table', care, pe langa matricea propriu-zisa cu reprezentarea
pieselor, contine si piesele capturate si pozitiile regilor pentru fiecare jucator.
Acest lucru ne ofera posibilitatea de a lucra cu diferite configuratii ale
jocului, nu doar cea curenta (asa cum era in cazul etapei 1, unde aveam
un singur set de variabile globale care sa retina aceste date, impartite
de intreg programul).

Astfel, putem implementa cautarea in spatiul posibilelor mutari, pornind de la
configuratia curenta, si generand pe parcurs reprezentari ale fiecarei stari,
iar mai departe sa alegem varianta cea mai avantajoasa.

Vom detalia mai jos procesul de decizie a urmatoarei mutari:

* Am folosit algoritmul Negamax, pentru a evita codul duplicat, pe care l-am
implementat in functia 'calculateNextMove'.
Am generat coada de posibile mutari, pe care am parcurs-o si am identificat
varianta pentru care functia euristica ofera cea mai mare valoare dupa analiza
configuratiei. De asemenea, am aplicat si un mijloc de randomizare pentru
alegerea intre variantele pentru care functia genereaza valoarea maxima
la iteratia curenta, pentru a varia parcursul jocului. Spre exemplu, in
cazul primelor mutari dintr-un meci, functia va genera valoarea 0 pentru
toate posibilele mutari (toate piesele sunt pe tabla, nu exista inca piese
capturate), iar daca pastram prima mutare cu valoare maxima gasita vom muta
mereu din aceeasi parte a tablei, cel putin pentru o perioada.
	
* Am implementat o euristica care asociaza fiecarei piese un punctaj, 
piesele capturate avand un scor mai mare, deoarece am dorit
ca versatilitatea mai mare a pieselor capturate sa reiasa din
punctaj.
Euristica calculeaza un scor adunand punctajul pieselor proprii
(capturate si pe tabla) si scazand numarul de puncte asociat pieselor adversarului.

* Suplimentar, am vrut ca botul sa incerce sa dea sah mai rapid, astfel incat am crescut
artificial scorul la un anumit pas daca in tabla curenta regele advers este in sah.
Am ales ca valoare bonus 5, intrucat nu am vrut ca botul sa fie prea agresiv in propriul detriment;
cu o valoare mai mare, ar fi riscat mai multe piese si mai des, ceea ce ducea la situatia in care
adversarul avea un numar foarte mare de piese in mana si, eventual, sah mat.

* Piesele au asociate urmatoarele scoruri:


	| Piesa     	| Capturata     | Normala   
	| ------------- | ------------- | --------    |
	| `Pion`        | 	1       | 	1     |
	| `Nebun`       | 	5       | 	3     |
	| `Cal`       	| 	10      | 	3     |
	| `Tura`        | 	5       | 	5     |
	| `Regina`      | 	10      | 	9     |
	| `Rege`       	| 	90      | 	90    |
	| `Casuta Goala`| 	0       | 	0     |

## Extra:
Pentru etapa 2, am incercat sa aplicam optimizarea Alpha-Beta pruning pentru
algoritmul Negamax, cu o sortare a mutarilor disponibile in functie de piesa
capturata, daca ea exista. In urma testarii, indiferent daca modificam valorile
pieselor sau cresteam adancimea cautarii (care era posibila datorita optimizarii),
am observat ca obtinem rezultate mai slabe impotriva Stockfish Hard, deoarece
cautarea se oprea mult prea repede, iar randomizarea mentionata mai sus nu mai
avea efect, deci se pierdea variatia zonelor in care efectuam mutari la inceputul
jocului.


## Responsabilitatea fiecarui membru al echipei

La aceasta etapa am lucrat exclusiv impreuna, folosind in continuare extensia Live Share, responsabilitatile fiind impartite on-the-spot.


## Resurse:

	https://en.wikipedia.org/wiki/Negamax
	https://ocw.cs.pub.ro/courses/pa/laboratoare/laborator-05

