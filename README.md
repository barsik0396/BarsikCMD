# BarsikCMD — Командная строка Барсика 😼

![License](https://img.shields.io/badge/license-MIT-green)
![Platform](https://img.shields.io/badge/platform-Linux-blue)
![Version](https://img.shields.io/badge/version-2026.3.4-orange)
![Nightly](https://img.shields.io/badge/nightly-2026.3.5--nightly--1-purple)
![Status](https://img.shields.io/badge/status-active-success)
![Open Source](https://img.shields.io/badge/open--source-yes-brightgreen)

**BarsikCMD** — лёгкая, быстрая и полностью open-source командная строка для Linux, созданная как альтернатива стандартным терминалам.  
> [!NOTE]
> Все силы разработки BarsikCMD были переданы на разработку [BarsikCMD-barsik](https://github.com/BarsikCMD/BarsikCMD-barsik).
>
> Обновления BarsikCMD будут очень редко.


Код открыт полностью: можно смотреть, изменять, форкать — лицензия это разрешает.

---

## ❓ Почему BarsikCMD?

**Быстрота** — запускается мгновенно, работает без задержек, обновляется часто.

**Простота** — никаких сложных настроек. Установка занимает несколько секунд, команды интуитивные.

**Приватность** — BarsikCMD не собирает данные: нет телеметрии, нет истории команд. Сеть используется только для загрузки обновлений по явной команде.

**Открытость** — исходники доступны полностью в папке `code`.

---

## 🚀 Быстрый старт

1. Скачайте последний релиз: [Releases](https://github.com/Barsik0396/BarsikCMD/releases)
2. Установите `.deb` пакет.
3. Запустите:
   ```bash
   barsikcmd
   ```

Подробная инструкция по установке: [INSTALL.md](INSTALL.md)

---

## 📰 Последние новости
### Планируется 2й nightly-релиз
(Примерно выйдет ~29-31 марта)


### Nightly-релиз 2026.3.5 (n1) вышел!
Официально вышел Nightly-релиз 2026.3.5-nightly-1!
Изменения:
#### Добавлено
Команда `file`
#### Скачать:
- Multi-ISO: [тут](https://barsikcmd-dev.pages.dev/downloads/BarsikCMD-Multi-ISO_2026.3.5.nightly-1.iso)

---

## ◼️ Команды

> Формат: `команда [необязательный] <обязательный>`

| Команда  | Использование  | Описание                  |
|----------|----------------|---------------------------|
| `clear`  | `clear`        | Очистить экран            |
| `echo`   | `echo [текст]` | Вывести текст             |
| `exit`   | `exit`         | Выйти из BarsikCMD        |
| `help`   | `help`         | Список команд             |
| `update` | `update`       | Обновить BarsikCMD        |
| `ver`    | `ver`          | Показать версию           |

---

## 🆕 Последние релизы

| Версия   | Название      | Что нового                                | Статус | Скачать |
|----------|---------------|-------------------------------------------|--------|---------|
| 2026.3.4 | Big update    | Новые команды, аргументы, фиксы           | ✅     | [DEB](https://github.com/barsik0396/BarsikCMD/releases/download/2026.3.4/barsik-cmd_2026.3.4_amd64.deb) · [Release](https://github.com/barsik0396/BarsikCMD/releases/tag/2026.3.4) |
| 2026.3.3 | Update fix    | Исправлено поведение команды `update`     | ✅     | [DEB](https://github.com/barsik0396/BarsikCMD/releases/download/2026.3.3/barsik-cmd_2026.3.3_amd64.deb) · [Release](https://github.com/barsik0396/BarsikCMD/releases/tag/2026.3.3) |
| 2026.3.2 | First update  | Команда `update` и цветной вывод          | ⚠️     | [DEB](https://github.com/barsik0396/BarsikCMD/releases/download/2026.3.2/barsik-cmd_2026.3.2_amd64.deb) · [Release](https://github.com/barsik0396/BarsikCMD/releases/tag/2026.3.2) |
| 2026.3.1 | First release | Первый релиз                              | ❌     | [DEB](https://github.com/barsik0396/BarsikCMD/releases/download/2026.3.1/barsik-cmd_2026.3.1_amd64.deb) · [Release](https://github.com/barsik0396/BarsikCMD/releases/tag/2026.3.1) |

---

## ❔ FAQ

<details>
<summary>Почему только Linux?</summary>

BarsikCMD разрабатывается на Linux Mint для Linux. Windows-версия не планируется.
</details>

<details>
<summary>Есть ли документация?</summary>

Да: [Wiki](https://github.com/barsik0396/BarsikCMD/wiki)
</details>

<details>
<summary>Когда появится язык автоматизации?</summary>

Планируется в версиях **2026.3.5 — 2026.4.0**.  
Расширение для VS Code — примерно **2026.5.4 — 2026.6.1**.
</details>

---

## 📄 Дополнительно

- [История создания](Creating.md)
- [Философия проекта](Philosophy.md)
- [Changelog](CHANGELOG.md)
- Исходники: `/code`