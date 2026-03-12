!include "MUI2.nsh"
Name "BarsikCMD"
Caption "Установка BarsikCMD"
UninstallCaption "Деинсталляция BarsikCMD"
InstallDir "C:\BarsikCMD"
RequestExecutionLevel admin
#BGGradient 000000 FFFFFF FFFFFF

!define MUI_ABORTWARNING_TEXT "Вы действительно хотите отменить установку?"
!define MUI_WELCOMEPAGE_TITLE "Добро пожаловать!"
!define MUI_WELCOMEPAGE_TEXT "Вас приветствует установщик проекта BarsikCMD.$\n$\r$\n$\rПеред началом установки рекомендуется проверить, не открыт ли BarsikCMD.$\n$\r$\n$\rЧтобы начать установку нажмите 'Далее' или 'Отмена' чтобы отменить установку."

!define MUI_LICENSEPAGE_TEXT_BOTTOM "Если вы принимайте лицензионное соглашение, нажмите на чекбокс ниже, а затем 'Продолжить'."
!define MUI_LICENSEPAGE_TEXT_TOP "Пожалуйста, внимательно прочитайте лицензионное соглашение BarsikCMD."
!define MUI_LICENSEPAGE_CHECKBOX_TEXT "Я принимаю лицензионное соглашение"
#!define MUI_LICENSEPAGE_CHECKBOX
!define MUI_LICENSEPAGE_BUTTON "Продолжить"

!define MUI_COMPONENTSPAGE_TEXT_COMPLIST "Выберите компоненты BarsikCMD, которые хотите установить:"
!define MUI_COMPONENTSPAGE_TEXT_DESCRIPTION_INFO "Наведите курсором на компонент, чтобы увидеть описание."
!define MUI_COMPONENTSPAGE_TEXT_DESCRIPTION_TITLE "Описание:"
!define MUI_COMPONENTSPAGE_TEXT_TOP "Выберите компоненты, которые вы хотите установить для программы BarsikCMD. После выбора нажмите 'Далее' для продолжения."

!define MUI_DIRECTORYPAGE_TEXT_DESTINATION "Путь установки BarsikCMD:"
!define MUI_DIRECTORYPAGE_TEXT_TOP "Выберите папку установки BarsikCMD (туда будет установлен BarsikCMD). Нажмите 'Обзор...' чтобы выбрать папку, а затем нажмите 'Установить' для начала установки."

!define MUI_INSTFILESPAGE_PROGRESSBAR "colored"

!define MUI_FINISHPAGE_BUTTON "Завершить!"
!define MUI_FINISHPAGE_LINK "Сайт-проводник BarsikCMD"
!define MUI_FINISHPAGE_LINK_COLOR "29B5BE"
!define MUI_FINISHPAGE_LINK_LOCATION "$INSTDIR\media\web\home.html"

!define MUI_FINISHPAGE_RUN "$INSTDIR\BarsikCMD-prof.exe"
#!define MUI_FINISHPAGE_RUN "$INSTDIR\Uninstall.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Запустить BarsikCMD"

!define MUI_ABORTWARNING

Function .onInit
    # your code here
    ReadRegStr $2 HKLM64 "Software\BarsikCMD" "installed"
    ${If} $2 == "True"
        MessageBox MB_YESNO|MB_ICONQUESTION "BarsikCMD уже установлен. Хотите ли вы переустановить его?" IDYES yes
        MessageBox MB_OK|MB_ICONSTOP "Установка отменена."
        Quit
        yes:
        MessageBox MB_OK|MB_ICONINFORMATION "Установка будет продолжена."
    ${EndIf}
FunctionEnd
/*Function RSC
    ReadRegStr $2 HKLM64 "Software\BarsikCMD" "installed"
    ReadRegStr $3 HKLM64 "Software\BarsikCMD" "instdir"
    ${If} $2 == "True"
        MessageBox MB_YESNOCANCEL|MB_ICONQUESTION "Хотите ли вы сохранить данные BarsikCMD?" IDYES yes IDNO no
        Goto cancel
        yes:
        MessageBox MB_OK|MB_ICONINFORMATION "Данные будут сохранены."
        Goto end
        no:
        MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "Данные будут удалены." IDCANCEL cancel
        RMDir /r "$3"
        Goto end
        cancel:
        MessageBox MB_OK|MB_ICONSTOP "Установка была отменена."
        Quit
        end:
    ${EndIf}
FunctionEnd*/


!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "A:\LICENSE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

Section "BarsikCMD" main
    SectionIn RO
    SetOutPath "$INSTDIR"
    #Call RSC
    # code here
    File /r "A:\media"
    File "A:\code\windows\build\BarsikCMD.exe"
    File "A:\code\windows\build\BarsikCMD-prof.exe"
    File /r "A:\code\windows\build\BarsikCMD"
    File "A:\code\windows\build\messages.dll"
    WriteRegStr HKLM64 "Software\BarsikCMD" "installed" "True"
    WriteRegStr HKLM64 "Software\BarsikCMD" "instdir" "$INSTDIR"
    WriteUninstaller "Uninstall.exe"
    
SectionEnd
Section "Обновитель" updater
    File /r "A:\code\windows\build\updater"
SectionEnd

Section "un.BarsikCMD" unbarsikcmd
    Delete "$INSTDIR\BarsikCMD-prof.exe"
    Delete "$INSTDIR\BarsikCMD.exe"
    Delete "$INSTDIR\messages.dll"
    RMDir /r "$INSTDIR\BarsikCMD"
    DeleteRegKey HKLM64 "Software\BarsikCMD"
SectionEnd
Section "un.Обновитель" unupdater
    Delete "$INSTDIR\updater\messages.dll"
    Delete "$INSTDIR\updater\updater.exe"
    RMDir /r "$INSTDIR\updater"
SectionEnd
Section /o "un.Удалить остатки" delALL
    RMDir /r "$INSTDIR"
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${main} "Установит основное приложение."
    !insertmacro MUI_DESCRIPTION_TEXT ${updater} "Установит обновитель."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${unbarsikcmd} "Деинсталлирует основное приложение."
    !insertmacro MUI_DESCRIPTION_TEXT ${unupdater} "Деинсталлирует обновитель."
    !insertmacro MUI_DESCRIPTION_TEXT ${delALL} "НЕ РЕКОМЕНДУЕТСЯ! Удалит все остатки приложения, включая данные."
!insertmacro MUI_UNFUNCTION_DESCRIPTION_END




!insertmacro MUI_LANGUAGE "Russian"