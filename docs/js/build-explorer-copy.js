const texts = {
    "1.0-ps": 'Invoke-WebRequest "https://github.com/barsik0396/BarsikCMD/releases/download/1.0/BarsikCMD-1.0-NSIS-installer.exe" -OutFile "InstallBarsikCMD.exe" | powershell -NoProfile -NoLogo -Command ".\InstallBarsikCMD.exe /S"',
    //"btn2": "Текст для второй кнопки"
};

document.querySelectorAll("button").forEach(button => {
    button.addEventListener("click", () => {
        navigator.clipboard.writeText(texts[button.id]);
    });
});