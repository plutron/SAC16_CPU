import sys
from PySide6.QtWidgets import QApplication
from window import MainWindow

def main():
    app = QApplication(sys.argv)
    app.setApplicationName("SAC16 Studio")
    app.setStyle("Fusion")
    window = MainWindow()
    window.show()
    sys.exit(app.exec())

if __name__ == "__main__":
    main()
