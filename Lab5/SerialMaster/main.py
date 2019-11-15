# -*- coding: utf-8 -*-

"""
Module implementing MainWindow.
"""

import sys
import datetime
import binascii

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5 import *

from Ui_main import Ui_MainWindow
from drawCurve import dialogDrawCurve

from Process import *
from serial_ctrl import *
from SettingCtrl import *

# 接收数据线程
class ReceiveThread(QThread):
    finishSignal = pyqtSignal(bytes)
    def __init__(self, parent=None):
        super(ReceiveThread, self).__init__(parent)

    def run(self):

        global g_rec_run, g_serial

        while g_rec_run:
            time.sleep(0.001)
            data = None
            try:
                data = g_serial.read_all()
            except Exception as e:
                print(e)
            if data is not  None:
                self.finishSignal.emit(data)

class MainWindow(QMainWindow, Ui_MainWindow):
    """
    Class documentation goes here.
    """
    def __init__(self, parent=None):
        """
        Constructor
        
        @param parent reference to the parent widget
        @type QWidget
        """
        super(MainWindow, self).__init__(parent)
        self.setupUi(self)

        # 参数读取
        self.params = SettingCtrl()
        self.load_params()

        # 动态绘制曲线窗口
        self.dlg_drawCurve = None
        
        # 接收 ASCII 或 HEX 显示
        self.ReceiveAsHex = False
        # 接收 自动换行
        self.ReceiveAutoNewLine = False
        # 接收 显示时间
        self.ReceiveAddTime = False
        # 接收 计数
        self.ReceiveCnt = 0

        # 发送 ASCII 或 HEX 显示
        self.SendAsHex = False
        # 发送 计数
        self.SendCnt = 0

    def load_params(self):
        """
        读取参数
        """
        self.get_serial()

        self.bps = self.params.load("serial", "bps")
        self.port = self.params.load("serial", "port")

        self.comboBox_BaundRate.setCurrentText(str(self.bps))
        self.port = self.comboBox_Port.currentText()

    def get_serial(self):
        """
        获取串口列表
        """
        plist = list(serial.tools.list_ports.comports())
        if len(plist) > 0:
            for x in list(plist):
                self.comboBox_Port.addItem(x[0])

    def print_receive(self, raw_data):
        """
        打印串口接收到的数据
        """

        global g_rec_cnt

        # 如果没有数据，就不要麻烦下面的操作了
        if len(raw_data) == 0:
            return
        else:
            g_rec_cnt += len(raw_data)
            self.label_ReceiveCnt.setText(str(g_rec_cnt))

        msg = ""

        # 是否十六进制显示
        if self.ReceiveAsHex is True:
            result = ''  
            hLen = len(raw_data)  
            for i in range(hLen):  
                hvol = raw_data[i]
                hhex = '%02X'%hvol  
                result += hhex+' '
            msg = result
        else:
            msg = raw_data.decode('utf-8', 'ignore')

        # 是否添加时间
        if self.ReceiveAddTime is True:
            now = datetime.datetime.now()
            otherStyleTime = now.strftime("%Y-%m-%d %H:%M:%S")
            msg = otherStyleTime + " - " + msg
            # 这里添加了时间后，默认自动换行了
            msg += "\r\n"
        else:
            # 是否换行
            if self.ReceiveAutoNewLine is True:
                msg += "\r\n"

        # 光标移动到最后
        cursor =  self.textEdit_Receive.textCursor()
        cursor.movePosition(QtGui.QTextCursor.End)
        self.textEdit_Receive.setTextCursor(cursor)

        # 输出显示
        self.textEdit_Receive.insertPlainText(msg)

    def close_serial(self):
        """
        关闭串口
        """
        global g_serial, g_rec_run, g_port_is_open

        # 关闭接收线程
        g_rec_run = False
        # 全部变量关闭
        g_port_is_open = False

        # 主窗口
        close_port(g_serial)
        self.statusBar.showMessage("关闭串口 : " + str(self.port))
        self.pushButton_Open.setText("打开")

    def open_serial(self):
        """
        打开串口
        """
        global g_serial, g_rec_run, g_port_is_open

        g_serial = open_port(self.port, self.bps)

        if g_serial is not None:

            g_port_is_open = True

            self.statusBar.showMessage("打开串口成功 : " + str(self.port))
            self.pushButton_Open.setText("关闭")

            # 接收线程启动
            self.t_rec = ReceiveThread()
            self.t_rec.finishSignal.connect(self.print_receive)
            self.t_rec.start()
            g_rec_run = True

            # 计数器清零
            self.ReceiveCnt = 0

        else:
            self.statusBar.showMessage("打开串口失败 : " + str(self.port))

    def open_or_close_port(self):
        """
        打开或关闭串口
        """

        global g_serial, g_rec_run, g_port_is_open

        if g_port_is_open:

            # 关闭接收线程
            g_rec_run = False
            # 全部变量关闭
            g_port_is_open = False

            # 主窗口
            close_port(g_serial)
            self.statusBar.showMessage("关闭串口 : " + str(self.port))
            self.pushButton_Open.setText("打开")

        else:

            g_serial = open_port(self.port, self.bps)

            if g_serial is not None:

                g_port_is_open = True

                self.statusBar.showMessage("打开串口成功 : " + str(self.port))
                self.pushButton_Open.setText("关闭")

                # 接收线程启动
                self.t_rec = ReceiveThread()
                self.t_rec.finishSignal.connect(self.print_receive)
                self.t_rec.start()
                g_rec_run = True

                # 计数器清零
                self.ReceiveCnt = 0

            else:
                self.statusBar.showMessage("打开串口失败 : " + str(self.port))
    
    @pyqtSlot(int)
    def on_comboBox_FlowCtrl_currentIndexChanged(self, index):
        """
        流控
        """
        pass
        
    @pyqtSlot(int)
    def on_comboBox_StopBit_currentIndexChanged(self, index):
        """
        停止位
        """
        pass
    
    @pyqtSlot(str)
    def on_comboBox_Port_currentIndexChanged(self, p0):
        """
        串口调整
        """
        global g_serial, g_rec_run, g_port_is_open

        self.port = p0

        if g_port_is_open:
            self.close_serial()
            self.open_serial()
    
    @pyqtSlot(int)
    def on_comboBox_DataLen_currentIndexChanged(self, index):
        """
        数据位
        """
        pass
    
    @pyqtSlot(int)
    def on_comboBox_CheckSum_currentIndexChanged(self, index):
        """
        校验位
        """
        pass
    
    @pyqtSlot()
    def on_pushButton_Open_clicked(self):
        """
        打开/关闭 串口
        """
        self.open_or_close_port()
    
    @pyqtSlot()
    def on_pushButton_RefreshPort_clicked(self):
        """
        刷新串口
        """
        self.comboBox_Port.clear()
        self.get_serial()
        self.statusBar.showMessage("刷新串口成功")
    
    @pyqtSlot()
    def on_pushButton_Send_clicked(self):
        """
        发送
        """

        global g_port_is_open, g_serial, g_snd_cnt

        raw_data = self.textEdit_Send.toPlainText()

        if len(raw_data) == 0:
            self.statusBar.showMessage("没有要发送的数据")
            return

        # 确认串口已经打开才能发送数据
        if g_port_is_open is True:
            
            # 是否HEX发送
            if self.SendAsHex is True:
                msg = Str2Hex(raw_data)
                g_serial.write(bytes(msg))
            else:
                g_serial.write(raw_data.encode('utf-8'))

            # 计数
            g_snd_cnt += len(raw_data)
            self.label_SendCnt.setText(str(g_snd_cnt))

    @pyqtSlot(str)
    def on_comboBox_BaundRate_currentIndexChanged(self, p0):
        """
        波特率
        """
        self.bps = int(p0)

        self.params.save("serial", "bps", str(self.bps))
        
    @pyqtSlot(bool)
    def on_checkBox_ReceiveAutoNewLine_clicked(self, checked):
        """
        自动换行
        """
        self.ReceiveAutoNewLine = checked
    
    @pyqtSlot(bool)
    def on_checkBox_ReceiveShowTime_clicked(self, checked):
        """
        显示时间
        """
        self.ReceiveAddTime = checked
        
    @pyqtSlot(bool)
    def on_checkBox_ReceiveHEX_clicked(self, checked):
        """
        十六进制接收
        """
        self.ReceiveAsHex = checked
    
    @pyqtSlot(bool)
    def on_checkBox_SendHEX_clicked(self, checked):
        """
        十六进制发送
        """
        self.SendAsHex = checked
    
    @pyqtSlot()
    def on_pushButton_ReceiveClearCnt_clicked(self):
        """
        接收清零
        """
        global g_rec_cnt

        g_rec_cnt = 0
        self.textEdit_Receive.setText("")
        self.label_ReceiveCnt.setText("0")
    
    @pyqtSlot()
    def on_pushButton_SendClearCnt_clicked(self):
        """
        发送清零
        """
        global g_snd_cnt

        g_snd_cnt = 0
        self.textEdit_Send.setText("")
        self.label_SendCnt.setText("0")
    
    @pyqtSlot()
    def on_actionDrawCurve_triggered(self):
        """
        动态绘制曲线窗口
        """

        global g_port_is_open, g_rec_run, g_serial

        # 如果串口已经打开，要关闭
        if g_port_is_open:

            # 关闭接收线程
            g_rec_run = False
            # 全部变量关闭
            g_port_is_open = False

            close_port(g_serial)
            self.statusBar.showMessage("关闭串口")
            self.pushButton_Open.setText("打开")

        # 参数传递
        self.dlg_drawCurve = None
        self.dlg_drawCurve = dialogDrawCurve()
        self.dlg_drawCurve.port = self.comboBox_Port.currentText()
        self.dlg_drawCurve.bps = int(self.comboBox_BaundRate.currentText())

        # 打开窗口
        self.dlg_drawCurve.show()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    main = MainWindow()
    main.show()
    sys.exit(app.exec_())
