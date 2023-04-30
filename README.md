# Crazyhouse Chess

## Etapa 1:
un bot care este capabil sa joace un joc de sah complet, respectand toate
miscarile legale din jocul de Crazyhouse

## Echipa: The Escapists
	- Chiriac Cristian-Alexandru
	- Mogodeanu Claudiu
	- Munteanu Alexandru-Constantin
	- Pandelica Mihai-Lucian

Acest proiect isi propune sa rezolve cerinta primei etape din cadrul proiectului
la PA. Botul este capabil sa:

* Efectueze mutarile de baza al pieselor, asigurandu-se de respectarea
regulilor.
* Execute promovarea pionilor.
* Indeplineasca Rocadele (Mica + Mare), dar si En-passant.
* Verifice daca regele este in sah si sa efectueze o mutare legala.
* Realizeze drop-in-urile specifice modului de joc Crazyhouse.

## Instructiuni de compilare:
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

