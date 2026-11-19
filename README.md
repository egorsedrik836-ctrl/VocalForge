# VocalForge — полная вокальная цепочка (VST3 для FL Studio)

Один плагин вместо десятка: **чистка → цвет → пространство → лимитер**.

```
Trim > Mono > HPF > Gate > Hiss > Mud Tamer > Rider > Comp 1 > De-Esser > Comp 2
     > EQ (4 полосы) > Saturation > Exciter > Doubler > Delay > Reverb > Out > Limiter
```

## Что внутри (78 параметров, 7 пресетов)

**CLEAN** — High-pass (12/24 dB), Noise Gate, Hiss Reducer, Mud/Resonance Tamer (динамический), Vocal Rider (автоматический райдер громкости), De-Esser (с режимом Listen)
**COLOR** — тональный EQ (low shelf, low-mid, presence, air), два компрессора (Leveler + Peak), насыщение (Tube / Tape / Crunch, с anti-aliasing), Exciter
**SPACE** — Doubler, стерео Delay (синхронизация с темпом проекта, ping-pong), Reverb (FDN, 8 линий)
**MASTER** — Input Trim, Mono In, Output Gain, Limiter (look-ahead, 1.5 мс), индикаторы уровней и GR
Пресеты: меню «Load preset…» вверху окна (Clean Pop, Rap Upfront, Airy Female, Warm Male, Podcast, Lo-Fi Radio).

> Плагин вносит задержку ~1.5 мс (look-ahead лимитера). FL Studio компенсирует её автоматически.

---

## Как получить плагин (ничего устанавливать не нужно)

Готовый `.vst3` я собрать не мог (в моей среде нет Windows-компилятора и интернета), поэтому сборку делает бесплатный сервер GitHub.

1. Зарегистрируйся на **github.com** (бесплатно).
2. Нажми **+ → New repository**, имя `VocalForge`, выбери **Public**, **Create repository**.
3. Распакуй zip. На странице репозитория нажми **uploading an existing file**.
4. Перетащи в окно **содержимое** распакованной папки (CMakeLists.txt, папки `Source`, `.github`, `tests` и т.д. — НЕ саму папку-родителя). Внизу **Commit changes**.
5. Открой вкладку **Actions** → сборка стартует сама («Build VST3 (Windows)»). Ждать ~10–15 минут, пока не появится зелёная галочка.
6. Открой завершённый запуск → внизу **Artifacts** → скачай `VocalForge-VST3-Windows`.
7. Распакуй и положи папку **`VocalForge.vst3`** в `C:\Program Files\Common Files\VST3\`.
8. FL Studio: **Options → Manage plugins → Find installed plugins**. Потом в слот микшера → **Select plugin → VocalForge**.

Если после шага 4 вкладка Actions пустая (папку `.github` браузер мог не загрузить): **Add file → Create new file**, имя файла введи как `.github/workflows/build.yml`, вставь туда содержимое файла `.github/workflows/build.yml` из архива и сделай Commit.

## Запасной вариант: собрать у себя на ПК

1. Запусти `1_install_tools.bat` (ставит CMake, Git и Visual Studio Build Tools через winget; это несколько ГБ).
2. Закрой окно. ПКМ по `2_build_and_install.bat` → **Запуск от имени администратора**. Скрипт соберёт плагин и сам положит его в папку VST3.

## Если сборка упала

Скопируй красный текст ошибки из лога (в GitHub: Actions → запуск → шаг «Build») и пришли мне — поправлю.

## Что проверено

DSP-ядро (`Source/dsp`) написано без зависимостей от JUCE и протестировано здесь: `tests/dsp_test.cpp` (g++ + AddressSanitizer/UBSan) — 13 групп проверок: HPF −24 дБ/окт, гейт (−40 дБ шума, сигнал не трогает), статика компрессора, де-эссер (−12 дБ на 7 кГц, 200 Гц не трогает), EQ ±6 дБ, лимитер строго ≤ потолка, реверб RT60, эхо дилея в нужный сэмпл, стабильность при экстремальных настройках (фидбэк 90%, реверб 8 с), 44.1/88.2/96 кГц, валидность всех пресетов.

**Не проверено:** сама JUCE-обвязка (окно, ручки, подключение параметров) пока не компилировалась с настоящим JUCE, только с упрощённой заглушкой. Звук в FL Studio на слух я тоже проверить не мог: пресеты — разумные стартовые точки, подстраивай под голос.
