# BarsikCMD Changelog

# Версия 1.0 "First meow"
## Добавлено
1. Консоль (главное)
2. Возможность обновления через команду (`BarsikCMD -update`)
3. Возможность обновления через файл обновления в формате `.bcu`
4. Сам обновитель
5. Установщик на базе NSIS
6. Установщик на базе Inno Setup
7. Установщик без UI
## Изменено
<!---<style> 
    /* А кто запрещал встраивать стили в markdown? */
    nothing {
        color: #9b9b9b;
        font-family: monospace;
        font-size: 120%;
    }
</style>-->
<!--<nothing>(Ничего)</nothing>-->
(Ничего)
## Удалено
(Ничего)
## Исправлено
(Ничего)
## Исправления безопасности
(Отсутствуют)
## Файлы релиза
| Файл | Тип | Назначение | Скачать |
|-|-|-|-|
| `BarsikCMD-1.0-NSIS-installer.exe` | EXE (NSIS) | Установщик BarsikCMD на базе NSIS | [Клик](https://github.com/barsik0396/BarsikCMD/releases/download/1.0/BarsikCMD-1.0-NSIS-installer.exe) |
| `BarsikCMD-1.0-InnoSetup-installer.exe` | EXE (Inno Setup) | Установщик BarsikCMD на базе Inno Setup | [Клик](https://github.com/barsik0396/BarsikCMD/releases/download/1.0/BarsikCMD-1.0-InnoSetup-installer.exe) |
| `BarsikCMD-1.0-nogui-installer.exe` | EXE (Inno Setup) | Установщик BarsikCMD без UI | [Клик](https://github.com/barsik0396/BarsikCMD/releases/download/1.0/BarsikCMD-1.0-nogui-installer.exe) |
## Подробнее
### Добавления
1. Консоль - в BarsikCMD была добавлена консоль с кучей команд. Консоль это главное в BarsikCMD.
2. Возможность обновления через команду - была добавлена команда `BarsikCMD -update` чтобы выполнять обновления.
3. Возможность обновления через файл обновления - теперь при установке будет регистрироваться формат `.bcu`, при открытии которого (двойной клик или через `BarsikCMD -update -file <путь_или_имя_файла>`) будет выполнено чтение файла и обновление.
4. Обновитель - теперь BarsikCMD будет поддерживать обновитель и его подкоманды.
5. Установщик на базе NSIS - Недавно был создан установщик созданный через NSIS. Он достаточно удобный, но не настолько как Inno Setup.
6. Установщик на базе Inno Setup - теперь можно установить BarsikCMD через установщик созданный через один из популярнейших install-builder'ов с названием Inno Setup. Данный установщик достаточно удобный, поймёт даже новичок.
7. Установщик без UI - был создан кастомный установщик который работает полностью без UI, а в консоли. 
### Использование обновителя
1. Показ помощи
```bash
BarsikCMD -update -help
```
2. Показ уже установленных обновлений
```bash
BarsikCMD -update -list-installed
```
3. Показ списка обновлений доступных для установки
```bash
BarsikCMD -update -list-can-install
```
4. Установка указанного обновления по ID
```bash
BarsikCMD -update -install --id ИД_ОБНОВЛЕНИЯ
```
5. Установка всех доступных для установки обновлений
```bash
BarsikCMD -update -install ALL
```
6. Создание точки восстановления
```bash
BarsikCMD -update -recovery-point create 'ИМЯ_ТОЧКИ_ВОССТАНОВЛЕНИЯ' 'ID_ТОЧКИ_ВОССТАНОВЛЕНИЯ'
```
7. Восстановление через созданную точку восстановления
```bash
BarsikCMD -update -recovery-point rollback 'ID_ТОЧКИ_ВОССТАНОВЛЕНИЯ'
```
8. Показ списка точек восстановления
```bash
BarsikCMD -update -recovery-point list
```
