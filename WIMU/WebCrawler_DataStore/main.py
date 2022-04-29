from PyQt5 import QtCore, QtGui, QtWidgets
import sys
import threading

from view import MainWinUI as ui
from src.crawler import crawler_run
from src.load import load2_elasticsearch
from src.search import query


class Main(QtWidgets.QMainWindow, ui.Ui_Form):
    def __init__(self):
        super().__init__()
        self.setupUi(self)

        # my class var
        # "idle", "crawler", "import", "search"
        self.status = "idle"
        self.stop_thread = True

        # defining members with function
        self.btn_crawler.clicked.connect(
            self.crawler_clicker
        )
        self.btn_import.clicked.connect(self.import_clicker)
        self.btn_search.clicked.connect(self.search_clicker)
        self.btn_stop.clicked.connect(self.stop_clicker)

    def crawler_clicker(self):
        if self.status == "idle":
            self.status = "crawler"
            self.stop_thread = False
            self.t = threading.Thread(
                target=crawler_run,
                args=(
                    self.sb_crawlerCount.value(),
                    lambda: self.stop_thread,
                ),
            )
            self.t.start()

    def import_clicker(self):
        if self.status == "idle":
            self.status = "import"
            print_to_logger("w", "", end="")
            self.tn = threading.Thread(
                target=load2_elasticsearch,
            )
            self.tn.start()
            self.tn.join()
            self.status = "idle"

    def search_clicker(self):
        if self.status == "idle":
            if self.pte_search_text.value() != "":
                self.status = "search"
                # print_
                self.tn = threading.Thread(
                    target=query,
                    args=(self.pte_search_text.value()),
                )
                self.tn.start()
                self.tn.join()
                self.status = "idle"

    def stop_clicker(self):
        if self.status == "crawler":
            self.stop_thread = True
            self.t.join()
            self.status = "idle"

    def print_to_logger(self, way, text, end="\n"):
        if way == "a":
            self.te_print.append(text + end)
        elif way == "w":
            self.te_print.setText(text + end)


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    win = Main()
    win.show()
    sys.exit(app.exec_())
