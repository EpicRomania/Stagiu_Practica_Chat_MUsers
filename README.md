# Stagiu_Practica_Chat_MUsers


# 18.06.2024 
Am adaugat componentele pentru server si pentru client goale + m am documentat despre libariile boost/asio/ssl pentru a le folosi in proiect

M am documentat despre SQLiteCpp si BD4S pentru crearea unei baze de date care sa fie folosita pentru stocarea utilizatorilor si
a grupurilor cu log create de utilizatori

# 19.06.2024

Adaugare componenta SQLlite pentru server goale , baza de date fiind doar pentru server + crearea unei scheme pentru baza de date

Am configurat o noua masina virtuala (linux mint) unde am instalat librariile Boost, Openssl, sqlite3, SQLiteCpp 

# 20.06.2024

Am creat baza de date si am adaugat si o diagrama cum arata 

Am creat componenta SQL care o sa faca conexiunea server->BD , cu niste functii de inserare si interogare + testarea in main a lor 

Am creat un MAKEFILE pentru a compila partea de server mai rapid 

# 21.06.2024

Am terminat de completat componenta SQL si am si testat o 

# 25.08.2024

Am creat componenta de server si am adaugat componetele de login/signup cu conexiune  la baza de date 

# 26.06.2024

Completarea functiilor de signup/login , + functii de monitorizare a utilizatorilor conectati
Modficare la cum "vorbeste" serverul si clientul din asincron -> sincron 
Modificarea bazei de date sa contina inca 2 tabele , una cu participantii la grupuri , si una cu DM urile

# 27.06.2024

Completare la Signup + login 

Completare componentei sql sa sustina noile tabele  + interogari noi 

Adaugare functie de Direct Message intre 2 utilizatori care sunt online 

Adaugare functie de a vizualiza toate mesajele primite , din BD

Adaugare functie de Create Group

Adaugare functie de Add Users to Group care adauga din utilizatorii case sunt online in grupurile create de cel care apeleaza functia 

Adaugare functie de trimitere a unui mesaj intr un grup de care utilizatorul apartine 

Adaugare functie de trimitere a tuturor mesajelor dintr un grup catre user 

Am completat componenta sql cu o functie care sa ia toate mesajele dintr un gerup , dupa id ul grupului