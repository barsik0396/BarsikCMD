#!/bin/bash

set -euo pipefail

# --- Настройки ---
GPG_KEY_ID=""   # Укажи ID GPG-ключа или оставь пустым для ключа по умолчанию
DEB_FILE="${1:-}"

# --- Проверки ---
if [[ -z "$DEB_FILE" ]]; then
    echo "Использование: $0 <path/to/package.deb>" >&2
    exit 1
fi

if [[ ! -f "$DEB_FILE" ]]; then
    echo "Ошибка: файл '$DEB_FILE' не найден." >&2
    exit 1
fi

if ! command -v debsigs &>/dev/null; then
    echo "Ошибка: debsigs не установлен. Установи: sudo apt install debsigs" >&2
    exit 1
fi

if ! command -v gpg &>/dev/null; then
    echo "Ошибка: gpg не установлен. Установи: sudo apt install gnupg" >&2
    exit 1
fi

# --- Определяем ключ ---
if [[ -z "$GPG_KEY_ID" ]]; then
    GPG_KEY_ID=$(gpg --list-secret-keys --with-colons 2>/dev/null | awk -F: '/^sec/{print $5; exit}')
    if [[ -z "$GPG_KEY_ID" ]]; then
        echo "Ошибка: GPG-ключ не найден. Создай ключ: gpg --gen-key" >&2
        exit 1
    fi
    echo "Используется ключ: $GPG_KEY_ID"
fi

# --- Подпись ---
echo "Подписываю: $DEB_FILE"
debsigs --sign=origin -k "$GPG_KEY_ID" "$DEB_FILE"
echo "Пакет успешно подписан."

# --- Проверка ---
echo "Проверяю подпись..."

TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

ar p "$DEB_FILE" _gpgorigin > "$TMPDIR/sig" 2>/dev/null || {
    echo "Ошибка: подпись не найдена в пакете." >&2
    exit 1
}

ar t "$DEB_FILE" | grep -v '_gpgorigin' | while read -r member; do
    ar p "$DEB_FILE" "$member" >> "$TMPDIR/data"
done

if gpg --verify "$TMPDIR/sig" "$TMPDIR/data" 2>&1; then
    echo "Подпись действительна."
else
    echo "Ошибка: проверка подписи не прошла!" >&2
    exit 1
fi