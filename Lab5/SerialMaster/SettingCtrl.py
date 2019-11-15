import configparser
import os
import codecs

class SettingCtrl():

    def __init__(self):
        self.fileName = "setting.ini"
        # Load Setting
        self.check()

    def check(self):
        if os.path.exists(self.fileName):
            pass
        else:
            # Create New One
            cfg = configparser.ConfigParser()

            cfg.add_section("curve")
            cfg.set("curve", "x_len", "500")

            cfg.add_section("serial")
            cfg.set("serial", "bps", "115200")
            cfg.set("serial", "port", "COM1")

            # Write File
            cfg.write(open(self.fileName,'w'))

    def save(self, section, param, value):
        cfg = configparser.ConfigParser()
        cfg.readfp(codecs.open(self.fileName, encoding="utf-8-sig"))
        cfg.set(section, param, value)
        cfg.write(open(self.fileName,'w'))

    def load(self, section, param):
        cfg = configparser.ConfigParser()
        cfg.readfp(codecs.open(self.fileName, encoding="utf-8-sig"))
        result = cfg.get(section, param)
        return result
