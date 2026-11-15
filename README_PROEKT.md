# Паралелно Монте Карло интегриране — C++ (OpenMP) срещу Java (java.util.concurrent)

Курсова работа М1-КР10-CJ. Пълният текст на заданието е в [ЗАДАНИЕ.md](ЗАДАНИЕ.md), а
избраният вариант (функция, интервал, параметри) — във [VARIANT.md](VARIANT.md).

## Структура

| Път | Съдържание |
|---|---|
| `cpp/src/` | C++ реализация: `rng.hpp` (PRNG), `function.hpp` (вариант), `monte_carlo.hpp` (seq + OpenMP), `main.cpp` (CLI, измерване) |
| `cpp/build.ps1` | Компилиране с g++ и OpenMP → `cpp/build/monte_carlo.exe` |
| `java/src/` | Java реализация: `Rng.java`, `Functions.java`, `MonteCarlo.java` (seq + ExecutorService), `Main.java` |
| `java/build.ps1` | Компилиране с javac → `java/out/` |
| `experiments/run_experiments.ps1` | Пуска цялата експериментална решетка (N × брой нишки × повторения) |
| `experiments/results/` | Сурови резултати, по един CSV файл на машина (`results_<машина>.csv`) |
| `analysis/analyze.py` | Обобщени таблици (`summary_*.csv`) и графики (`plots/`) |

## Изисквания (Windows)

- **g++ с OpenMP** — MSYS2 UCRT64 (`C:\msys64\ucrt64\bin` в `PATH`). OpenMP runtime-ът е
  отделен пакет; ако линкването дава `cannot find -lgomp`, инсталирайте го:
  ```
  C:\msys64\usr\bin\pacman.exe -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-libgomp
  ```
- **JDK 17+** (тествано с Temurin 21) — `javac` и `java` в `PATH`.
- **Python 3** с `pandas` и `matplotlib` — само за анализа: `pip install pandas matplotlib`.

Ако PowerShell забранява скриптове, за текущата сесия:
`Set-ExecutionPolicy -Scope Process Bypass`.

## Компилиране

От корена на проекта:

```powershell
.\cpp\build.ps1
.\java\build.ps1
```

## Стартиране на отделно изчисление

И двете програми приемат еднакви аргументи и извеждат еднакъв CSV формат:

```powershell
.\cpp\build\monte_carlo.exe --mode par --n 10000000 --threads 4 --repeats 5
java -cp java\out Main      --mode par --n 10000000 --threads 4 --repeats 5
```

| Аргумент | Значение | По подразбиране |
|---|---|---|
| `--mode` | `seq`, `par` или `verify` | `seq` |
| `--n` | брой проби | 1000000 |
| `--threads` | брой работни нишки (за `par`) | 1 |
| `--seed` | начална стойност на PRNG | 42 |
| `--repeats` | брой измервани повторения | 1 |
| `--warmup` | брой неизмервани загряващи пускания | 1 |
| `--machine` | етикет на машината в изхода | `unknown` |

Измерва се само изчислението на оценката; разборът на аргументите, загряването и
изчисляването на грешката не влизат в измереното време.

## Проверка, че пробите са еднакви в C++ и Java

```powershell
.\cpp\build\monte_carlo.exe --mode verify --seed 42 > v_cpp.txt
java -cp java\out Main --mode verify --seed 42 > v_java.txt
Compare-Object (Get-Content v_cpp.txt) (Get-Content v_java.txt)   # без изход = идентични
```

Отпечатват се 64-битовият хеш и битовете на получената равномерна проба за първите 10
индекса.

## Възпроизвеждане на експериментите

```powershell
.\experiments\run_experiments.ps1
```

Скриптът компилира двете реализации, записва описание на машината в
`experiments/results/machine_<машина>.txt` и добавя резултатите към
`experiments/results/results_<машина>.csv`. Параметрите по подразбиране са тези от
VARIANT.md. Броят нишки по подразбиране е 1, 2, 4, 8, 16 плюс точките, в които се сменя
видът на ядрата: P (ядра с hyper-threading = логически − физически), 2P, броят физически
ядра и броят логически процесори. Всичко може да се промени, напр.:

```powershell
.\experiments\run_experiments.ps1 -Machine laptopB -Sizes 1000000,100000000 -Threads 1,2,4 -Repeats 10
```

За по-стабилни времена: включен захранващ адаптер, режим High performance, затворени
други програми.

Не са нужни входни файлове — всички проби се генерират детерминирано от `seed` и индекса
на пробата.

## Анализ

```powershell
python analysis\analyze.py
```

Чете всички `experiments/results/*.csv` и създава:

- `analysis/summary_performance.csv` — медианно време, ускорение спрямо собствената
  последователна версия, ефективност, проби/сек;
- `analysis/summary_accuracy.csv` — оценка, абсолютна и относителна грешка спрямо π;
- `analysis/plots/` — графики на ускорение, ефективност, пропускателна способност и точност.

## Документация (Word)

Текстът на курсовата работа е в `docs/kursova.md`. От него се генерира `docs/kursova.docx`
със стиловете от `docs/reference.docx`:

```powershell
python -m pip install pypandoc_binary python-docx   # еднократно
python docs\build_docx.py
```

При първо отваряне Word пита дали да обнови полетата — отговорът е **Yes**, за да се
попълни съдържанието с номерата на страниците. PDF версията се прави от Word с
File → Save As → PDF. Стиловете се променят в `docs/make_reference.py` (след това
`python docs\make_reference.py` и отново `build_docx.py`).
