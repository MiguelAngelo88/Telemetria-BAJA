from PyQt5.QtWidgets import QWidget
from PyQt5.QtGui import QPainter, QColor, QPen, QFont
from PyQt5.QtCore import Qt

class BateriaWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.nivel = 0  # nível da bateria em %

    def setNivel(self, valor):
        self.nivel = max(0, min(valor, 100))
        self.update()

    def paintEvent(self, event):

        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)

        largura = self.width()
        altura = self.height()

        bateria_largura = largura * 0.5
        bateria_altura = altura * 0.3

        corpo_largura = int(bateria_largura * 0.85)
        corpo_altura = int(bateria_altura)
        corpo_x = (largura - corpo_largura) // 2
        corpo_y = (altura - corpo_altura) // 2

        pino_largura = int(bateria_largura * 0.15)
        pino_altura = int(bateria_altura * 0.5)
        pino_x = corpo_x + corpo_largura
        pino_y = corpo_y + (corpo_altura - pino_altura) // 2

        # Cor do contorno baseado no nível da bateria
        if self.nivel > 60:
            cor_contorno = QColor(0, 200, 0)  # verde
        elif self.nivel > 30:
            cor_contorno = QColor(255, 255, 0)  # amarelo
        else:
            cor_contorno = QColor(255, 0, 0)  # vermelho

        # Corpo da bateria
        painter.setPen(QPen(cor_contorno, 3))
        painter.setBrush(QColor(255, 255, 255))
        painter.drawRect(corpo_x, corpo_y, corpo_largura, corpo_altura)

        # Pino da bateria — agora com contorno que muda de cor igual ao corpo
        painter.setPen(QPen(cor_contorno, 2))
        painter.setBrush(Qt.NoBrush)
        painter.drawRect(pino_x, pino_y, pino_largura, pino_altura)

        # Símbolos "-" e "+"
        painter.setPen(QPen(cor_contorno, 2))
        font = QFont("Arial", 10, QFont.Bold)
        painter.setFont(font)

        menos_x = corpo_x + 6
        menos_y = corpo_y + corpo_altura // 2 + 2
        painter.drawText(menos_x, menos_y - 7, 12, 12, Qt.AlignCenter, "-")

        mais_x = corpo_x + corpo_largura - 20
        mais_y = menos_y
        painter.drawText(mais_x, mais_y - 7, 12, 12, Qt.AlignCenter, "+")

        # Texto do nível
        painter.setPen(Qt.black)
        font_text = QFont("Arial", 8, QFont.Bold)
        painter.setFont(font_text)
        texto_rect = self.rect()
        margin_bottom = 75
        adjusted_rect = texto_rect.adjusted(0, 0, 0, -margin_bottom)
        painter.drawText(adjusted_rect, Qt.AlignBottom | Qt.AlignHCenter, f"Nível: {self.nivel}%")
