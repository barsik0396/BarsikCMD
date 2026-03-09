const url = window.location.href;
if (url === "https://barsik0396.github.io/BarsikCMD/builds/") { // Скорее всего - проблема в том что GitHub автоматически добавляет /, а у меня не указан /, и получается вечная "загрзука"
    setTimeout(() => {
        window.location.replace("https://barsik0396.github.io/BarsikCMD/build-explorer");
    }, 2000); // 2 секунды
} else if (url === "http://127.0.0.1:3000/docs/builds/") {
    setTimeout(() => {
        window.location.replace("http://127.0.0.1:3000/docs/build-explorer.html");
    }, 2000);
}