# Пускане на експериментите на Лаптоп 2 (Windows)

**Накратко:** Code → Download ZIP → разархивирай → PowerShell в папката →
`Set-ExecutionPolicy -Scope Process Bypass` и `.\experiments\run_experiments.ps1 -Machine laptop2`
→ прати двата файла от `experiments\results\`.

Подробно по-долу.

## 1. Еднократна подготовка

**Java (JDK 17 или по-нов)**

Проверка в PowerShell: `java -version` и `javac -version`.
Ако липсва: https://adoptium.net → Temurin 21 (LTS), Windows x64 `.msi`, с включено „Add to PATH“.

**g++ с OpenMP (MSYS2)**

Проверка в PowerShell: `g++ --version`. Ако липсва:

1. Инсталира се MSYS2 от https://www.msys2.org (по подразбиране в `C:\msys64`).
2. В PowerShell:
   ```
   C:\msys64\usr\bin\pacman.exe -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-libgomp
   ```
3. `C:\msys64\ucrt64\bin` се добавя към PATH
   (Start → „Edit the system environment variables“ → Environment Variables → Path → New).
4. PowerShell се затваря и отваря наново.

Ако g++ го има, но при компилиране излиза `cannot find -lgomp`, се пуска само командата с
`pacman` от точка 2.

Python **не** е нужен.

## 2. Пускане

- Лаптопът е на захранване, другите програми са затворени, режим „Best performance“.
- Горе вдясно на тази страница: зелен бутон **Code → Download ZIP**, после се разархивира.
- В разархивираната папка (тази, в която има `experiments`, `cpp`, `java`):
  Shift + десен бутон → **Open PowerShell window here** (или Open in Terminal) и:

  ```
  Set-ExecutionPolicy -Scope Process Bypass
  .\experiments\run_experiments.ps1 -Machine laptop2
  ```

- Отнема около 5–10 минути; докато върви, лаптопът не се ползва.
- Накрая пише `Results appended to experiments\results\results_laptop2.csv`.

## 3. Какво се изпраща обратно

Двата файла от `experiments\results\`:

- `results_laptop2.csv` — суровите измервания;
- `machine_laptop2.txt` — процесор, ядра, RAM, версии на g++ и Java.

Ако нещо се обърка — снимка на последните редове в PowerShell.
