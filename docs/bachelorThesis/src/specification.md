# Specifikace bakalářské práce
**Platforma pro vizualizaci a ovládání různých typů robotických podvozků** 
## Úvod
Bakalářská práce navazuje na můj ročníkový projekt: Knihovna pro ovládání různých typů robotických podvozků dále jen jako [*Move library*](https://gitlab.mff.cuni.cz/venzarv/rocnikovy-projekt). Práce bude dále projekt rozvíjet a přidávat mu další funkce.
Cílem bakalářské práce je vytvořit aplikaci, která umožní simulaci jízdy robotických podvozků, komunikaci a zobrazování dat z *MoveLibrary*, vytváření konfiguračních souborů robota pro *MoveLibrary* a vizualizaci robota z konfiguračního souboru.

Aplikace bude jednoduchá pro ovládání s dobrou a pochopitelnou dokumentací, protože bude používaná na výuku a ukázku pohybu robotů žákům gymnázií a středních škol.

## Aplikace
### Technologie
Předběžný list technologií, které aplikace k naprogramování aplikace použiji:
- Hlavní jazyk aplikace: **C++**
- Vizualizace: **SFML**
- GUI: **ImGui + ImGui-SFML**
- Komunikace: **Boost.Asio** nebo **libserial**
- Build: **CMake**

### Funkce aplikace
#### Editor konfigurací
Umožní vytváření a úpravu konfiguračních souborů robotických podvozků.
Tyto soubory budou použitelné jak v simulátoru, tak při reálném řízení robota přes *MoveLibrary*.
#### Vizualizátor podvozku
Na základě konfiguračního souboru vytvoří model podvozku robota.
Uživatel si tak může ověřit správnost konfigurace (počet kol, rozměry, umístění motorů apod.) a popřípadě ji upravit.
#### Simulátor pohybu
Umožní simulaci pohybu robota podle nahrané konfigurace a příkazů API, které odpovídá rozhraní *MoveLibrary*.
Simulace zobrazí trajektorii, rychlost, rotaci i případné chyby.

*V jakém jazyce psát simulační program?*

#### Komunikace s *MoveLibrary*
 Zajistí komunikaci s knihovnou, za pomocí seriové linky, popřípadě Wi-FI nebo bluetooth. 
- **Řízení robota**
  Pomocí komunikace s knihovnou zajistí přenos příkazů robotovi, který je následně vykoná v reálném světě.
- **Vizualizace dat pohybu robot**
  Zajistí přijímání dat z robota o rychlosti a akceleraci. Tyto data pak zobrazí v reálném čase na simulaci robota.


## Kapitoly a obsah práce
#### Úvod do problematiky
Seznámení se systémy a aplikacemi co už existují a vysvětlení, proč jsou pro vzdělávací účely příliš složité nebo nevhodné.
#### Vysvětlení druhů podvozků a matematiky za nimi
Popis a srovnání typů podvozků:
- Diferenciální 
- Omni wheel
- Mecanum wheel
  
Každý typ bude doprovázen matematickým modelem (převod mezi rychlostí kol a pohybem robota) a krátkou analýzou výhod a nevýhod.

#### Popis fungování aplikace
Popsat fungování a architekturu aplikace. Vysvětlit použité způsoby komunikace s *Move Library* a využití knihovny v aplikaci.
Popsat možnosti aplikace a vysvětlit její funkce.
Popsat vývoj a problémy, které se vyskytly.

#### Testování a ověření funkčnosti
Popis testovacích scénářů (např. porovnání simulované a skutečné trajektorie).

Hodnocení použitelnosti při výuce.

#### Závěr
Shrnutí výsledků práce.

#### Výstupy
- Zdrojový kód aplikace
- Funkční simulační aplikace
- Dokumentace a manuál pro uživatele
