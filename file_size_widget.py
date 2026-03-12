import sys
import os
import signal
from PySide6.QtWidgets import QApplication, QWidget, QVBoxLayout, QLabel, QFileDialog
from PySide6.QtCore import Qt, QTimer, QPoint
from PySide6.QtGui import QFont, QColor, QPainter, QBrush, QPen, QMouseEvent, QShortcut, QKeySequence


def format_size(size_bytes: int) -> str:
    for unit in ("B", "KB", "MB", "GB", "TB"):
        if size_bytes < 1024:
            return f"{size_bytes:.2f} {unit}"
        size_bytes /= 1024
    return f"{size_bytes:.2f} PB"


class OverlayWidget(QWidget):
    def __init__(self, file_path: str):
        super().__init__()

        self.file_path = file_path

        self.setWindowFlags(
            Qt.WindowType.FramelessWindowHint |
            Qt.WindowType.WindowStaysOnTopHint |
            Qt.WindowType.Tool
        )
        self.setAttribute(Qt.WidgetAttribute.WA_TranslucentBackground)
        self.setFixedSize(240, 80)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(16, 12, 16, 12)
        layout.setSpacing(2)

        self.label_name = QLabel()
        self.label_name.setFont(QFont("Consolas", 8))
        self.label_name.setStyleSheet("color: rgba(180, 180, 180, 220);")
        self.label_name.setAlignment(Qt.AlignmentFlag.AlignLeft)

        self.label_size = QLabel("—")
        self.label_size.setFont(QFont("Consolas", 20, QFont.Weight.Bold))
        self.label_size.setStyleSheet("color: rgba(0, 230, 180, 255);")
        self.label_size.setAlignment(Qt.AlignmentFlag.AlignLeft)

        layout.addWidget(self.label_name)
        layout.addWidget(self.label_size)

        self._drag_pos = QPoint()

        # Ctrl+C — выход
        shortcut = QShortcut(QKeySequence("Ctrl+C"), self)
        shortcut.activated.connect(QApplication.quit)

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_size)
        self.timer.start(500)

        self.set_file(self.file_path)

    def set_file(self, path: str):
        self.file_path = path
        name = os.path.basename(path)
        if len(name) > 28:
            name = name[:25] + "..."
        self.label_name.setText(name)
        self.update_size()

    def update_size(self):
        try:
            size = os.path.getsize(self.file_path)
            self.label_size.setText(format_size(size))
            self.label_size.setStyleSheet("color: rgba(0, 230, 180, 255);")
        except FileNotFoundError:
            self.label_size.setText("not found")
            self.label_size.setStyleSheet("color: rgba(255, 80, 80, 220);")
        except Exception:
            self.label_size.setText("error")
            self.label_size.setStyleSheet("color: rgba(255, 80, 80, 220);")

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)
        painter.setBrush(QBrush(QColor(20, 20, 28, 210)))
        painter.setPen(QPen(QColor(0, 230, 180, 80), 1))
        painter.drawRoundedRect(self.rect().adjusted(1, 1, -1, -1), 10, 10)

    def mousePressEvent(self, event: QMouseEvent):
        if event.button() == Qt.MouseButton.LeftButton:
            self._drag_pos = event.globalPosition().toPoint() - self.frameGeometry().topLeft()

    def mouseMoveEvent(self, event: QMouseEvent):
        if event.buttons() & Qt.MouseButton.LeftButton:
            self.move(event.globalPosition().toPoint() - self._drag_pos)

    def mouseDoubleClickEvent(self, event: QMouseEvent):
        # Двойной клик — выбор нового файла
        path, _ = QFileDialog.getOpenFileName(None, "Выбрать файл", os.path.dirname(self.file_path))
        if path:
            self.set_file(path)


if __name__ == "__main__":
    app = QApplication(sys.argv)

    # Ctrl+C в терминале тоже завершает процесс
    signal.signal(signal.SIGINT, lambda *_: QApplication.quit())
    # Пульс чтобы Python успевал обрабатывать SIGINT
    pulse = QTimer()
    pulse.start(200)
    pulse.timeout.connect(lambda: None)

    # Выбор файла при старте
    start_dir = r"C:\Users\usarn\Videos"
    file_path, _ = QFileDialog.getOpenFileName(None, "Выбрать файл для мониторинга", start_dir)
    if not file_path:
        sys.exit(0)

    widget = OverlayWidget(file_path)
    widget.show()
    sys.exit(app.exec())