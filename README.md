# Opgave Project Computer Graphics

### prof. dr. Frank Van Reeth

### dr. Jeroen Put

### dr. Lode Jorissen

### dhr. Joren Michels

### academiejaar: 2024-

## 1 Opgave

In dit project maak je met OpenGL een3D-omgeving met een object dat over een curve beweegten
die verder voldoet aan een aantal vereisten. Thematisch ben je vrij in de invulling van de omgeving.
Het object dat over de curve beweegt kan bv. een rollercoaster zijn, een mijnkarretje, een trein,...

## 2 Vereisten

### 2.1 Inhoud


1. curve: in de applicatie laat je een object over een traject van Bezier curves bewegen. Visualiseer
bij voorkeur de curves a.d.h.v. forward differencing. Gebruik de brute-force methode als je dit
niet ge ̈ımplementeerd krijgt.
2. animatie: de eenvoudigste manier om het object over een curve te verplaatsen is door de posities
rechtstreeks met de curveparametertte evalueren, die binnen 0 (het begin) en 1 (het einde van
curve) ligt. Je zal echter merken dat een constante verandering van die parameter geen constante
snelheid van het object teweegbrengt. Om dit op te lossen dien je de curve te evalueren in
functie van booglengted. Stel m.a.w. een look-up table op die voor elke waarde vandeen
overeenkomstige waarde voor parametertgeeft. Door nu parameterdte gebruiken voor de
beweging zal de snelheid op elk deel van de curve constant zijn.


3. modellen/textures: gebruik bij voorkeur 3D-modellen en textures die je op het internet vindt.
Je mag die bestanden met een library inladen. Het tekenen moet echter gebeuren met zelfge-
schreven OpenGL-code.
4. visualisatie: voorzie functionaliteit om de curve zelf te visualiseren. Dit is ook handig voor
jezelf om je applicatie te debuggen. Zorg voor objecten die meebuigen met de curve, bv. een
treinspoor. Dit zorgt voor een veel mooiere visualisatie. Een eenvoudige manier om dit aan te
pakken maakt gebruik van vaste ’puzzelstukjes’ die je aan elkaar plakt, bv. een treinspoor met
rechte stukken, bochten van 10 graden, 20 graden,... Een meer geavanceerde methode vertrekt
van een recht stuk en buigt de mesh d.m.v. een geometrische transformatie op de vertices.
5. camera: implementeer zowel een camera waarmee je je hele parcours kan bekijken alsook een
first-person camera zodat je de rit van je object/karretje kan meevolgen.
6. belichting: plaats lokale lichtbronnen naast je parcours die het object opvallend belichten. Zorg
voor per-pixel belichting m.b.v. fragment shaders. De invloed van elke lichtbron moet afnemen
met de afstand.



7. convolutie: implementeer een shader die convolutie toepast als een post-processing shader.
Gebruik een kernel die resulteert in het blurren van het beeld (bijvoorbeeld aan de hand van
een Gaussian Blur) en een kernel die de randen highlight (bijvoorbeeld een Laplaciaan of Sobel
kernel). Render hiervoor je scene uit naar een framebuffer object (FBO). Gebruik vervolgens de
kleurenbuffer van deze FBO als textuur en de opgestelde shader om een rechthoek die uitgelijnd
is met de camera, en het scherm volledig vult (een screen aligned quad), uit te tekenen.
8. post-processing: gebruik de vorige stap om een bloom/neon/halo effect op je lichtbronnen te
implementeren, zodat ze nog feller lijken te schijnen. Een manier waarop je dit kan doen is door
je lichtbronnen naar een tijdelijke framebuffer te renderen, waarop je een Gaussian blur toepast.
Naderhand kan je dit resultaat combineren met je oorspronkelijke framebuffer om zodoende
een schijn rond je lichtbronnen te bekomen.
9. chroma-keying: implementeer chroma-keying (green-screen of blue-screen) in de YCbCr-
kleurenruimte om een afbeelding over de gerenderde scene te tonen. Dit doe je, net zoals bij
convolutie, eveneens door een screen aligned quad uit te tekenen. De gebruikte afbeelding kan
bv. een versierde kader zijn die in het midden groen is. De groene kleur wordt dan met chroma
keying in de fragment shader gediscard.
10. interactie: voeg minstense ́en vorm van interactie toe a.d.h.v. picking. Je mag dit implemen- ́
teren met OpenGL picking, colour picking of raycasting. Een voorbeeld van een dergelijke
interactie is het klikken op een wisselspoor, waarna het object een ander deel van het parcours
volgt.

### 2.2 Programmeeromgeving

1. De applicatie wordt geschreven in C++ of Python.
2. Als graphics API gebruik je OpenGL.
3. Voor deze applicatie is de library GLFW aan te raden.
4. Het onderwijsteam gaat uit van een Windows programmeeromgeving met Visual Studio. Een
    andere configuratie of besturingssysteem mag, maar hiervoor kan geen ondersteuning geboden
    worden. Merk in het bijzonder op dat MacOS de laatste jaren slechte ondersteuning heeft voor
    OpenGL. Hou hier dus rekening mee.

## 3 Modaliteiten

1. Het project wordt ingroepjes van tweegemaakt. Tegen uiterlijk16 aprilgeef je je keuze door
    per mail aanalleassistenten (Jeroen Put, Lode Jorissen en Joren Michels).
2. De code zal worden gecontroleerd op plagiaat. Klakkeloos code overnemen uit online tutorials
    of van anderen wordt niet getolereerd. Dit wordt door ons gecontroleerd met automatische anti-
    plagiaatsoftware, dus de pakkans is hoog. Plagiaat wordt behandeld als fraude op het examen.
    Wees dus gewaarschuwd!
3. Gebruik bij voorkeur een versioning systeem zoals Git of Subversion. Normaal gesproken zal
    een groepjee ́en score krijgen, maar in geval van onevenwichtige bijdragen in een groepje van ́
    2 zullen de logs gebruikt worden om de eindscores te bepalen.


## 4 Verslag

In het verslag (ongeveer 2 blz) geef je een lijst vanallefeatures die je ge ̈ımplementeerd hebt. Indien
je elementen uit de basisopgave niet hebt gemaakt, vermeld dit dan ook. Bespreek tevens nauwkeurig
de planning die je gevolgd hebt (per dag, per uur, per persoon).

## 5 Demofilmpje

Voor de beoordeling van je project maak je een kort Youtube filmpje (min. 5 min, max. 10 minuten)
waarin jeallefunctionaliteiten van je applicatie demonstreert. Geefmet tekst en spraakaan welke
feature te zien is. Toon enkel je applicatie, niet je development IDE of code. Dit filmpje zal gebruikt
worden voor de quotering, verzorg dit dus! Bovendien kan dit filmpje later altijd nog nuttig zijn op
bv. je online CV om je skills in de verf te zetten.

## 6 Beoordeling en inlevering

Bij de beoordeling bekijken we het filmpje en wordt de achterliggende code tevens doorgelicht. In
geval van vragen kan het onderwijsteam contact opnemen per mail. Stuur een link naar het filmpje en
een .zip bestand met code en verslag naar het gehele onderwijsteam per mail. Indien de bestanden te
groot zijn, stuur dan de bijlage met WeTransfer.

## 7 Deadline

De deadline voor het project is4 juni om 23.59uur.
Bij vragen of opmerkingen over de opgave: aarzel niet om het onderwijsteam te contacteren!

VEEL SUCCES!