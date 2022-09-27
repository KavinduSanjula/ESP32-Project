from PyQt5 import QtWidgets, QtCore, uic
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import sys
import serial
import time

class Controller(QtCore.QThread):
    def __init__(self,parent):
        super().__init__()
        self.ui = parent
        self.port = None
        self.connected = True

        self.temp = 0
        self.light = 0
        self.hour = 0
        self.minute = 0
        self.second = 0

        self.time = ''


    def run(self):
        while self.connected:
            if not self.port: return
            
            line = self.port.readline().decode().strip()

            self.hour, self.minute, self.second, self.temp, self.light = line.split(',')
            self.time = f'{self.hour}:{self.minute}:{self.second}'

            self.ui.graph_time.append(self.time)
            self.ui.graph_temp.append(self.temp)
            self.ui.graph_light.append(self.light)

            self.ui.update_ui()
            time.sleep(1)

    def connect(self,com_port):
        try:
            self.port = serial.Serial(com_port,115200)
            self.connected = True
            self.start()
            return True
        except:
            self.connected = False
            return False
        
    def disconnect(self):
        self.port.close()
        self.connected = False


class Ui(QtWidgets.QMainWindow):
    def __init__(self):
        super(Ui, self).__init__()
        uic.loadUi('esp32_gui.ui', self)
        self.show()

        self.controller = Controller(self)

        self.graph_time = []
        self.graph_temp = []
        self.graph_light = []
        
        #button connections
        self.btn_connect.clicked.connect(self.connect)
        self.btn_disconnect.clicked.connect(self.disconnect)
        self.btn_show_in_graph.clicked.connect(self.show_in_graph)

        
    def connect(self):
        self.log("Connecting...")

        #connect to esp32
        connected = self.controller.connect(self.tb_port.text())

        if connected:
            self.btn_connect.setEnabled(False)
            self.btn_disconnect.setEnabled(True)
            self.label_status.setText('Connected')
            self.label_status.setStyleSheet('color:green;')
            self.log('Connected')
        else:
            self.log('Error in connection!')

    def disconnect(self):
        self.log("Disconnecting...")

        #disconnect from 
        self.controller.disconnect()

        self.btn_connect.setEnabled(True)
        self.btn_disconnect.setEnabled(False)
        self.label_status.setText('Disconnected')
        self.label_status.setStyleSheet('color:red;')
        self.log('Disconnected')


    def show_in_graph(self):
        self.log('opening graph...')

        fig = plt.figure('ESP32 Sensor Values')
        ax = fig.add_subplot(1,1,1)

        def animate(i):
            self.graph_time = self.graph_time[-10:]
            self.graph_temp = self.graph_temp[-10:]
            self.graph_light = self.graph_light[-10:]

            ax.clear()
            
            ax.plot(self.graph_time,self.graph_temp)
            ax.plot(self.graph_time,self.graph_light)

            ax.legend(['Temprature','Light Level'])
            ax.set_xlabel('Time')
            ax.set_ylabel('Temprature/Light Level')
            ax.set_title('ESP32 Sensor Values')

        self.ani = animation.FuncAnimation(fig,animate,interval=500)
        plt.show()


    def write_to_file(self):
        self.log('writing data to file...')

        file = open('esp32-logfile.txt','a')
        file.write(f'{self.controller.time}\ttemprature: {self.controller.temp} \'C\tlight-level: {self.controller.light}\n')
        file.close()

    def update_ui(self):
        self.label_temp.setText(str(self.controller.temp)+" 'C")
        self.label_light.setText(str(self.controller.light))
        self.label_time.setText(str(self.controller.time))

        self.write_to_file()
        
        self.log(f'Reciving Data...')

    def log(self,msg):
        self.label_log.setText(str(msg))



app = QtWidgets.QApplication(sys.argv)
window = Ui()
app.exec_()