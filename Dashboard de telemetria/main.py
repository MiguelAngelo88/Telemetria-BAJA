import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget
from Painel import Ui_painel
from velocimetro_widget import VelocimetroWidget
from rpm_widget import RPMWidget
from bateria_widget import BateriaWidget
from temperatura_widget import TemperaturaWidget
from temperatura_freio_widget import TemperaturaFreioWidget


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.ui = Ui_painel()
        self.ui.setupUi(self)

        # Velocímetro
        layout_velocimetro = QVBoxLayout(self.ui.Velocimetro)
        self.velocimetro = VelocimetroWidget(self.ui.Velocimetro)
        layout_velocimetro.addWidget(self.velocimetro)
        self.velocimetro.setVelocidade(180)

        # RPM
        layout_rpm = QVBoxLayout(self.ui.RPM)
        self.rpm = RPMWidget(self.ui.RPM)
        layout_rpm.addWidget(self.rpm)
        self.rpm.setRPM(4500)

        # Bateria
        layout_bateria = QVBoxLayout(self.ui.Bateria)
        self.bateria = BateriaWidget(self.ui.Bateria)
        layout_bateria.addWidget(self.bateria)
        self.bateria.setNivel(65)

        # Temperatura CVT
        layout_temperatura = QVBoxLayout(self.ui.Temperatura)
        self.temperatura = TemperaturaWidget(self.ui.Temperatura)
        layout_temperatura.addWidget(self.temperatura)
        self.temperatura.setTemperatura(95)

        #Temperatura freio
        layout_temperatura_freio = QVBoxLayout(self.ui.Temperatura_freio)
        self.temperatura_freio = TemperaturaFreioWidget(self.ui.Temperatura_freio)
        layout_temperatura_freio.addWidget(self.temperatura_freio)
        self.temperatura_freio.setTemperatura(60)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    janela = MainWindow()
    janela.showMaximized()
    sys.exit(app.exec_())
