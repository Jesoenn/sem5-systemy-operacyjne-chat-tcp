# Wielowątkowy chat grupowy (TCP)

Projekt umożliwia komunikację wielu klientów za pomocą serwera TCP. Każdy klient podłączony do serwera może wysyłać oraz odbierać wiadomości grupowe w czasie rzeczywistym.


## Funkcjonalności

- Jednoczesna obsługa wielu klientów (każdy klient ma przydzielony swój wątek na serwerze)
- Wysyłanie i odbieranie wiadomości tekstowych
- UI klienta
- Wysyłanie historii wiadomości dla nowych użytkowników.

## Wykorzystane technologie

- **C++**
- **CMake**

## Uruchamianie serwera

```bash
so_projekt2 -s <ip> <port>
```

### Uruchamianie klienta

```bash
so_projekt2 -c[d] <ip> <port> <nick>
```

- `d` - tryb do testowania prędkości wysyłu wybranej ilości wiadomości w zależności od ich długości
