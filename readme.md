# DRAFT TREE lang'a
Tworzymy zmienną a, która jest dostępna jedynie w trybie statycznym, wartości
```
$a = 2
```
Zmienna może także przechowywać referencję
```
&b = getReference()
```

$b = &a // Zostanie utworzona kopia
&b = &a // Wskazują na ten sam objekt

a = 2 // Kompilator się domyśli o co nam chodzi, ale wyśle info, że możne warto to oznaczyć jako $a
a = getReference() // Tak jak wyżej, jest to poprawne, ale nie koniecznie dobre

b: 3 // Przypisanie symboliczne nie może stwiedzać czy jest referencją czy stałą

$c = b // Zostanie pobrana wartość z tego elementu w tym momenie, $c nie będzie się automatycznie aktualizować


d: a + b // jako że dla kompilatora a i tak jest stałe, to wartość ta jest równa $a + b i zależy tylko od b


showResult( a + b ) // jest to traktowane jako &a + $b

@showResult( a + b ) // Funkcja ta będzie nasłuchiwać na zmiany b i aktualizować się, można ją anulować jeżeli ją przejmiemy
handle: @showResult(a + b)
// albo
&handle = @showReult(a + b)

// Jest to duża zmiana odnośnie scss'a, gdzie jest na odwrót

// Czyli u nas będzie

if( a + b){} // Wywołuje się statycznie


@if( a + b) /* działa podobnie do media */

@media screen(width == 100) // działa podobnie jak

@if(screen.width == 100)

// Fajną metodą jest @use when
// Załóżmy, że mamy przełącznik darkMode który ustawia tryb na ciemny

@use ./darkMode.tree when darkMode is on // `is on` to inaczej `== true`
// Albo
@use ./darkMode.tree when :darkMode

// W takim przypadku, darkMode.tree będzie działać tylko wtedy kiedy mamy s

// Zdarzenia
// Zdarzenia to specjalny rodzaj zmiennych, które mogą działać jak funkcję

// To znaczy dosłownie: kiedy zmienna hover zostanie ustawiona na wartość prawdziwą
:hover{
    height: 100
}

// Możemy to wywołać
emit :hover

// Streamy
// Możemy przekazywać wynik operacji jednej funkcji do drugiej za pomocą operatora |>

getSize |> * 2 |> $var

// Jest tym samym co:
$var = getSize() * 2

// Można także wykonać polecenie dla wszystkich elementów za pomocą operatora ... (lub forEach) i połączyć za pomocą operatora [...] lub [_]

[a, b, c] |> ... |> *2 |> [_] |> $var

$var = [a * 2, b * 2, c * 2]

// Można prosto to zsumować
function sum(...arr){
    return arr.sum()
}


[a, b, c] |> ... |> sum |> $var

[a, b, c] |> forEach |> sum |> $var
$var = a + b + c


// Są także dostępne dynamiczne stramy :>

:input :>


// Zwracanie typów oraz nullable, error
// TREE posiada zaawansowaną obsługę błędów bazującą na zdarzeniach oraz

## Wartości puste
### Wbudowane warości puste
TREE posiada szerokie grono wartości `pustych`:
 - `void`
 - `undefined` `unset`, `undetermined`, `unreferenced`
 - `null`

**Te wartości nie są równe**

### `void`
`void` nie posiada wartości, można go użyć jako identyfikatora funkcji która nic nie zwraca a także w wyrażeniach które z natury coś zwracają, ale chcemy żeby nie miało to wpływu

**Przykłady:**
```php
$a = 3 if a > 3
```
zamiast `if` rozważmy naszą funkcję, która mogłaby zrobić to samo

```php
operator (number a) ourX (test x) just before (=){
    if ( x ) {
        return a
    } else {
        return void
    }
}
```
Funkcja ta zwraca naszą liczbę `a` jeżeli `x` został spełniony, w przeciwnyn razie zwraca `void`

`void` zatrzymuje działanie każdego operatora i zwaraca `void`. To znaczy, że przypisanie w tej linie nie wykona się

Typu `void` nie da się przypisać do zmiennej, zostanie ona dalej nieustawiona


### `undefined` (`unset`, `undetermined`, `unreferenced`)
Te cztery warości służą do przekazania wartości, która w jakiś sposób nie została ustalona

```js
print a // undefined
```

```js
int a
print a // unset
```

```js
x = new Object()
a = &x
delete x
print a // unreferenced
```

```js
a: 2 + b
print a // undetermined
```

Trzy pierwsze z tych wyrażeń możemy uzyskać za pomocą operatorów i przypisania

`undefined`:
```js
int a = 10
a = undefined
print a // undefined

// Albo

int a = 10
free a
print a // undefined
```
**Zmiennej niezdefiniowanej nie dotyczą zasady dziedziczenia**

`unset`:
```js
int b: 10
b: unset
printf b // unset

// Z dziedziczeniem
int b: 10
{
    b: 20
    b: unset
    print b // 10
}
```

### `null`
`Null` jest konceptem, który mówi, że część zmiennych może mieć z zasady nieustaloną wartość takie zmienne nazywamy `nullable`. Koncept ten został przeniesiony wraz ze słowem kluczowym `null` w celu kompatybilności z `SQL`'em

W TREE taki typ możemy oznaczyć poprzez `| null` albo w skrócie prefixem `?`

```php
int a = 2
?int b = null
int | null c = null
```
`null` jest typem samym w sobie

## Tworzenie języka, czyli nie tylko `function`
### Standardowe wyrażenie
W `tree` możemy zdefiniować stworzyć dowolne wyrażenie, wraz ze sposobem jego parsowania. Przykładowo:
```php
expression select (Array what) from (Table& | string table){
    if table is string {
        table = database.getTable(table)
    }
    return table.get(what)
}
```

### Wyrażenia z dwoma argumentami
Szybszym zapisem wyrażenia jedynie z dwoma argumentami jest `operator`
```php
operator (int& a) + (int& b){}
```

Kolejność operatorów możemy przekazać za pomocą tagów, albo tagów wewnętrznych
```php
// Używamy przypisania dynamicznego, bo kolejność operatorów może się zmieniać w trakcie działania programu, i chcemy żeby zawsze to było spełnione

#[priority: just before(=), after(*), like(-)]
#[join: left]
operator (int a) + (int b){}

```

Cechy te możemy sprawdzić później za pomocą `refleksji`

```php
from reflect operator + get priority
// Albo mniej angielsko xD

reflect(operator +).priority
```

Funkcja reflekst przyjmie każde znane wyrażenie

### `Unary operator` and `function`
Jeżeli chcemy stworzyć operator jednoargumentowy, to możemy stworzyć go w sposób operatorowy albo funkcyjny

```php
operator ! (bool s){
    if s == true
        return false
    else
        return true
}
```
Albo to samo jako funkcja

```php
function ! (bool& s){
    if s == true
        return false
    else
        return true
}
```

Ogólnie przyjętą normą jest to... że słowa operator używamy wtedy, gdy jest to operator xD


`Funkcje` same z siebie z założenia przyjmują jedną grupę argumentów. Specjalnym rodzajem funkcji jest `test`. Funkcja ta przyjmuje jeden argument i zwraca wartość logiczną `nienulowalną` czyli `bool`

**Przykłady:**
```php
function helloWorld(){
    print 'Hello world'
}
```
```php
function even(int& a){
    return a % 2 == 0
}
```

Funkcji testowych `test` używamy zazwyczaj z wyrażeniem `is`

**Przykład:**
```php
123 is odd

// Albo, bardziej po angielsku
is 123 odd

odd 123

odd(123)
```

Wszystkie powyższe funckcje dają ten sam wynik, i działają pod spodem tak samo

Prosta implementacja `is`, aby zobrazować jak działą to wyrażenie

```php
// operator is dla pierwszego przykładu
operator (number a) is (test testFunction){
    return testFunction(a)
}

// funkcja dla odwróconej kolejności
function is(number a, test testFunction){
    return testFunction(a)
}
```

W ten sposób zaimplementowaliśmy działający operator `is` (wprawdzie jedynie dla liczb, ale to tylko przykład)

## Przekazywanie argumentów do funkcji
W TREE można argumenty przekazać w postaci `listy` bądź `luźniej listy`


### Ciekawostka
`return` jest także wyrażeniem, które ustawia wartość $#result na podaną wartość
### `Luźna lista`
Jest zawsze konwertowana do faktycznej listy (`list`) nie jest zatem typem, a rodzajem wyrażenia listy

Luźna lista posiada kolejne wartości oddzielone spacją a dodatkowo większe sekcje mogą być odzielone przecinkiem przykładowo

```scss
List a = 123 234, 232 23
// Jest identyczne z:
List a = ((123 234), (232, 23))
```
Luźna lista nie jest jednak zawsze tłumaczona w sposób dosłowny, jest tam mechanizm zwany `typowym dopasowaniem argumentów`

**Luźne listy możemy stosować jedynie w wyrażeniach i przypisaniach. Linia kodu która jest luźną listą jest błędna!**
## Dopasowanie argumentów jako `luźnej listy`

Zdefinujmy sobie funkcję:
```php
function a(int i, string s, float f, number n, date d, time t){}
```
możemy ją wywołać następująco:
```php
a(10, "Jakiś tekst", 1.0, 0x12, 2024-02-03, 12:45)
```
W ten sposób podajemy jawnie argumenty, natomiast możemy ją także wywołać tak:
```php
a 2024-02-03 12:45 1.0 10 0x12 "Jakiś tekst"
```
Tak wywołana funkcja zadziała dokładnie tak samo dlaczego? Skoro argumenty są podane w innej kolejności. Za ten stan rzeczy odpowiada `mechanizm przypisania argumentów`

> Mechanizm ten działa także w przypadku uruchamiania programów, o tym więcej możesz poczytać [...]

### Algorytm przypisywania argumentów
**Dopasowanie bezpośrednie**

Bierzemy argumenty i tworzymi z nich tabelkę typ - warość

| **Typ**   | **Argument(y)**   |
| ---       | ---               |
| `date`    | `2024-02-03`      |
| `time`    | `12:45`           |
| `float`   | `1.0`             |
| `int`     | `10`, `0x12`      |
| `string`  | `"Jakiś tekst"`   |

Następnie bierzemy nasze argumenty które oczekujemy, i jeżeli jakiś argument się pokrywa to go ściągamy z tabelki i kładziemy w liście

```
i = 10
s = "Jakiś tekst"
f = "1.0"
n = ?
d = 2024-02-03
t = 12:45
```

Widzimy, że po tym kroku nie przypisaliśmy jedynie n z typem `number`

**Rzutowanie w górę**

Drugim krokiem jest przypasowanie typów przez `rzutowanie w górę`

> `Rzutowanie w górę` jest mechanizmem rozwiązywania `typu alternatywy`

Typ number to tak na prawdę `int | float | complex ` a zatem, możemy w jego miejscę w stawić pozostałego `int`'a

**Rzutowanie automatyczne**

Trzecim i ostatnim krokiem jest rzutowanie pozostałych elementów za pomocą zdefiniowanych opecji operatora rzutowania

### Nazwane właściwości
W przypadku zarówno `list` jak i `luźnych list` możemy używać nazwanych właściwości

Bazując na funkcji z poprzedniego podpunktu, można zrobić:
```php
a i = 10 f = 1.0 s = "Jakiś tekst" t = 12:45 d = 2024-02-03
```

W przypadku imperatywnych wyrażeń, nie ma znaczenie użycia opertaora `=` lub `: ` natomiast najlepiej używać `=` dla przejżystości

## Własne typy danych
Typy danych możemy podzielić na kilka sposobów
### `Typy kontenerowe`, `Typy strukturalne`, `Typy alternatywy` `Typy flagi` oraz `Typy odroczonej egzekucji`
Rozważmy dwa typy
`rgba` oraz `Human`
Samodzielność pól jest zasadą tłumaczącą kiedy w nazwie typu należy używać `PascalCase` a kiedy `camelCase`

Zasada ta brzmi:
> Typ składający się z równoważnych sobie pod względem użyteczności części nazywamy typem zbiorowym, natomiast składającym się z cech jego-opisujących nazywamy typem strukturalnym

A tak po ludzku?

**Na początku typy kontenerowe**

Rozważmy typ `rgba`. Składa się on z 4 `byte`'ów dla każej składowej koloru jego definicja mogłaby wyglądać tak:

```php
container rgba{
    byte red
    byte green
    byte blue
    byte alpha = 100%
}
```

Każda z jego składowych jest dla nas tak samo ważna, i zachowywać będzie się podobnie. Tak samo jest z np. czasem:
```php
container time{
    byte char
    byte minutes
    ?byte secounds
}
```
Szczególnym przypadkiem takiego typu jest `array` oraz `list`, które nie posiadają określonej liczby elementów, oraz wszelkie typy prymitywne, które posiadają jedynie jedno pole z samym sobą. Taki typ prosty, możemy stworzyć kożystając z wyrażenia `type`

```php
type fancyText = string
```

```php
type ints = []int

// To samo co

type ints = array<int>

// Albo tak też można, ale nie ma po co

type ints{
    array<int>
}
```

**Typy strukturalne**

W `TREE` mamy trzy podstawowe typy strukturalne: `struct`, `class` oraz `node`

- `struct` posiada jedynie `pola cech`
- `class` posiada dodatkowo `metody manipulacji danymi`
- `node` posiada to co `class` oraz dodatkowe pole specjalne `children` umożliwiające traktowanie go także jako kontener

**Typy alternatywy (unie)**

```php
union number = int | float | complex
```

**Typy flagi i wyliczeniowe**

To typy, które nie przechowują wartości a jedynie samą siebie

Tworzymy je, za pomocą wyrażenia `flag`
```php
flag isGreen = true

// Lub

bool flag isGreen = true
```
Użycie:
```php
function a(isGreen = false){
    print isGreen
}

a // false

a isGreen // true
```

Można także stworzyć wiele elementów na raz za pomocą słowa `enum`
```php
// Anonimowy enum dla wartości rgba
enum{
    red = rgba(255, 0, 0)
    green = rgba(0, 255, 0)
    blue = rgba(0, 0, 255)
    black = rgba(0, 0, 0)
    white = rgba(255, 255, 255)
}
```

Użycie:
```php
struct Box{
    int width
    int height
    rgba background
}

box = Box(100, 100, red)
```
