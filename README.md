# Projekt studencki z Przetwarzania Rozproszonego

Program wielowątkowy napisany w C++ z wykorzystaniem biblioteki MPI symulujący dostęp do zasobów. Implementacja algorytmu Lamporta, którego wykorzystuje się do rozwiązania problemu wzajemnego wykluczania.

### Rozwiązywany problem

Dany jest system, w którym są repliki dokumentów i N procesów. Replik jest zawsze co najwyżej K<N (może być mniej, zawsze co najmniej 1).
Procesy działają w pętli: ubiegają się o replikę, jakiś czas jej używają w trybie do odczytu (współbieżnie) albo zapisu (tylko jeden proces może mieć replikę), a potem mogą ją zwolnić od razu, albo dopiero po tym, gdy jakiś inny proces będzie się ubiegał. Należy zapewnić jednak, że zawsze co najmniej jeden proces będzie posiadał replikę.
