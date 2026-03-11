import sys
import subprocess
import os
from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QLabel, QPushButton, QLineEdit, QTextEdit, QListWidget,
    QListWidgetItem, QSplitter, QFrame, QCheckBox, QDialog,
    QDialogButtonBox, QScrollArea, QMessageBox, QFileDialog,
    QStatusBar, QToolButton, QSizePolicy
)
from PySide6.QtCore import Qt, QThread, Signal, QTimer
from PySide6.QtGui import QFont, QColor, QPalette, QIcon, QTextCursor


# ─── Git worker thread ────────────────────────────────────────────────────────

class GitWorker(QThread):
    finished = Signal(bool, str)

    def __init__(self, commands: list[list[str]], cwd: str):
        super().__init__()
        self.commands = commands
        self.cwd = cwd

    def run(self):
        log = []
        for cmd in self.commands:
            try:
                result = subprocess.run(
                    cmd, cwd=self.cwd,
                    capture_output=True, text=True, encoding="utf-8"
                )
                out = result.stdout.strip()
                err = result.stderr.strip()
                if out:
                    log.append(out)
                if err:
                    log.append(err)
                if result.returncode != 0:
                    self.finished.emit(False, "\n".join(log))
                    return
            except Exception as e:
                self.finished.emit(False, str(e))
                return
        self.finished.emit(True, "\n".join(log))


# ─── Per-file commit dialog ───────────────────────────────────────────────────

class FileCommitDialog(QDialog):
    def __init__(self, filename: str, parent=None):
        super().__init__(parent)
        self.setWindowTitle(f"Коммит для: {filename}")
        self.setMinimumWidth(520)
        self.setStyleSheet(parent.styleSheet() if parent else "")

        layout = QVBoxLayout(self)
        layout.setSpacing(10)
        layout.setContentsMargins(16, 16, 16, 16)

        layout.addWidget(QLabel(f"<b>{filename}</b>"))

        layout.addWidget(QLabel("Краткое описание:"))
        self.short_edit = QLineEdit()
        self.short_edit.setPlaceholderText("feat: добавить новую функцию")
        layout.addWidget(self.short_edit)

        layout.addWidget(QLabel("Подробное описание (необязательно):"))
        self.long_edit = QTextEdit()
        self.long_edit.setPlaceholderText(
            "Детальное описание изменений...\n\nМожно писать несколько абзацев."
        )
        self.long_edit.setMinimumHeight(120)
        layout.addWidget(self.long_edit)

        buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        layout.addWidget(buttons)

    def get_message(self) -> str:
        short = self.short_edit.text().strip()
        long_ = self.long_edit.toPlainText().strip()
        if not short:
            return ""
        if long_:
            return f"{short}\n\n{long_}"
        return short


# ─── File item widget ─────────────────────────────────────────────────────────

class FileItem(QWidget):
    def __init__(self, status: str, filepath: str, parent=None):
        super().__init__(parent)
        self.filepath = filepath
        self.custom_commit: str = ""

        layout = QHBoxLayout(self)
        layout.setContentsMargins(4, 2, 4, 2)
        layout.setSpacing(8)

        self.checkbox = QCheckBox()
        self.checkbox.setChecked(True)
        layout.addWidget(self.checkbox)

        status_colors = {
            "M": "#f0a500", "A": "#4caf7d", "D": "#e05c5c",
            "?": "#888", "R": "#7ec8e3", "C": "#b39ddb",
        }
        status_label = QLabel(status)
        status_label.setFixedWidth(18)
        status_label.setAlignment(Qt.AlignCenter)
        color = status_colors.get(status, "#aaa")
        status_label.setStyleSheet(
            f"color: {color}; font-weight: bold; font-family: monospace;"
        )
        layout.addWidget(status_label)

        self.name_label = QLabel(filepath)
        self.name_label.setStyleSheet("font-family: monospace; font-size: 12px;")
        layout.addWidget(self.name_label, 1)

        self.commit_btn = QToolButton()
        self.commit_btn.setText("✎")
        self.commit_btn.setToolTip("Задать отдельный коммит для этого файла")
        self.commit_btn.setFixedSize(26, 26)
        self.commit_btn.clicked.connect(self._edit_commit)
        layout.addWidget(self.commit_btn)

        self.commit_preview = QLabel()
        self.commit_preview.setStyleSheet("color: #888; font-size: 11px; font-style: italic;")
        self.commit_preview.setMaximumWidth(200)
        self.commit_preview.setWordWrap(False)
        layout.addWidget(self.commit_preview)

    def _edit_commit(self):
        dlg = FileCommitDialog(self.filepath, self.parent())
        if dlg.exec():
            msg = dlg.get_message()
            self.custom_commit = msg
            if msg:
                preview = msg.splitlines()[0]
                if len(preview) > 30:
                    preview = preview[:27] + "..."
                self.commit_preview.setText(preview)
                self.commit_btn.setStyleSheet("color: #4caf7d; font-weight: bold;")
            else:
                self.commit_preview.setText("")
                self.commit_btn.setStyleSheet("")

    def is_checked(self) -> bool:
        return self.checkbox.isChecked()


# ─── Main window ──────────────────────────────────────────────────────────────

DARK_STYLE = """
QMainWindow, QDialog, QWidget {
    background-color: #1a1d23;
    color: #cdd6f4;
}
QFrame#card {
    background-color: #24273a;
    border-radius: 8px;
    border: 1px solid #363a4f;
}
QLabel {
    color: #cdd6f4;
    font-size: 13px;
}
QLabel#title {
    font-size: 22px;
    font-weight: bold;
    color: #89b4fa;
    font-family: "JetBrains Mono", "Fira Code", monospace;
}
QLabel#section {
    font-size: 11px;
    font-weight: bold;
    color: #6c7086;
    letter-spacing: 1.5px;
    text-transform: uppercase;
}
QLineEdit, QTextEdit {
    background-color: #1e2030;
    border: 1px solid #363a4f;
    border-radius: 6px;
    padding: 6px 10px;
    color: #cdd6f4;
    font-size: 13px;
    selection-background-color: #89b4fa;
}
QLineEdit:focus, QTextEdit:focus {
    border: 1px solid #89b4fa;
}
QPushButton#primary {
    background-color: #89b4fa;
    color: #1a1d23;
    border: none;
    border-radius: 8px;
    padding: 10px 24px;
    font-size: 14px;
    font-weight: bold;
}
QPushButton#primary:hover {
    background-color: #a6c8ff;
}
QPushButton#primary:disabled {
    background-color: #363a4f;
    color: #6c7086;
}
QPushButton#secondary {
    background-color: #363a4f;
    color: #cdd6f4;
    border: 1px solid #45475a;
    border-radius: 6px;
    padding: 6px 14px;
    font-size: 12px;
}
QPushButton#secondary:hover {
    background-color: #45475a;
}
QPushButton#danger {
    background-color: #1e2030;
    color: #f38ba8;
    border: 1px solid #f38ba8;
    border-radius: 6px;
    padding: 6px 14px;
    font-size: 12px;
}
QPushButton#danger:hover {
    background-color: #f38ba8;
    color: #1a1d23;
}
QToolButton {
    background-color: transparent;
    border: 1px solid #363a4f;
    border-radius: 4px;
    color: #89b4fa;
    font-size: 14px;
}
QToolButton:hover {
    background-color: #363a4f;
}
QListWidget {
    background-color: #1e2030;
    border: 1px solid #363a4f;
    border-radius: 6px;
    outline: none;
}
QListWidget::item {
    border-radius: 4px;
    padding: 1px;
}
QListWidget::item:selected {
    background-color: #2a2d3e;
}
QScrollBar:vertical {
    background: #1e2030;
    width: 8px;
    border-radius: 4px;
}
QScrollBar::handle:vertical {
    background: #45475a;
    border-radius: 4px;
    min-height: 30px;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QCheckBox::indicator {
    width: 16px; height: 16px;
    border: 1px solid #45475a;
    border-radius: 3px;
    background: #1e2030;
}
QCheckBox::indicator:checked {
    background: #89b4fa;
    border-color: #89b4fa;
    image: none;
}
QStatusBar {
    background: #13141c;
    color: #6c7086;
    font-size: 11px;
    border-top: 1px solid #363a4f;
}
QDialogButtonBox QPushButton {
    background-color: #363a4f;
    color: #cdd6f4;
    border: 1px solid #45475a;
    border-radius: 6px;
    padding: 6px 18px;
    font-size: 13px;
    min-width: 80px;
}
QDialogButtonBox QPushButton:hover {
    background-color: #45475a;
}
"""


class GitManager(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Git Manager")
        self.setMinimumSize(900, 680)
        self.setStyleSheet(DARK_STYLE)

        self.repo_path: str = ""
        self.file_items: list[FileItem] = []
        self.worker: GitWorker | None = None

        self._build_ui()
        self._try_detect_repo()

    # ── UI construction ──────────────────────────────────────────────────────

    def _build_ui(self):
        central = QWidget()
        self.setCentralWidget(central)
        root = QVBoxLayout(central)
        root.setContentsMargins(16, 12, 16, 8)
        root.setSpacing(10)

        # Header
        header = QHBoxLayout()
        title = QLabel("git manager")
        title.setObjectName("title")
        header.addWidget(title)
        header.addStretch()

        self.repo_label = QLabel("репозиторий не выбран")
        self.repo_label.setStyleSheet("color: #6c7086; font-size: 12px; font-family: monospace;")
        header.addWidget(self.repo_label)

        choose_btn = QPushButton("Открыть репо")
        choose_btn.setObjectName("secondary")
        choose_btn.clicked.connect(self._choose_repo)
        header.addWidget(choose_btn)

        refresh_btn = QPushButton("↺ Обновить")
        refresh_btn.setObjectName("secondary")
        refresh_btn.clicked.connect(self._load_status)
        header.addWidget(refresh_btn)

        root.addLayout(header)

        # Main splitter
        splitter = QSplitter(Qt.Horizontal)
        splitter.setHandleWidth(1)
        splitter.setStyleSheet("QSplitter::handle { background: #363a4f; }")

        # ── Left: file list ──────────────────────────────────────────────────
        left = QFrame()
        left.setObjectName("card")
        left_layout = QVBoxLayout(left)
        left_layout.setContentsMargins(12, 12, 12, 12)
        left_layout.setSpacing(8)

        lbl = QLabel("ИЗМЕНЁННЫЕ ФАЙЛЫ")
        lbl.setObjectName("section")
        left_layout.addWidget(lbl)

        sel_row = QHBoxLayout()
        sel_all = QPushButton("Выбрать все")
        sel_all.setObjectName("secondary")
        sel_all.clicked.connect(self._select_all)
        sel_none = QPushButton("Снять все")
        sel_none.setObjectName("secondary")
        sel_none.clicked.connect(self._select_none)
        sel_row.addWidget(sel_all)
        sel_row.addWidget(sel_none)
        sel_row.addStretch()
        left_layout.addLayout(sel_row)

        self.file_list = QListWidget()
        self.file_list.setSpacing(2)
        left_layout.addWidget(self.file_list)

        splitter.addWidget(left)

        # ── Right: commit panel ──────────────────────────────────────────────
        right = QFrame()
        right.setObjectName("card")
        right_layout = QVBoxLayout(right)
        right_layout.setContentsMargins(14, 12, 14, 12)
        right_layout.setSpacing(10)

        lbl2 = QLabel("КОММИТ")
        lbl2.setObjectName("section")
        right_layout.addWidget(lbl2)

        right_layout.addWidget(QLabel("Краткое описание:"))
        self.short_msg = QLineEdit()
        self.short_msg.setPlaceholderText("feat: краткое описание изменений")
        right_layout.addWidget(self.short_msg)

        right_layout.addWidget(QLabel("Подробное описание (необязательно):"))
        self.long_msg = QTextEdit()
        self.long_msg.setPlaceholderText(
            "Детальное описание изменений.\n\n"
            "Что изменилось и почему?\n"
            "Какие побочные эффекты?"
        )
        self.long_msg.setMinimumHeight(100)
        right_layout.addWidget(self.long_msg)

        # Options
        opt_lbl = QLabel("ОПЕРАЦИИ")
        opt_lbl.setObjectName("section")
        right_layout.addWidget(opt_lbl)

        self.cb_add    = QCheckBox("git add (выбранные файлы)")
        self.cb_commit = QCheckBox("git commit")
        self.cb_push   = QCheckBox("git push")
        for cb in (self.cb_add, self.cb_commit, self.cb_push):
            cb.setChecked(True)
            right_layout.addWidget(cb)

        right_layout.addStretch()

        # Push button
        self.run_btn = QPushButton("⚡  Выполнить")
        self.run_btn.setObjectName("primary")
        self.run_btn.setMinimumHeight(44)
        self.run_btn.clicked.connect(self._run_git)
        right_layout.addWidget(self.run_btn)

        splitter.addWidget(right)
        splitter.setSizes([540, 340])
        root.addWidget(splitter, 1)

        # ── Log area ─────────────────────────────────────────────────────────
        log_frame = QFrame()
        log_frame.setObjectName("card")
        log_layout = QVBoxLayout(log_frame)
        log_layout.setContentsMargins(10, 8, 10, 8)
        log_layout.setSpacing(4)

        log_hdr = QHBoxLayout()
        log_lbl = QLabel("ЛОГ")
        log_lbl.setObjectName("section")
        log_hdr.addWidget(log_lbl)
        log_hdr.addStretch()
        clear_log = QPushButton("Очистить")
        clear_log.setObjectName("secondary")
        clear_log.clicked.connect(lambda: self.log_view.clear())
        log_hdr.addWidget(clear_log)
        log_layout.addLayout(log_hdr)

        self.log_view = QTextEdit()
        self.log_view.setReadOnly(True)
        self.log_view.setMaximumHeight(130)
        self.log_view.setStyleSheet(
            "font-family: 'JetBrains Mono', 'Fira Code', 'Courier New', monospace;"
            "font-size: 11px; background: #13141c; border: 1px solid #363a4f;"
        )
        log_layout.addWidget(self.log_view)

        root.addWidget(log_frame)

        # Status bar
        self.status = QStatusBar()
        self.setStatusBar(self.status)
        self.status.showMessage("Готов")

    # ── Repo detection ───────────────────────────────────────────────────────

    def _try_detect_repo(self):
        cwd = os.getcwd()
        try:
            result = subprocess.run(
                ["git", "rev-parse", "--show-toplevel"],
                cwd=cwd, capture_output=True, text=True
            )
            if result.returncode == 0:
                path = result.stdout.strip()
                self._set_repo(path)
        except Exception:
            pass

    def _choose_repo(self):
        path = QFileDialog.getExistingDirectory(self, "Выбрать репозиторий", os.getcwd())
        if path:
            self._set_repo(path)

    def _set_repo(self, path: str):
        self.repo_path = path
        short = path if len(path) <= 50 else "…" + path[-47:]
        self.repo_label.setText(short)
        self._load_status()

    # ── Git status ───────────────────────────────────────────────────────────

    def _load_status(self):
        if not self.repo_path:
            return
        try:
            result = subprocess.run(
                ["git", "status", "--porcelain"],
                cwd=self.repo_path, capture_output=True, text=True, encoding="utf-8"
            )
            lines = [l for l in result.stdout.splitlines() if l.strip()]
            self._populate_files(lines)
            self._log(f"git status: {len(lines)} изменений")
        except Exception as e:
            self._log(f"Ошибка: {e}", error=True)

    def _populate_files(self, lines: list[str]):
        self.file_list.clear()
        self.file_items.clear()

        for line in lines:
            if len(line) < 3:
                continue
            xy = line[:2]
            filepath = line[3:].strip()
            # pick most meaningful status char
            status = xy[1] if xy[1] != " " else xy[0]

            item_widget = FileItem(status, filepath, self)
            list_item = QListWidgetItem(self.file_list)
            list_item.setSizeHint(item_widget.sizeHint())
            self.file_list.addItem(list_item)
            self.file_list.setItemWidget(list_item, item_widget)
            self.file_items.append(item_widget)

        self.status.showMessage(f"{len(lines)} файлов с изменениями")

    # ── Select all / none ────────────────────────────────────────────────────

    def _select_all(self):
        for fi in self.file_items:
            fi.checkbox.setChecked(True)

    def _select_none(self):
        for fi in self.file_items:
            fi.checkbox.setChecked(False)

    # ── Run git ──────────────────────────────────────────────────────────────

    def _run_git(self):
        if not self.repo_path:
            QMessageBox.warning(self, "Ошибка", "Репозиторий не выбран.")
            return

        checked = [fi for fi in self.file_items if fi.is_checked()]
        if not checked:
            QMessageBox.warning(self, "Ошибка", "Не выбрано ни одного файла.")
            return

        short = self.short_msg.text().strip()
        long_ = self.long_msg.toPlainText().strip()

        if self.cb_commit.isChecked() and not short:
            # Check if all files have individual commits
            missing = [fi for fi in checked if not fi.custom_commit]
            if missing:
                QMessageBox.warning(
                    self, "Ошибка",
                    "Укажите краткое описание коммита или задайте отдельные коммиты для каждого файла."
                )
                return

        commands: list[list[str]] = []

        # Determine commit strategy
        # Files with custom commits → individual commits
        # Files without custom commits → grouped under global message
        individual = [fi for fi in checked if fi.custom_commit]
        grouped    = [fi for fi in checked if not fi.custom_commit]

        if self.cb_add.isChecked():
            if grouped:
                commands.append(["git", "add", "--"] + [fi.filepath for fi in grouped])
            for fi in individual:
                commands.append(["git", "add", "--", fi.filepath])

        if self.cb_commit.isChecked():
            # Commit grouped files with global message
            if grouped and short:
                global_msg = f"{short}\n\n{long_}" if long_ else short
                commands.append(["git", "commit", "-m", global_msg,
                                  "--"] + [fi.filepath for fi in grouped])
            # Commit individual files
            for fi in individual:
                commands.append(["git", "commit", "-m", fi.custom_commit, "--", fi.filepath])

        if self.cb_push.isChecked():
            commands.append(["git", "push"])

        self._log("─" * 40)
        for cmd in commands:
            self._log("$ " + " ".join(cmd))

        self.run_btn.setEnabled(False)
        self.status.showMessage("Выполняю…")

        self.worker = GitWorker(commands, self.repo_path)
        self.worker.finished.connect(self._on_done)
        self.worker.start()

    def _on_done(self, ok: bool, output: str):
        self.run_btn.setEnabled(True)
        if output:
            self._log(output, error=not ok)
        if ok:
            self.status.showMessage("✓ Выполнено успешно")
            self._log("✓ Готово", success=True)
            self._load_status()
        else:
            self.status.showMessage("✗ Ошибка")
            QMessageBox.critical(self, "Ошибка Git", output or "Неизвестная ошибка")

    # ── Log helper ───────────────────────────────────────────────────────────

    def _log(self, text: str, error: bool = False, success: bool = False):
        cursor = self.log_view.textCursor()
        cursor.movePosition(QTextCursor.End)
        if error:
            fmt = '<span style="color:#f38ba8;">{}</span><br>'
        elif success:
            fmt = '<span style="color:#a6e3a1;">{}</span><br>'
        else:
            fmt = '<span style="color:#cdd6f4;">{}</span><br>'
        cursor.insertHtml(fmt.format(text.replace("<", "&lt;").replace(">", "&gt;")))
        self.log_view.setTextCursor(cursor)
        self.log_view.ensureCursorVisible()


# ─── Entry point ──────────────────────────────────────────────────────────────

if __name__ == "__main__":
    app = QApplication(sys.argv)
    app.setApplicationName("Git Manager")
    window = GitManager()
    window.show()
    sys.exit(app.exec())